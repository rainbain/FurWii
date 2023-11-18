/*
 * WiiFur
 * By Rainbain
 * Nov 1, 2023
 *
 * Images
 * Generates images used.
*/

#pragma once

#include <stdint.h>

void Images_NoiseR8(uint8_t *img, uint32_t squarePixels, uint8_t range, uint8_t offset);
void Images_DistanceMapR8(uint8_t *img, uint16_t w, uint16_t h, float thickness);

//uint32_t Image_Swizzle(uint16_t x, uint16_t y, uint16_t w);