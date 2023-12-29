#include <lcom/lcf.h>

#include <stdint.h>

#ifdef LAB3
uint32_t counter = 0;
#endif

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
	*lsb = (uint8_t) val;
	
	return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
	*msb = (uint8_t) (val >> 8);
	
	return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
	uint32_t val = (uint32_t) *value;
	
	if (sys_inb(port, &val) != 0) return 1;
	
    #ifdef LAB3
    counter++;
    #endif

	*value = val;
	
	return 0;
}
