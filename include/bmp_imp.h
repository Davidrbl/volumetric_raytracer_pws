#pragma once

#include <stdint.h>

void bmp_load(const char** addresses, uint32_t num_addresses, uint8_t** bitmap, uint32_t* bmp_res);

uint8_t value_in_bmp(uint8_t* bmp, uint32_t bmp_res, float x, float y, float z);

