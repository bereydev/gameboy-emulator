#include "bus.h"
#include "error.h"
#include "component.h"
#include "bit.h"

int bus_remap(bus_t bus, component_t* c, addr_t offset){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE( (c->end - c->start + offset) <= c->mem->size, ERR_ADDRESS, "input offset (%x) is incorrect",offset);

	size_t s = c->end - c->start + 1;
	for(size_t i = 0; i<s;++i) {
		bus[c->start + i] = &c->mem->memory[offset + i];
		}
   
    return ERR_NONE;
}

// ======================================================================

int bus_forced_plug(bus_t bus, component_t* c, addr_t start, addr_t end, addr_t offset){
    M_REQUIRE_NON_NULL_CUSTOM_ERR(c, ERR_BAD_PARAMETER);
    M_REQUIRE(0x0000 <= start && start <= 0xFFFF, ERR_BAD_PARAMETER, "input start (%x) is not between %x and %x",start,0x0000,0xFFFF);
    M_REQUIRE(0x0000 <= end && end <= 0xFFFF, ERR_BAD_PARAMETER, "input end (%x) is not between %x and %x",end,0x0000,0xFFFF); 
	M_REQUIRE(start!=end, ERR_BAD_PARAMETER, "inputs start(%x) and end(%x) are equal", start, end);
    M_REQUIRE( end - start + offset <= c->mem->size, ERR_ADDRESS, "input offset (%x) is incorrect",offset);
    
    c->start = start;
    c->end = end;   
    if (bus_remap(bus, c, offset) != ERR_NONE){
        component_free(c);
        return ERR_MEM;
    } 
    return ERR_NONE;
}

// ======================================================================

int bus_plug(bus_t bus, component_t* c, addr_t start, addr_t end){	
	
	for(size_t i = start; i <= end; ++i){
		if(bus[i]!=NULL) return ERR_ADDRESS;
		}
	return bus_forced_plug(bus, c, start, end, 0);
	
	}

// ======================================================================

int bus_unplug(bus_t bus, component_t* c){
    M_REQUIRE_NON_NULL(c);
    
    for(size_t i = c->start; i <= c->end; ++i){
		bus[i] = NULL;
		}
    component_free(c);
    return ERR_NONE;
	
	}
	
// ======================================================================

int bus_read(const bus_t bus, addr_t address, data_t* data){
	M_REQUIRE_NON_NULL_CUSTOM_ERR(data, ERR_BAD_PARAMETER);
	M_REQUIRE(0x0000 <= address && address <= 0xFFFF, ERR_BAD_PARAMETER, "input address (%x) is not between %x and %x",address,0x0000,0xFFFF);
	
	*data = (bus[address]==NULL) ? 0xFF : *bus[address];
	
	return ERR_NONE;
	}
	
// ======================================================================

int bus_write(bus_t bus, addr_t address, data_t data){
	M_REQUIRE(0x0000 <= address && address <= 0xFFFF, ERR_BAD_PARAMETER, "input address (%x) is not between %x and %x",address,0x0000,0xFFFF);
	M_REQUIRE_NON_NULL(bus[address]);
	bus[address] = &data;
	
	return ERR_NONE;
	
	}

// ======================================================================

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
	
// ======================================================================

int bus_write16(bus_t bus, addr_t address, addr_t data16){
	data_t ls_byte = lsb8(data16);
	data_t ms_byte = msb8(data16);
	bus_write(bus, address, ls_byte);
	bus_write(bus, address + 1, ms_byte);
	
	return ERR_NONE;
	}
