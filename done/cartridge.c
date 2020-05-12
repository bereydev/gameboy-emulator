/**
 * @file bootrom.c
 * @brief Game Boy Boortom simulation 
 *
 * @author C la vie
 * @date 2020
 */
 #include "error.h"
 
 
 //Reads a file into the memory of a component
 int cartridge_init_from_file(component_t* c, const char* filename) {
	 M_REQUIRE_NON_NULL(c);
	 M_REQUIRE_NON_NULL(filename);
	 
	 //TODO fermer le fichier dans tous les cas d'erreur ou il y a un return
	 
	FILE fp* = fopen(filename, "rb");
	if (fp == NULL) {
		fclose(fp);
		return ERR_IO;
		} 
	
	//TODO modulariser
	
	size_t size_read = fread(c->mem->memory, sizeof(data_t), BANK_ROM_SIZE, fp);
	if(size_read != nb_el) {
		fclose(fp);
		return ERR_IO;
	}else {
		for(size_t i = 0; i<BANK_ROM_SIZE; ++i) {
			printf("c->mem->memory[%2zu] = %x \n ", i, c->mem->memory[i]);
			}
		if(c->mem->memory[CARTRIDGE_TYPE_ADDR]!=0u) {
			fclose(fp);
			return ERR_NOT_IMPLEMENTED;
			}
		}
	
	fclose(fp);
	
	return ERR_NONE;
	}
	
int cartridge_init(cartridge_t* ct, const char* filename){
	component_create(ct, BANK_ROM_SIZE); 
	cartridge_init_from_file(ct, filename);
	
	return ERR_NONE;
	}
	
int cartridge_plug(cartridge_t* ct, bus_t bus){
	bus_forced_plug(bus,ct, BANK_ROM0_START, BANK_ROM1_END, 0); 
	
	return ERR_NONE;
	}
	
void cartridge_free(cartridge_t* ct){
	component_free(ct);
	}
	 

