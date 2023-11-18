/*
 * WiiFur
 * By Rainbain
 * Nov 3, 2023
 *
 * GeometryPool
 * Pools graphics primitives.
 * Pools each seperatly, no geometry struct, since not all geometrys will use the same primatives
*/

#pragma once

#include "MathUtil.h"

#define GEOMETRYPOOL_UV_CHANNELS 2

typedef struct {
    guVector *poses;
    guVector *norms;
    GXColor *colors;
    UV *uvs[GEOMETRYPOOL_UV_CHANNELS];

    uint16_t numPoses;
    uint16_t numNorms;
    uint16_t numColors;
    uint16_t numUvs[GEOMETRYPOOL_UV_CHANNELS];

    uint8_t formatChannel;
} GeometryPool;

typedef struct {
    GeometryPool *pool;
    uint16_t points;
    guVector *poses;
    guVector *norms;
    GXColor *colors;
    uint16_t startPosIdx;
    uint16_t startNormIdx;
    uint16_t startColIdx;
    uint16_t startUvIdx[GEOMETRYPOOL_UV_CHANNELS];

    UV *uvs[GEOMETRYPOOL_UV_CHANNELS];
} GeometryPoolEntry;

extern void GeometryPool_Initiate(GeometryPool *pool, uint8_t formatChannel);
extern void GeometryPool_Free(GeometryPool* pool);

extern guVector* GeometryPool_PushPositions(GeometryPool *pool, guVector *poses, uint16_t size);
extern guVector* GeometryPool_PushNormals(GeometryPool *pool, guVector *norms, uint16_t size);
extern GXColor* GeometryPool_PushColors(GeometryPool *pool, GXColor *colors, uint16_t size);
extern UV* GeometryPool_PushUVs(GeometryPool *pool, UV *uvs, uint16_t size, uint8_t channel);

extern void GeometryPool_Load(GeometryPool *pool);
extern void GeometryPool_Setup(GeometryPool *pool);