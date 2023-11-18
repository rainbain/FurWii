/*
 * WiiFur
 * By Rainbain
 * Nov 2, 2023
 * 
 * ShellRenderer
 * Used to render shells around some geometry.
*/

#include "Geometry.h"

#include "ShellRenderer.h"

// Create our shell layers and UVs
void ShellRenderer_GenerateGeometry(ShellRenderer *sr, Geometry* geometry, GeometryPool* pool, uint16_t numShells, float height){
    Geometry geometryGenerator;
    Geometry_Initiate(&geometryGenerator, geometry->index);

    sr->numVerts = numShells * geometry->index;

    // Not the fastest things here
    for(uint16_t i = 0; i < numShells; i++){
        float offset = (float)(i+1) / (float)numShells * height;

        Geometry_Clear(&geometryGenerator);
        Geometry_Insert(&geometryGenerator, geometry);
        Geometry_PushAlongNormals(&geometryGenerator, offset);
        Geometry_ScaleUVs(&geometryGenerator, sr->uvScale);
        GeometryPoolEntry ge = Geometry_PushToPool(&geometryGenerator, pool, GEOMETRY_CHANNEL_POSES | ((i == 0) ? (GEOMETRY_CHANNEL_UVS|GEOMETRY_CHANNEL_NORMS) : 0));

        if(i == 0) {
            sr->geometry = ge; // As an added bonus, our geometry pool entry will now contain the number of verts per shell
        }
    }
    Geometry_Free(&geometryGenerator);
}

// Generates the local uvs used for the distance texture
void ShellRender_GenerateLocalUVs(ShellRenderer *sr, float density){
    UV localUVs[sr->geometry.points];

    for(uint16_t i = 0; i < sr->geometry.points; i++){
        UV uv = sr->geometry.uvs[0][i];

        localUVs[i] = (UV){uv.s * (float)sr->noiseTex.width - (1.0f/(float)sr->noiseTex.width/2.0f), uv.t * (float)sr->noiseTex.height - (1.0f/(float)sr->noiseTex.width/2.0f)};
    }

    sr->geometry.startUvIdx[1] = sr->geometry.pool->numUvs[1];
    sr->geometry.uvs[1] = GeometryPool_PushUVs(sr->geometry.pool, localUVs, sr->geometry.points, 1);
}

// Create our color pallet. One per shell
void ShellRender_GeneratePallet(ShellRenderer *sr, Palette *colorPallete, GeometryPool* pool, uint16_t numShells) {
    GXColor colors[numShells];
    for(uint16_t i = 0; i < numShells; i++){
        float t = (float)(i+1) / (float)numShells;
        colors[i] = Palette_Interpolate(colorPallete, t);
    }

    sr->geometry.colors = GeometryPool_PushColors(pool, colors, numShells);
}

void ShellRenderer_Initiate(ShellRenderer *sr, TextureAtlasEntry noiseTexture, Geometry* geometry, GeometryPool* pool, Palette *colorPallete, uint16_t numShells, float height, float density){
    sr->uvScale = (UV){1.0f / (float)noiseTexture.width * density, 1.0f / (float)noiseTexture.height * density};
    sr->useDistanceAttinuation = false;
    sr->noiseTex = noiseTexture;
    sr->numShells = numShells;
    
    ShellRenderer_GenerateGeometry(sr, geometry, pool, numShells, height);
    ShellRender_GenerateLocalUVs(sr, density);
    ShellRender_GeneratePallet(sr, colorPallete, pool, numShells);
}

void ShellRenderer_DistanceAttinuation(ShellRenderer *sr, TextureAtlasEntry distanceTex){
    sr->useDistanceAttinuation = true;
    sr->distanceTex = distanceTex;
}

void ShellRender_Setup(ShellRenderer *sr){
    /*
     * Setup TEV stages. These will determin how our geometry is rasterized
    */
   GX_SetNumTevStages(sr->useDistanceAttinuation ? 2 : 1); // One tev stage if we dont want distance attinuation, 2 for it

    // Alpha combinator equation for refrence: tevregid = (d (\a tevop) ((1.0 - c)*a + c*b) + \a tevbias) * \a tevscale;

    // Of the two boolean alpha comparers, one will trigger when the alpha is greater than the constant, 0
    // The other will never trigger. Their results are combined with an OR gate
   GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_NEVER, 0);

   // We will be mimicing the function of (rand - h) - distance > 0
   GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, sr->noiseTex.atlas->texgen, GX_COLOR0A0); //Take in texture alpha and color data
   GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
   GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
   GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);

    // Tev stage 2
   if(sr->useDistanceAttinuation){
    GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, sr->distanceTex.atlas->texgen, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE1, GX_PASSCLR);
    GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
   }
}

void ShellRenderer_Draw(ShellRenderer *sr){
    GX_Begin(GX_TRIANGLES, sr->geometry.pool->formatChannel, sr->numVerts);
   for(uint16_t i = 0; i < sr->numShells; i++){
    for(uint16_t j = 0; j < sr->geometry.points; j++){
        GX_Position1x16(sr->geometry.startPosIdx+i*sr->geometry.points+j);
        GX_Normal1x16(sr->geometry.startNormIdx+j);
        GX_Color1x16(sr->geometry.startColIdx+i);
        GX_TexCoord1x16(sr->geometry.startUvIdx[0]+j);
        GX_TexCoord1x16(sr->geometry.startUvIdx[1]+j); // Always do other uvs, or else O_O
    }
   }
   GX_End();
}