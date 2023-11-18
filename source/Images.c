/*
 * WiiFur
 * By Rainbain
 * Nov 1, 2023
 *
 * Images
 * Generates images used.
*/

#include "Images.h"

#include <stdlib.h>
#include <math.h>

void Images_NoiseR8(uint8_t *img, uint32_t squarePixels, uint8_t range, uint8_t offset){
    for(uint32_t i = 0; i < squarePixels; i++) img[i] = rand() % range + offset;
}

void Images_DistanceMapR8(uint8_t *img, uint16_t w, uint16_t h, float thickness){
    float halfW = (float)(w-1) / 2.0f;
    float halfH = (float)(h-1) / 2.0f;
    for(int x = 0; x < w; x++){
        for(int y = 0; y < h; y++){
            float fx = (x-halfW) / halfW;
            float fy = (y-halfH) / halfH;
            float dist = sqrtf(fx*fx+fy*fy) / thickness;


            if(dist > 1.0f) dist = 1.0f;
            if(dist < 0.0f) dist = 0.0f;

            img[x+y*w] = (uint8_t)(dist*255);
        }
    }
}

// Moved to texture atlas code
/*
uint32_t Image_Swizzle(uint16_t x, uint16_t y, uint16_t w){
    uint16_t widthInTiles = (w + IMAGE_TILE_WIDTH-1) / IMAGE_TILE_WIDTH;

    uint16_t tileX = x / IMAGE_TILE_WIDTH;
    uint16_t tileY = y / IMAGE_TILE_HEIGH;
    uint16_t inTileX = x % IMAGE_TILE_WIDTH;
    uint16_t inTileY = y % IMAGE_TILE_HEIGH;

    return (tileY * widthInTiles + tileX) * (IMAGE_TILE_WIDTH * IMAGE_TILE_HEIGH)
        + inTileY * IMAGE_TILE_WIDTH
        + inTileX;
}
*/