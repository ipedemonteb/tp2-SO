#include <stdint.h>
#include <lib.h>
#include "./include/module_loader.h"

static void load_module(uint8_t ** module, void * target_module_address);
static uint32_t read_uint32(uint8_t ** address);

void load_modules(void * payload_start, void ** target_module_address) {
	int i;
	uint8_t * current_module = (uint8_t *)payload_start;
	uint32_t module_count = read_uint32(&current_module);

	for (i = 0; i < module_count; i++) {
		load_module(&current_module, target_module_address[i]);
	}
}

static void load_module(uint8_t ** module, void * target_module_address) {
	uint32_t module_size = read_uint32(module);
	memcpy(target_module_address, *module, module_size);
	*module += module_size;
}

static uint32_t read_uint32(uint8_t ** address) {
	uint32_t result = *(uint32_t *)(*address);
	* address += sizeof(uint32_t);
	return result;
}
