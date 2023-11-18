/*
 * WiiFur
 * By Rainbain
 * Nov 3, 2023
 *
 * GeometryPool
 * Pools graphics primitives.
 * Pools each seperatly, no geometry struct, since not all geometrys will use the same primatives
*/

#include "GeometryPool.h"

#include <stdlib.h>
#include <string.h>

void GeometryPool_Initiate(GeometryPool *pool, uint8_t formatChannel){
    // These will be nullptr, until realloc does something with them
    pool->poses = 0;
    pool->norms = 0;
    pool->colors = 0;
    for(uint8_t i = 0; i < GEOMETRYPOOL_UV_CHANNELS; i++)
        pool->uvs[i] = 0;
    
    pool->numPoses = 0;
    pool->numNorms = 0;
    pool->numColors = 0;
    for(uint8_t i = 0; i < GEOMETRYPOOL_UV_CHANNELS; i++)
        pool->numUvs[i] = 0;
    
    pool->formatChannel = formatChannel;
}

void GeometryPool_Free(GeometryPool* pool){
    if(pool->poses) {free(pool->poses); pool->poses = 0;}
    if(pool->norms) {free(pool->norms); pool->norms = 0;}
    if(pool->colors) {free(pool->colors); pool->colors = 0;}
    for(uint8_t i = 0; i < GEOMETRYPOOL_UV_CHANNELS; i++)
        if(pool->uvs[i]) {free(pool->uvs[i]); pool->uvs[i] = 0;}
    
    pool->numPoses = 0;
    pool->numNorms = 0;
    pool->numColors = 0;
    for(uint8_t i = 0; i < GEOMETRYPOOL_UV_CHANNELS; i++)
        pool->numUvs[i] = 0;
}


guVector* GeometryPool_PushPositions(GeometryPool *pool, guVector *poses, uint16_t size){
    uint16_t index = pool->numPoses;

    pool->numPoses += size;
    pool->poses = realloc(pool->poses, pool->numPoses * sizeof(guVector));
    memcpy(&pool->poses[index], poses, size*sizeof(guVector));

    return &pool->poses[index];
}

guVector* GeometryPool_PushNormals(GeometryPool *pool, guVector *norms, uint16_t size){
    uint16_t index = pool->numNorms;

    pool->numNorms += size;
    pool->norms = realloc(pool->norms, pool->numNorms * sizeof(guVector));
    memcpy(&pool->norms[index], norms, size*sizeof(guVector));

    return &pool->norms[index];
}

GXColor* GeometryPool_PushColors(GeometryPool *pool, GXColor *colors, uint16_t size){
    uint16_t index = pool->numColors;

    pool->numColors += size;
    pool->colors = realloc(pool->colors, pool->numColors * sizeof(GXColor));
    memcpy(&pool->colors[index], colors, size*sizeof(GXColor));

    return &pool->colors[index];
}

UV* GeometryPool_PushUVs(GeometryPool *pool, UV *uvs, uint16_t size, uint8_t channel){
    uint16_t index = pool->numUvs[channel];

    pool->numUvs[channel] += size;
    pool->uvs[channel] = realloc(pool->uvs[channel], pool->numUvs[channel] * sizeof(UV));
    memcpy(&pool->uvs[channel][index], uvs, size*sizeof(UV));

    return &pool->uvs[channel][index];
}


void GeometryPool_Load(GeometryPool *pool){
    if(pool->poses){
        GX_SetVtxAttrFmt(pool->formatChannel, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    }

    if(pool->norms) {
        GX_SetVtxAttrFmt(pool->formatChannel, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    }

    if(pool->colors) {
        GX_SetVtxAttrFmt(pool->formatChannel, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    }

    for(uint8_t i = 0; i < GEOMETRYPOOL_UV_CHANNELS; i++){
        if(pool->uvs[i] == 0) continue;
        GX_SetVtxAttrFmt(pool->formatChannel, GX_VA_TEX0+i, GX_TEX_ST, GX_F32, 0);
        GX_SetTexCoordGen(GX_TEXCOORD0+i, GX_TG_MTX2x4, GX_TG_TEX0+i, GX_IDENTITY);
    }
    GX_InvVtxCache();
}

void GeometryPool_Setup(GeometryPool *pool){
    GX_ClearVtxDesc();
    if(pool->poses){
        GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
        GX_SetArray(GX_VA_POS, pool->poses, 3*sizeof(float));
    }

    if(pool->norms) {
        GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GX_SetArray(GX_VA_NRM,	pool->norms,	3*sizeof(float));
    }

    if(pool->colors) {
        GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GX_SetArray(GX_VA_CLR0,	pool->colors, 4*sizeof(u8));
    }

    for(uint8_t i = 0; i < GEOMETRYPOOL_UV_CHANNELS; i++){
        if(pool->uvs[i] == 0) continue;
        GX_SetVtxDesc(GX_VA_TEX0+i, GX_INDEX16);
        GX_SetTexCoordGen(GX_TEXCOORD0+i, GX_TG_MTX2x4, GX_TG_TEX0+i, GX_IDENTITY);
    }
    for(uint8_t i = 0; i < GEOMETRYPOOL_UV_CHANNELS; i++){
        if(pool->uvs[i] == 0) continue;
        GX_SetArray(GX_VA_TEX0+i,	pool->uvs[i], 2*sizeof(float));
    }
}