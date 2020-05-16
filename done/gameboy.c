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

#define DRAW_IMAGE_CYCLES ((uint64_t) 17556)

#ifdef BLARGG
static int blargg_bus_listener(gameboy_t* gameboy, addr_t addr){
    M_REQUIRE_NON_NULL(gameboy);

	if(addr == BLARGG_REG ) printf("%c", cpu_read_at_idx(gameboy->cpu, addr));
	
	return ERR_NONE;
}
#endif

int gameboy_create(gameboy_t* gameboy, const char* filename){
    M_REQUIRE_NON_NULL(gameboy);
    
    //Initialisation du bus
    memset(&gameboy->bus, 0, BUS_SIZE);

    gameboy->boot = 1u;

    cartridge_init(&gameboy->cartridge,filename);
    cartridge_plug(&gameboy->cartridge, gameboy->bus);

    bootrom_init(&gameboy->bootrom);
    bootrom_plug(&gameboy->bootrom, gameboy->bus);

    timer_init(&gameboy->timer, &gameboy->cpu);

    //TODO il faut vérifier ce que les fonctions retournent
    component_t work_ram;
    component_create(&work_ram, MEM_SIZE(WORK_RAM));
    gameboy->components[0] = work_ram;
    bus_plug(gameboy->bus, &work_ram, WORK_RAM_START, WORK_RAM_END);
    //TODO voir comment améliorer ces déclaration en boucle avec les macros ?
    //MEMSIZE macro concatener avec les # pour faire une boucle avec la macro

   /* component_t echo_ram;
     On n'ajoute pas echo_ram à la liste components car il partage
     * la même mémoire que work_ram cela permet d'eviter de free deux fois
     * la même zone mémoire dans gameboy_free 
    component_shared(&echo_ram, &work_ram);
    bus_plug(gameboy->bus, &echo_ram, ECHO_RAM_START, ECHO_RAM_END);

    component_t registers;
    component_create(&registers, MEM_SIZE(REGISTERS));
    gameboy->components[1] = registers;
    bus_plug(gameboy->bus, &registers, REGISTERS_START, REGISTERS_END);

    component_t extern_ram;
    component_create(&extern_ram, MEM_SIZE(EXTERN_RAM));
    gameboy->components[2] = extern_ram;
    bus_plug(gameboy->bus, &extern_ram, EXTERN_RAM_START, EXTERN_RAM_END);

    component_t video_ram;
    component_create(&video_ram, MEM_SIZE(VIDEO_RAM));
    gameboy->components[3] = video_ram;
    bus_plug(gameboy->bus, &video_ram, VIDEO_RAM_START, VIDEO_RAM_END);

    component_t graph_ram;
    component_create(&graph_ram, MEM_SIZE(GRAPH_RAM));
    gameboy->components[4] = graph_ram;
    bus_plug(gameboy->bus, &graph_ram, GRAPH_RAM_START, GRAPH_RAM_END);

    component_t useless;
    component_create(&useless, MEM_SIZE(USELESS));
    gameboy->components[5] = useless;
    bus_plug(gameboy->bus, &useless, USELESS_START, USELESS_END);*/

    //c-mem is null 
    cpu_plug(&gameboy->cpu, &gameboy->bus);

    gameboy->cycles = 0;

    return ERR_NONE;
}

void gameboy_free(gameboy_t* gameboy){
    if (gameboy != NULL){
        // TODO comment unplug quelque chose qui n'est plus accessible ? (créer echo et unplug)
        for (size_t i = 0; i < GB_NB_COMPONENTS; ++i) {
            bus_unplug(gameboy->bus, &gameboy->components[i]);
            component_free(&gameboy->components[i]);
        }
        
        bus_unplug(gameboy->bus, &gameboy->bootrom);
        component_free(&gameboy->bootrom);
        
        bus_unplug(gameboy->bus, &gameboy->cartridge);
        cartridge_free(&gameboy->cartridge);

    }
}

int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle){
    while (gameboy->cycles < cycle)
    {
        // TODO une fois que les autres composant auront XX_cycle implémenté
        // il faudra les ajouter ici

        //TODO il faut check le retour des fonctions
        timer_cycle(&gameboy->timer);
        cpu_cycle(&gameboy->cpu);
        bootrom_bus_listener(gameboy, gameboy->cpu.write_listener );
        timer_bus_listener(&gameboy->timer, gameboy->cpu.write_listener);
        if(((gameboy->cycles) % DRAW_IMAGE_CYCLES) == 0) cpu_request_interrupt(&gameboy->cpu, VBLANK);
        #ifdef BLARGG
        M_EXIT_IF_ERR(blargg_bus_listener(gameboy, gameboy->cpu.write_listener));
        #endif
        //le nombre de cycles déjà simulés est mis à jour non?
        gameboy->cycles++;
    }
}