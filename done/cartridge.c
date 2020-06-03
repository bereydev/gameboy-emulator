/**
 * @file bootrom.c
 * @brief Game Boy Boortom simulation 
 *
 * @author C la vie
 * @date 2020
 */

#include "cartridge.h"
#include "error.h"
#include <stdio.h>

int cartridge_init_from_file(component_t *c, const char *filename)
{
	M_REQUIRE_NON_NULL(c);
	M_REQUIRE_NON_NULL(c->mem);

	FILE *fp = fopen(filename, "rb");
	if (fp == NULL)
		return ERR_IO;

	size_t size_read = fread(c->mem->memory, sizeof(data_t), BANK_ROM_SIZE, fp);
	if (size_read != BANK_ROM_SIZE)
	{
		fclose(fp);
		return ERR_IO;
	}
	else
	{
		for (size_t i = 0; i < BANK_ROM_SIZE; ++i)
		{
		}
		if (c->mem->memory[CARTRIDGE_TYPE_ADDR] != 0u)
		{
			fclose(fp);
			return ERR_NOT_IMPLEMENTED;
		}
	}

	fclose(fp);

	return ERR_NONE;
}

int cartridge_init(cartridge_t *ct, const char *filename)
{
	M_REQUIRE_NON_NULL(ct);
	M_REQUIRE_NON_NULL(filename);

	M_EXIT_IF_ERR(component_create(&ct->c, BANK_ROM_SIZE));
	if (cartridge_init_from_file(&ct->c, filename) != ERR_NONE) {
		component_free(&ct->c);
		return ERR_MEM;
	} else {
		return ERR_NONE;
	}
}

int cartridge_plug(cartridge_t *ct, bus_t bus)
{
	M_REQUIRE_NON_NULL(ct);
	M_REQUIRE_NON_NULL(ct->c.mem);

	return bus_forced_plug(bus, &ct->c, BANK_ROM0_START, BANK_ROM1_END, 0);
}

void cartridge_free(cartridge_t *ct)
{
	if (ct != NULL) {
		component_free(&ct->c);
	}
}
