/*
 * WiiFur
 * By Rainbain
 * Nov 4, 2023
 * 
 * MatRenderer
 * Used to render solid color objects.
*/

#include "MatRenderer.h"

void MatRenderer_Setup(){
    // Most basic tev order there can really be
    GX_SetNumTevStages(1);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
}

void MatRenderer_Draw(GeometryPoolEntry *poolEnetry){
    GX_Begin(GX_TRIANGLES, poolEnetry->pool->formatChannel, poolEnetry->points);
    for(uint16_t i = 0; i < poolEnetry->points; i++){
        GX_Position1x16(poolEnetry->startPosIdx+i);
        GX_Normal1x16(poolEnetry->startNormIdx+i);
        GX_Color1x16(poolEnetry->startColIdx);
    }
    GX_End();
}