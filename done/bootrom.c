/**
 * @file bootrom.c
 * @brief Game Boy Boortom simulation 
 *
 * @author C la vie
 * @date 2020
 */

#include "error.h"

int bootrom_init(component_t* c) {
	M_REQUIRE_NON_NULL(c);
	
	component_create(c, MEM_SIZE(BOOT_ROM));
	data_t content[MEM_SIZE(BOOT_ROM)] = GAMEBOY_BOOT_ROM_CONTENT;
	for(size_t i = 0; i < MEM_SIZE(BOOT_ROM); ++i) {
		*c->mem->memory[i] = content[i];
		}
	return ERR_NONE;
	}


int bootrom_bus_listener(gameboy_t* gameboy, addr_t addr){
	M_REQUIRE_NON_NULL(gameboy);

	if(addr == REG_BOOT_ROM_DISABLE && gameboy->boot == 1) {
		bus_unplug(gameboy->bus, &gameboy->bootrom); 
		cartridge_plug(&gameboy->cartridge, gameboy->bus); 
		boot = 0;
		}
	
	return ERR_NONE;
	}
