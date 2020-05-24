/**
 * @file gameboy.c
 * @brief Gameboy for GameBoy Emulator
 *
 * @author C la vie
 * @date 2020
 */

#include "gameboy.h"
#include "error.h"
#include "bootrom.h"
#include "cpu-storage.h"

#ifdef BLARGG
static int blargg_bus_listener(gameboy_t *gameboy, addr_t addr)
{
    M_REQUIRE_NON_NULL(gameboy);

    if (addr == BLARGG_REG)
        printf("%c", cpu_read_at_idx(&gameboy->cpu, addr));

    return ERR_NONE;
}
#endif

// ----------------------------------------------------------------------
/**
 * @brief init the specified X component of the gameboy 
 */
#define INIT_COMPONENT(X, i)                                           \
    do                                                                 \
    {                                                                  \
        component_t c;                                                 \
        M_EXIT_IF_ERR(component_create(&c, MEM_SIZE(X)));              \
        gameboy->components[i] = c;                                    \
        M_EXIT_IF_ERR(bus_plug(gameboy->bus, &c, X##_START, X##_END)); \
    } while (0)

int gameboy_create(gameboy_t *gameboy, const char *filename)
{
    M_REQUIRE_NON_NULL(gameboy);

    memset(&gameboy->bus, 0, (BUS_SIZE * sizeof(data_t *)));

    M_EXIT_IF_ERR(cartridge_init(&gameboy->cartridge, filename));
    M_EXIT_IF_ERR(cartridge_plug(&gameboy->cartridge, gameboy->bus));

    gameboy->boot = 1u;
    M_EXIT_IF_ERR(bootrom_init(&gameboy->bootrom));
    M_EXIT_IF_ERR(bootrom_plug(&gameboy->bootrom, gameboy->bus));

    M_EXIT_IF_ERR(timer_init(&gameboy->timer, &gameboy->cpu));

    int i = 0;
    INIT_COMPONENT(WORK_RAM, i++);
    INIT_COMPONENT(REGISTERS, i++);
    INIT_COMPONENT(EXTERN_RAM, i++);
    INIT_COMPONENT(VIDEO_RAM, i++);
    INIT_COMPONENT(GRAPH_RAM, i++);
    INIT_COMPONENT(USELESS, i++);

    component_t echo_ram;
    /* On n'ajoute pas echo_ram à la liste components car il partage
     * la même mémoire que work_ram cela permet d'eviter de free deux fois
     * la même zone mémoire dans gameboy_free */
    M_EXIT_IF_ERR(component_shared(&echo_ram, &gameboy->components[0]));
    M_EXIT_IF_ERR(bus_plug(gameboy->bus, &echo_ram, ECHO_RAM_START, ECHO_RAM_END));

    M_EXIT_IF_ERR(cpu_init(&gameboy->cpu));
    M_EXIT_IF_ERR(cpu_plug(&gameboy->cpu, &gameboy->bus));

    gameboy->cycles = 0;

    return ERR_NONE;
}

void gameboy_free(gameboy_t *gameboy)
{
    if (gameboy != NULL)
    {
        // Unplug a clone of echo_ram
        component_t echo_clone = {NULL, ECHO_RAM_START, ECHO_RAM_END};
        RETURN_IF_ERROR_MSG_ONLY(bus_unplug(gameboy->bus, &echo_clone));

        for (size_t i = 0; i < GB_NB_COMPONENTS; ++i)
        {
            RETURN_IF_ERROR_MSG_ONLY(bus_unplug(gameboy->bus, &gameboy->components[i]));
            component_free(&gameboy->components[i]);
        }

        RETURN_IF_ERROR_MSG_ONLY(bus_unplug(gameboy->bus, &gameboy->bootrom));
        component_free(&gameboy->bootrom);

        RETURN_IF_ERROR_MSG_ONLY(bus_unplug(gameboy->bus, &gameboy->cartridge.c));
        cartridge_free(&gameboy->cartridge);
    }
}

int gameboy_run_until(gameboy_t *gameboy, uint64_t cycle)
{
    while (gameboy->cycles < cycle)
    {
        M_EXIT_IF_ERR(timer_cycle(&gameboy->timer));
        M_EXIT_IF_ERR(cpu_cycle(&gameboy->cpu));
        M_EXIT_IF_ERR(bootrom_bus_listener(gameboy, gameboy->cpu.write_listener));
        M_EXIT_IF_ERR(timer_bus_listener(&gameboy->timer, gameboy->cpu.write_listener));

#ifdef BLARGG
        M_EXIT_IF_ERR(blargg_bus_listener(gameboy, gameboy->cpu.write_listener));
#endif

        gameboy->cycles++;
    }
    return ERR_NONE;
}
