/*
 * WiiFur
 * By Rainbain
 * Nov 1, 2023
 *
 * Geometry
 * Used to generate all the geometry we will be using.
*/

#include "Geometry.h"

#include <stdlib.h>
#include <string.h>

void Geometry_Initiate(Geometry* geo, uint32_t verts){
    size_t poolSize = (sizeof(guVector) * 2 + sizeof(UV)) * verts;

    geo->pool = malloc(poolSize);
    geo->poses = geo->pool;
    geo->norms = geo->pool + sizeof(guVector) * verts;
    geo->uvs = geo->pool + sizeof(guVector) * verts * 2;
    geo->color = (GXColor){0,0,0,0};

    geo->index = 0;
    geo->size = verts;
}

void Geometry_Push(Geometry *geo, guVector pos, guVector norm, UV uv){
    if(geo->index >= geo->size) return;

    geo->poses[geo->index] = pos;
    geo->norms[geo->index] = norm;
    geo->uvs[geo->index] = uv;

    geo->index++;
}

void Geometry_Clear(Geometry *geo){
    geo->index = 0;
}

void Geometry_Free(Geometry* geo){
    free(geo->pool);
}


uint32_t Geometry_PlaneItch(){
    return 6;
}

void Geometry_Plane(Geometry *geo){
    Geometry_Push(geo, (guVector){0.5f, 0.0f, -0.5f}, (guVector){0.0f, 1.0f, 0.0f}, (UV){1.0f, 0.0f});
    Geometry_Push(geo, (guVector){-0.5f, 0.0f, -0.5f}, (guVector){0.0f, 1.0f, 0.0f}, (UV){0.0f, 0.0f});
    Geometry_Push(geo, (guVector){0.5f, 0.0f, 0.5f}, (guVector){0.0f, 1.0f, 0.0f}, (UV){1.0f, 1.0f});
    Geometry_Push(geo, (guVector){-0.5f, 0.0f, -0.5f}, (guVector){0.0f, 1.0f, 0.0f}, (UV){0.0f, 0.0f});
    Geometry_Push(geo, (guVector){-0.5f, 0.0f, 0.5f}, (guVector){0.0f, 1.0f, 0.0f}, (UV){0.0f, 1.0f});
    Geometry_Push(geo, (guVector){0.5f, 0.0f, 0.5f}, (guVector){0.0f, 1.0f, 0.0f}, (UV){1.0f, 1.0f});
}


void Geometry_Insert(Geometry *geo, Geometry* src){
    for(uint32_t i = 0; i < src->index; i++){
        Geometry_Push(geo, src->poses[i], src->norms[i], src->uvs[i]);
    }
}

void Geometry_InsertArrays(Geometry * geo, uint16_t size, const guVector *poses, const guVector *norms, const UV *uvs){
    memcpy(&geo->poses[geo->index], poses, size*sizeof(guVector));
    memcpy(&geo->norms[geo->index], norms, size*sizeof(guVector));
    memcpy(&geo->uvs[geo->index], uvs, size*sizeof(UV));
    geo->index += size;
}

void Geometry_PushAlongNormals(Geometry *geo, float dist){
    guVector vec;
    for(uint32_t i = 0; i < geo->index; i++){
        vec = (guVector){geo->norms[i].x * dist, geo->norms[i].y * dist, geo->norms[i].z * dist};
        guVecAdd(&geo->poses[i], &vec, &geo->poses[i]);
    }
}

void Geometry_ApplyTransform(Geometry* geo, Transform* t){
    for(uint32_t i = 0; i < geo->index; i++){
        c_guVecMultiply(t->modelMatrix, &geo->poses[i], &geo->poses[i]);
        c_guVecMultiply(t->rotationMatrix, &geo->norms[i], &geo->norms[i]);
    }
}

void Geometry_TranslaveUVs(Geometry* geo, UV offset){
    for(uint32_t i = 0; i < geo->index; i++){
        geo->uvs[i].s += offset.s;
        geo->uvs[i].t += offset.t;
    }
}

void Geometry_ScaleUVs(Geometry *geo, UV scale){
    for(uint32_t i = 0; i < geo->index; i++){
        geo->uvs[i].s *= scale.s;
        geo->uvs[i].t *= scale.t;
    }
}

void Geometry_SetColor(Geometry *geo, GXColor color){
    geo->color = color;
}

GeometryPoolEntry Geometry_PushToPool(Geometry *geo, GeometryPool* pool, uint8_t channels){
    GeometryPoolEntry entry;
    entry.pool = pool;
    entry.points = geo->index;

    entry.startPosIdx = pool->numPoses;
    entry.startNormIdx = pool->numNorms;
    entry.startColIdx = pool->numColors;
    for(uint8_t i = 0; i < GEOMETRYPOOL_UV_CHANNELS; i++)
        entry.startUvIdx[i] = pool->numUvs[i];

    guVector* poses = (channels & GEOMETRY_CHANNEL_POSES) ? GeometryPool_PushPositions(pool, geo->poses, geo->index) : 0;
    guVector* norms = (channels & GEOMETRY_CHANNEL_NORMS) ? GeometryPool_PushNormals(pool, geo->norms, geo->index) : 0;
    UV* uvs0 = (channels & GEOMETRY_CHANNEL_UVS) ? GeometryPool_PushUVs(pool, geo->uvs, geo->index, 0) : 0;
    GXColor *color = (channels & GEOMETRY_CHANNEL_COLOR) ? GeometryPool_PushColors(pool, &geo->color, 1) : 0;

    entry.poses = poses;
    entry.norms = norms;
    entry.colors = color;
    entry.uvs[0] = uvs0;
    
    for(uint8_t i = 1; i < GEOMETRYPOOL_UV_CHANNELS; i++)
        entry.uvs[i] = 0;
    
    return entry;
}
/*
void Geometry_Allocate(Geometry *geo, size_t verts){
    size_t poolSize = sizeof(guVector) * verts * 2 + sizeof(uint16_t) * verts + sizeof(UV) * verts * 2;

    geo->pool = malloc(poolSize);
    geo->poses = geo->pool;
    geo->norms = geo->pool + verts * sizeof(guVector);
    geo->colorIndexes = geo->pool + verts * sizeof(guVector) + verts * sizeof(guVector);
    geo->uvs = geo->pool + verts * sizeof(guVector) + verts * sizeof(guVector) + sizeof(uint16_t) * verts;
    geo->circleUvs = geo->pool + verts * sizeof(guVector) + verts * sizeof(guVector) + sizeof(uint16_t) * verts + sizeof(UV) * verts;

    geo->index = 0;
    geo->size = verts;
}

void Geometry_Free(Geometry *geo){
    free(geo->pool);
}

void Geometry_Push(Geometry *geo, guVector pos, guVector normal, uint16_t colorIndex, UV uv, float uvScale){
    if(geo->index >= geo->size) return;

    geo->poses[geo->index] = pos;
    geo->norms[geo->index] = normal;
    geo->colorIndexes[geo->index] = colorIndex;
    geo->uvs[geo->index] = (UV){uv.s * uvScale, uv.t * uvScale};
    geo->circleUvs[geo->index] = (UV){(uv.s*(float)geo->noiseWidth-(1.0f/(float)geo->noiseWidth/2.0f))*uvScale, (uv.t*(float)geo->noiseHeight-(1.0f/(float)geo->noiseHeight/2.0f))*uvScale};

    geo->index++;
}

void Geometry_PushTransformed(Geometry *geo, guVector pos, guVector normal, Transform* transform, uint16_t colorIndex, UV uv, float uvScale){
    c_guVecMultiply(transform->modelMatrix, &pos, &pos);
    c_guVecMultiply(transform->rotationMatrix, &normal, &normal);
    
    Geometry_Push(geo, pos, normal, colorIndex, uv, uvScale);
}

void Geometry_SetPallet(Geometry *geo, GXColor *pallet){
    geo->pallet = pallet;
}

void Geometry_SetTextures(Geometry *geo, void * data, uint16_t w, uint16_t h, void * cdata, uint16_t cw, uint16_t ch){
    geo->noiseWidth = w;
    geo->noiseHeight = h;
    geo->noiseData = data;
    geo->circleData = cdata;
    geo->circleWidth = cw;
    geo->circleHeight = ch;
}


size_t Geometry_PlaneItch(){
    return 6;
}

void Geometry_Plane(Geometry *geo, Transform* transform, uint16_t colorIndex, float uvScale){
    Geometry_PushTransformed(geo, (guVector){0.5f, 0.0f, -0.5f}, (guVector){0.0f, 1.0f, 0.0f}, transform, colorIndex, (UV){1.0f, 0.0f},uvScale);
    Geometry_PushTransformed(geo, (guVector){-0.5f, 0.0f, -0.5f}, (guVector){0.0f, 1.0f, 0.0f}, transform, colorIndex, (UV){0.0f, 0.0f},uvScale);
    Geometry_PushTransformed(geo, (guVector){0.5f, 0.0f, 0.5f}, (guVector){0.0f, 1.0f, 0.0f}, transform, colorIndex, (UV){1.0f, 1.0f},uvScale);
    Geometry_PushTransformed(geo, (guVector){-0.5f, 0.0f, -0.5f}, (guVector){0.0f, 1.0f, 0.0f}, transform, colorIndex, (UV){0.0f, 0.0f},uvScale);
    Geometry_PushTransformed(geo, (guVector){-0.5f, 0.0f, 0.5f}, (guVector){0.0f, 1.0f, 0.0f}, transform, colorIndex, (UV){0.0f, 1.0f},uvScale);
    Geometry_PushTransformed(geo, (guVector){0.5f, 0.0f, 0.5f}, (guVector){0.0f, 1.0f, 0.0f}, transform, colorIndex, (UV){1.0f, 1.0f},uvScale);
}

void Geometry_Load(Geometry *geo){

    GX_ClearVtxDesc();
    GX_SetNumChans(1); // One vertex format

    // Positions, normals, colors, uvs, all indexed
    GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
    GX_SetVtxDesc(GX_VA_NRM, GX_INDEX8);
    GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
    GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);
    GX_SetVtxDesc(GX_VA_TEX1, GX_INDEX8);

    // Each of the formats, how they should be decoded
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,	GX_POS_XYZ,	GX_F32,	0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);

    // Arrays to the data to be indexed
    GX_SetArray(GX_VA_POS, geo->poses, 3*sizeof(float));
    GX_SetArray(GX_VA_NRM,	geo->norms,	3*sizeof(float));
    GX_SetArray(GX_VA_CLR0,	geo->pallet, 4*sizeof(u8));
    GX_SetArray(GX_VA_TEX0,	geo->uvs, 2*sizeof(float));
    GX_SetArray(GX_VA_TEX1,	geo->circleUvs, 2*sizeof(float));
    GX_InvVtxCache();

    
    GX_SetNumTexGens(2); // Number of texture
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY); // Texture coord type
    GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY); // Texture coord type

    // Noise Texture settings
    GXTexObj noiseTex;
    GX_InitTexObj(&noiseTex, geo->noiseData, geo->noiseWidth, geo->noiseHeight, GX_TF_I8, GX_REPEAT, GX_REPEAT, GX_FALSE);
    GX_InitTexObjFilterMode(&noiseTex, GX_NEAR, GX_NEAR);
    GX_LoadTexObj(&noiseTex, GX_TEXMAP0);

    // Circle Texture settings, recycle texobj
    GX_InitTexObj(&noiseTex, geo->circleData, geo->circleWidth, geo->circleHeight, GX_TF_I8, GX_REPEAT, GX_REPEAT, GX_FALSE);
    GX_LoadTexObj(&noiseTex, GX_TEXMAP1);

    GX_InvalidateTexAll();

    
    // Global TEV settings
    GX_SetNumTevStages(2);
    GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_NEVER, 0); // Do not discard if alpha > 126 || 0

    // TEV stage 0, (rand - h)
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR); // Just pass color, ignore texture, we will handle it in the alpha combinator

    // Alpha combinator equation:tevregid = (d (\a tevop) ((1.0 - c)*a + c*b) + \a tevbias) * \a tevscale;
    // Result: alpha = texture_alpha-color_alpha
    GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);

    // TEV stage 1, alpha-distance from center
    GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE1, GX_PASSCLR);

    // Same as lator alpha combinator
    GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
}

void Geometry_Draw(Geometry *geo){
    GX_Begin(GX_TRIANGLES, GX_VTXFMT0, geo->size);
    for(size_t i = 0; i < geo->size; i++){
        GX_Position1x8(i);
	    GX_Normal1x8(i);
        GX_Color1x8(geo->colorIndexes[i]);
        GX_TexCoord1x8(i);
        GX_TexCoord1x8(i);
    }
    GX_End();
}

*/