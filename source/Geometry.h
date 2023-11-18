/*
 * WiiFur
 * By Rainbain
 * Nov 1, 2023
 *
 * Geometry
 * Used to generate all the geometry we will be using.
*/

#pragma once

#include "MathUtil.h"
#include "GeometryPool.h"

#define GEOMETRY_CHANNEL_POSES 1
#define GEOMETRY_CHANNEL_NORMS 2
#define GEOMETRY_CHANNEL_UVS 4
#define GEOMETRY_CHANNEL_COLOR 8

#define GEOMETRY_CHANNEL_ALL 15
#define GEOMETRY_CHANNEL_MAT (GEOMETRY_CHANNEL_POSES|GEOMETRY_CHANNEL_NORMS|GEOMETRY_CHANNEL_COLOR)

typedef struct {
    uint32_t index;
    uint32_t size;
    
    guVector *poses;
    guVector *norms;
    UV *uvs;
    void *pool;
    GXColor color;
} Geometry;

extern void Geometry_Initiate(Geometry* geo, uint32_t verts);
extern void Geometry_Push(Geometry *geo, guVector pos, guVector norm, UV uv);
extern void Geometry_Clear(Geometry *geo);
extern void Geometry_Free(Geometry* geo);

extern uint32_t Geometry_PlaneItch();
extern void Geometry_Plane(Geometry *geo);

// Insert geometry into this geometry.
extern void Geometry_Insert(Geometry *geo, Geometry* src);
extern void Geometry_InsertArrays(Geometry * geo, uint16_t size, const guVector *poses, const guVector *norms, const UV *uvs);

extern void Geometry_PushAlongNormals(Geometry *geo, float dist);
extern void Geometry_ApplyTransform(Geometry* geo, Transform* t);
extern void Geometry_TranslaveUVs(Geometry* geo, UV offset);
extern void Geometry_ScaleUVs(Geometry *geo, UV scale);

extern void Geometry_SetColor(Geometry *geo, GXColor color);

extern GeometryPoolEntry Geometry_PushToPool(Geometry *geo, GeometryPool* pool, uint8_t channels);

/*
typedef struct {
    void *pool;
    guVector *poses;
    guVector *norms;
    uint16_t *colorIndexes;
    UV *uvs;
    UV *circleUvs;
    size_t index;
    size_t size;

    uint16_t noiseWidth;
    uint16_t noiseHeight;
    void *noiseData;

    uint16_t circleWidth;
    uint16_t circleHeight;
    void *circleData;

    GXColor *pallet;
} Geometry;

extern void Geometry_Allocate(Geometry *geo, size_t verts);
extern void Geometry_Free(Geometry *geo);
extern void Geometry_Push(Geometry *geo, guVector pos, guVector normal, uint16_t colorIndex, UV uv, float uvScale);
extern void Geometry_PushTransformed(Geometry *geo, guVector pos, guVector normal, Transform *transform, uint16_t colorIndex, UV uv, float uvScale);
extern void Geometry_SetPallet(Geometry *geo, GXColor *pallet);
extern void Geometry_SetTextures(Geometry *geo, void * data, uint16_t w, uint16_t h, void * cdata, uint16_t cw, uint16_t ch);

extern size_t Geometry_PlaneItch();
extern void Geometry_Plane(Geometry *geo, Transform *transform, uint16_t colorIndex, float uvScale);

// Put it on the GPU, shared memory so its just passing a pointer
extern void Geometry_Load(Geometry *geo);
extern void Geometry_Draw(Geometry *geo);
*/