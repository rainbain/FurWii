/*
 * WiiFur
 * By Rainbain
 * Nov 3, 2023
 * 
 * Palette
 * Used to calculate and use color pallets.
*/

#include <gccore.h>

#pragma once

typedef struct {
    float* ankers;
    GXColor* colors;

    uint16_t entrys;
} Palette;

GXColor Palette_Interpolate(Palette *pt, float t);

// Sets each channel of each color with the liner interpolation of A and B based on the anker value
// Easy way to create a liner gradiant across one of the channels
void Palette_LinerFill(Palette *pt, uint8_t a, uint8_t b, uint8_t chan);