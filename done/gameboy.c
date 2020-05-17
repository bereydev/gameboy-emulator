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

#define INIT_COMPONENT(X, i)                            \
    {                                                   \
        component_t c;                                  \
        component_create(&c, MEM_SIZE(X));              \
        gameboy->components[i] = c;                     \
        bus_plug(gameboy->bus, &c, X##_START, X##_END); \
    }                                                   \
    while (0)

int gameboy_create(gameboy_t *gameboy, const char *filename)
{
    M_REQUIRE_NON_NULL(gameboy);
    
    //Initialisation du bus
    memset(&gameboy->bus, 0, (BUS_SIZE * sizeof(data_t*)));

    cartridge_init(&gameboy->cartridge,filename);
    cartridge_plug(&gameboy->cartridge, gameboy->bus);

    gameboy->boot = 1u;
    bootrom_init(&gameboy->bootrom);
    bootrom_plug(&gameboy->bootrom, gameboy->bus);

    timer_init(&gameboy->timer, &gameboy->cpu);

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
    component_shared(&echo_ram, &gameboy->components[0]);
    bus_plug(gameboy->bus, &echo_ram, ECHO_RAM_START, ECHO_RAM_END);

    cpu_init(&gameboy->cpu);
    cpu_plug(&gameboy->cpu, &gameboy->bus);

    gameboy->cycles = 0;

    return ERR_NONE;
}

void gameboy_free(gameboy_t *gameboy)
{
    if (gameboy != NULL)
    {
        // Unplug une copie de echo_ram
        component_t echo_clone = {NULL, ECHO_RAM_START, ECHO_RAM_END};
        bus_unplug(gameboy->bus, &echo_clone);

        for (size_t i = 0; i < GB_NB_COMPONENTS; ++i)
        {
            bus_unplug(gameboy->bus, &gameboy->components[i]);
            component_free(&gameboy->components[i]);
        }

        bus_unplug(gameboy->bus, &gameboy->bootrom);
        component_free(&gameboy->bootrom);

        bus_unplug(gameboy->bus, &gameboy->cartridge.c);
        cartridge_free(&gameboy->cartridge);
    }
}

int gameboy_run_until(gameboy_t *gameboy, uint64_t cycle)
{
    while (gameboy->cycles < cycle)
    {
        // TODO une fois que les autres composant auront XX_cycle implémenté
        // il faudra les ajouter ici

        //TODO il faut check le retour des fonctions
        timer_cycle(&gameboy->timer);
        cpu_cycle(&gameboy->cpu);
        bootrom_bus_listener(gameboy, gameboy->cpu.write_listener);
        timer_bus_listener(&gameboy->timer, gameboy->cpu.write_listener);
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
