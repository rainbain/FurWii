/*
 * WiiFur
 * By Rainbain
 * Nov 2, 2023
 *
 * TextureAtlas
 * Fits textures, of a similar format, onto one image and applys swizzling
*/

#include "TextureAtlas.h"

#include <stdlib.h>

void TextureAtlas_Initiate(TextureAtlas *atlas, uint8_t fmt, uint8_t texgen, uint16_t w, uint16_t h, uint8_t pixelSize){
    atlas->data = malloc((uint32_t)w * (uint32_t)h * (uint32_t)pixelSize);
    atlas->texgen = texgen;
    atlas->pixelSize = pixelSize;
    atlas->width = w;
    atlas->height = h;

    GX_InitTexObj(&atlas->texinfo, atlas->data, w, h, fmt, GX_REPEAT, GX_REPEAT, GX_FALSE);
}

void TextureAtlas_SetFilterMode(TextureAtlas *atlas, uint8_t filterMode){
    GX_InitTexObjFilterMode(&atlas->texinfo, filterMode, filterMode);
}

void TextureAtlas_Free(TextureAtlas *atlas){
    free(atlas->data);
}


void TextureAtlas_Load(TextureAtlas *atlas){
    GX_InvalidateTexAll();
    GX_LoadTexObj(&atlas->texinfo, atlas->texgen);
}

void TextureAtlasEntry_Put(TextureAtlasEntry * entry, void* image){
    uint16_t widthInTiles = (entry->atlas->width + SWIZZLE_TILE_WIDTH-1) / SWIZZLE_TILE_WIDTH;
    for(uint32_t px = 0; px < entry->width; px++){
        for(uint32_t py = 0; py < entry->height; py++){
            uint16_t setX = px + entry->x;
            uint16_t setY = py + entry->y;

            uint16_t tileX = setX / SWIZZLE_TILE_WIDTH;
            uint16_t tileY = setY / SWIZZLE_TILE_HEIGH;
            uint16_t inTileX = setX % SWIZZLE_TILE_WIDTH;
            uint16_t inTileY = setY % SWIZZLE_TILE_HEIGH;

            uint32_t pixelPosition = (tileY * widthInTiles + tileX) * (SWIZZLE_TILE_WIDTH * SWIZZLE_TILE_HEIGH)
                + inTileY * SWIZZLE_TILE_WIDTH
                + inTileX;

            for(uint8_t i = 0; i < entry->atlas->pixelSize; i++){
                ((uint8_t*)entry->atlas->data)[pixelPosition * entry->atlas->pixelSize + i] = ((uint8_t*)image)[(px+py*entry->width) * entry->atlas->pixelSize + i];
            }
        }
    }
}


extern void TextureAtlasEntry_Initiate(TextureAtlasEntry * entry, TextureAtlas *atlas, uint16_t x, uint16_t y, uint16_t width, uint16_t height){
    entry->atlas = atlas;
    entry->x = x;
    entry->y = y;
    entry->width = width;
    entry->height = height;
}

void TextureAtlasEntry_GetUVTransform(TextureAtlasEntry * entry, UV* scale, UV* offset){
    *scale = (UV){(float)entry->width / (float)entry->atlas->width,
        (float)entry->height / (float)entry->atlas->height};
    
    *offset = (UV){(float)entry->x / (float)entry->atlas->width,
        (float)entry->y / (float)entry->atlas->height};
}