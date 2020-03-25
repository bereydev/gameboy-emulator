#include "bus.h"
#include "error.h"
#include "component.h"
#include "bit.h"

/**
 * @brief Remap the memory of a component to the bus
 *
 * @param bus bus to remap to
 * @param c component to remap
 * @param offset new offset to use
 * @return error code
 */
int bus_remap(bus_t bus, component_t* c, addr_t offset){
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE( (c->end - c->start + offset) <= c->mem.size, ERR_ADDRESS, "input offset (%x) is incorrect",offset);
    // l'adresse start du bus pointera sur l'adresse offset de la mémoire du composant (il s'agit bien de l'adresse d'une data_t pas de memory_t)
    //bus[c->start] = &c->mem.memory[offset];
    
    
    //il faut faire le mapping entier (de taille size), pas seulement pour le premier
	size_t s = c->end - c->start + 1;
	for(size_t i = 0; i<s;++i) {
		bus[c->start + i] = &c->mem.memory[offset + i];
		}
   
    return ERR_NONE;
}

/**
 * @brief Plug forcibly a component into the bus (allows to use outside of range start and end).
 *        For example, if we want to map a component to somewhere else than the initialy described area.
 *
 * @param bus bus to plug into
 * @param c component to plug into bus
 * @param start address from where to plug to (included)
 * @param end address until where to plug to (included)
 * @param offset offset where to start in the component
 * @return error code
 */
int bus_forced_plug(bus_t bus, component_t* c, addr_t start, addr_t end, addr_t offset){
    M_REQUIRE_NON_NULL_CUSTOM_ERR(c, ERR_BAD_PARAMETER);
    M_REQUIRE(0x0000 <= start && start <= 0xFFFF, ERR_BAD_PARAMETER, "input start (%x) is not between %x and %x",start,0x0000,0xFFFF);
    M_REQUIRE(0x0000 <= end && end <= 0xFFFF, ERR_BAD_PARAMETER, "input end (%x) is not between %x and %x",end,0x0000,0xFFFF); 						//si start = end = 0xFFFF
	M_REQUIRE(start!=end, ERR_BAD_PARAMETER, "inputs start(%x) and end(%x) are equal", start, end);
    M_REQUIRE( end - start + offset <= c->mem.size, ERR_ADDRESS, "input offset (%x) is incorrect",offset);
    
    c->start = start;
    c->end = end;   
    if (bus_remap(bus, c, offset) != ERR_NONE){
        component_free(c);
        return ERR_MEM;
    } 
    return ERR_NONE;
}

/**
 * @brief Plug a component into the bus
 *
 * @param bus bus to plug into
 * @param c component to plug into bus
 * @param start address from where to plug (included)
 * @param end address until where to plug (included)
 * @return error code
 */
int bus_plug(bus_t bus, component_t* c, addr_t start, addr_t end){	
	
	for(size_t i = start; i <= end; ++i){
		if(bus[i]!=NULL) return ERR_ADDRESS;
		}
	return bus_forced_plug(bus, c, start, end, 0);
	
	}

/**
 * @brief Unplug a component from the bus
 *
 * @param bus bus to unplug from
 * @param c component to plug into bus
 * @return error code
 */
int bus_unplug(bus_t bus, component_t* c){
	M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    
    for(size_t i = c->start; i <= c->end; ++i){
		bus[i] = NULL;
		}
    component_free(c);
    return ERR_NONE;
	
	}
	
	/**
 * @brief Read the bus at a given address
 *
 * @param bus bus to read from
 * @param address address to read at
 * @param data pointer to write read data to
 * @return error code
 */
int bus_read(const bus_t bus, addr_t address, data_t* data){
	M_REQUIRE_NON_NULL_CUSTOM_ERR(data, ERR_BAD_PARAMETER);
	M_REQUIRE(0x0000 <= address && address <= 0xFFFF, ERR_BAD_PARAMETER, "input address (%x) is not between %x and %x",address,0x0000,0xFFFF);
	
	*data = (bus[address]==NULL) ? 0xFF : *bus[address];
	
	return ERR_NONE;
	}


/**
 * @brief Write to the bus at a given address
 *
 * @param bus bus to write to
 * @param address address to write at
 * @param data data to write
 * @return error code
 */
int bus_write(bus_t bus, addr_t address, data_t data){
	M_REQUIRE(0x0000 <= address && address <= 0xFFFF, ERR_BAD_PARAMETER, "input address (%x) is not between %x and %x",address,0x0000,0xFFFF);
	M_REQUIRE_NON_NULL(bus[address]);
	bus[address] = &data;
	
	return ERR_NONE;
	
	}

/**
 * @brief Read the bus at a given address (reads 16 bits)
 *
 * @param bus bus to read from
 * @param address address to read at
 * @param data16 pointer to write read data to
 * @return error code
 */
int bus_read16(const bus_t bus, addr_t address, addr_t* data16){
	M_REQUIRE_NON_NULL_CUSTOM_ERR(data16, ERR_BAD_PARAMETER);
	
	data_t ls_byte = 0;
	int first_error_code = bus_read(bus, address, &ls_byte);
	data_t ms_byte = 0;
	int second_error_code = bus_read(bus, address+1, &ms_byte);
	if(first_error_code!=ERR_NONE && second_error_code!=ERR_NONE) *data16 = merge8(ls_byte, ms_byte);
	else *data16 = 0xFF;
	return ERR_NONE;
	}

/**
 * @brief Write to the bus at a given address (writes 16 bits)
 *
 * @param bus bus to write to
 * @param address address to write at
 * @param data16 data to write
 * @return error code
 */
int bus_write16(bus_t bus, addr_t address, addr_t data16){
	data_t ls_byte = lsb8(data16);
	data_t ms_byte = msb8(data16);
	bus_write(bus, address, ls_byte);
	bus_write(bus, address + 1, ms_byte);
	
	return ERR_NONE;
	}
