/*
 * WiiFur
 * By Rainbain
 * Nov 2, 2023
 *
 * TextureAtlas
 * Fits textures, of a similar format, onto one image and applys swizzling
*/

#pragma once

#include <stdint.h>
#include <gccore.h>

#include "MathUtil.h"

#define SWIZZLE_TILE_WIDTH 8
#define SWIZZLE_TILE_HEIGH 4

typedef struct {
    void *data;
    uint8_t texgen;
    uint8_t pixelSize;
    GXTexObj texinfo;

    uint16_t width, height;
} TextureAtlas;

typedef struct {
    TextureAtlas *atlas;

    uint16_t x, y;
    uint16_t width, height;
} TextureAtlasEntry;

extern void TextureAtlas_Initiate(TextureAtlas *atlas, uint8_t fmt, uint8_t texgen, uint16_t w, uint16_t h, uint8_t pixelSize);
extern void TextureAtlas_SetFilterMode(TextureAtlas *atlas, uint8_t filterMode);
extern void TextureAtlas_Free(TextureAtlas *atlas);
extern void TextureAtlas_Load(TextureAtlas *atlas);

extern void TextureAtlasEntry_Initiate(TextureAtlasEntry * entry, TextureAtlas *atlas, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
extern void TextureAtlasEntry_GetUVTransform(TextureAtlasEntry * entry, UV* scale, UV* offset);
extern void TextureAtlasEntry_Put(TextureAtlasEntry * entry, void* image);