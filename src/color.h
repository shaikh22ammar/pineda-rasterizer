#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef union {
	struct {
		uint8_t r : 8;
		uint8_t g : 8;
		uint8_t b : 8;
		uint8_t a : 8;
	}; 
	uint32_t rgba;} 
color32_t;

#endif
