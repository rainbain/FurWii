/*
 * WiiFur
 * By Rainbain
 * Nov 3, 2023
 * 
 * Palette
 * Used to calculate and use color pallets.
*/

#include "Palette.h"

#include "MathUtil.h"

GXColor Palette_Interpolate(Palette *pt, float t){
    if(pt->entrys == 0) return (GXColor){0,0,0,0};

    for(uint16_t i = 0; i < pt->entrys; i++){
        if(pt->ankers[i] > t){
            if(i == 0) return pt->colors[0];

            GXColor a = pt->colors[i-1];
            GXColor b = pt->colors[i];

            float v = (t - pt->ankers[i-1]) / (pt->ankers[i] - pt->ankers[i-1]);

            return Color_Lerp(a, b, v);  
        }
    }

    return pt->colors[pt->entrys-1];
}

void Palette_LinerFill(Palette *pt, uint8_t a, uint8_t b, uint8_t chan){
    for(uint16_t i = 0; i < pt->entrys; i++) {
        ((uint8_t*)&pt->colors[i])[chan] = U8_Lerp(a, b, pt->ankers[i]);
    }
}