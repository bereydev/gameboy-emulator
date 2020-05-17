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

// ----------------------------------------------------------------------
/**
 * @brief returns with error code only if function does not return ERR_NONE
 */
#define RETURN_IF_ERROR(func)                         \
    do                                                \
    {                                                 \
        int ret_error = func;                         \
        if (ret_error != ERR_NONE)                    \
        {                                             \
            M_EXIT(ret_error, "Function has failed"); \
        }                                             \
    } while (0)

// ----------------------------------------------------------------------
/**
 * @brief returns a message without error code only if function does not return ERR_NONE (designed for void functions)
 */
#define RETURN_IF_ERROR_MSG_ONLY(func)                \
    do                                                \
    {                                                 \
        int ret_error = func;                         \
        if (ret_error != ERR_NONE)                    \
        {                                             \
            debug_print("Function has failed"); \
        }                                             \
    } while (0)

#define DRAW_IMAGE_CYCLES ((uint64_t)17556)

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
#define INIT_COMPONENT(X, i)                            \
    do                                                  \
    {                                                   \
        component_t c;                                  \
        component_create(&c, MEM_SIZE(X));              \
        gameboy->components[i] = c;                     \
        bus_plug(gameboy->bus, &c, X##_START, X##_END); \
    } while (0)

int gameboy_create(gameboy_t *gameboy, const char *filename)
{
    M_REQUIRE_NON_NULL(gameboy);

    //Initialisation du bus
    memset(&gameboy->bus, 0, (BUS_SIZE * sizeof(data_t *)));

    RETURN_IF_ERROR(cartridge_init(&gameboy->cartridge, filename));
    RETURN_IF_ERROR(cartridge_plug(&gameboy->cartridge, gameboy->bus));

    gameboy->boot = 1u;
    RETURN_IF_ERROR(bootrom_init(&gameboy->bootrom));
    RETURN_IF_ERROR(bootrom_plug(&gameboy->bootrom, gameboy->bus));

    RETURN_IF_ERROR(timer_init(&gameboy->timer, &gameboy->cpu));

    int i = 0;
    //checker les retours des fonctions
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
    RETURN_IF_ERROR(component_shared(&echo_ram, &gameboy->components[0]));
    RETURN_IF_ERROR(bus_plug(gameboy->bus, &echo_ram, ECHO_RAM_START, ECHO_RAM_END));

    RETURN_IF_ERROR(cpu_init(&gameboy->cpu));
    RETURN_IF_ERROR(cpu_plug(&gameboy->cpu, &gameboy->bus));

    gameboy->cycles = 0;

    return ERR_NONE;
}

void gameboy_free(gameboy_t *gameboy)
{
    if (gameboy != NULL)
    {   
        // Unplug un clone de echo_ram
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
        RETURN_IF_ERROR(timer_cycle(&gameboy->timer));
        RETURN_IF_ERROR(cpu_cycle(&gameboy->cpu));
        RETURN_IF_ERROR(bootrom_bus_listener(gameboy, gameboy->cpu.write_listener));
        RETURN_IF_ERROR(timer_bus_listener(&gameboy->timer, gameboy->cpu.write_listener));
        if (((gameboy->cycles) % DRAW_IMAGE_CYCLES) == 0)
            cpu_request_interrupt(&gameboy->cpu, VBLANK);
#ifdef BLARGG
        M_EXIT_IF_ERR(blargg_bus_listener(gameboy, gameboy->cpu.write_listener));
#endif
        //le nombre de cycles déjà simulés est mis à jour non?
        gameboy->cycles++;
    }
    return ERR_NONE;
}
