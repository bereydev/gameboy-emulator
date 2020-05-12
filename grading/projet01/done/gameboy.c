/**
 * @file gameboy.c
 * @brief Gameboy for GameBoy Emulator
 *
 * @date 2020
 */
 
#include "gameboy.h"
#include "error.h"

int gameboy_create(gameboy_t* gameboy, const char* filename){
    M_REQUIRE_NON_NULL(gameboy);
    
    gameboy->nb_allocated_components = 0u;
    component_t work_ram;
    component_create(&work_ram, MEM_SIZE(WORK_RAM));
    gameboy->components[gameboy->nb_allocated_components++] = work_ram;
    bus_plug(gameboy->bus, &work_ram, WORK_RAM_START, WORK_RAM_END);
    component_t echo_ram;
    /* On n'ajoute pas echo_ram à la liste components car il partage
     * la même mémoire que work_ram cela permet d'eviter de free deux fois
     * la même zone mémoire dans gameboy_free */
    component_shared(&echo_ram, &work_ram);
    bus_plug(gameboy->bus, &echo_ram, ECHO_RAM_START, ECHO_RAM_END);
    
    return ERR_NONE;
}

void gameboy_free(gameboy_t* gameboy){
    if (gameboy != NULL){
        for (size_t i = 0; i < gameboy->nb_allocated_components; ++i) {
            bus_unplug(gameboy->bus, &gameboy->components[i]);
            component_free(&gameboy->components[i]);
        }
        gameboy->nb_allocated_components = 0u;
    }
}
