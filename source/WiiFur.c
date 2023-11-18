/*
 * WiiFur
 * By Rainbain
 * Oct 31, 2023
 *
 * WiiFur
 * Scene setting and initalization.
*/

#include "Graphics.h"
#include "FreeCam.h"
#include "TextureAtlas.h"
#include "GeometryPool.h"
#include "Geometry.h"
#include "Images.h"
#include "Palette.h"
#include "ShellRenderer.h"
#include "MatRenderer.h"
#include "Sphere.h"

#include <stdlib.h>

#include <wiiuse/wpad.h>

int main() {
    // Settings we shall use
    uint16_t shellNoiseResolution = 32;
    uint16_t shellDistanceResolution = 32;
    float thickness = 1.0f;
    uint16_t numberOfShells = 256;
    float height = 2.0f;
    float density = 64.0f;

    float palletAnkers[] = {1.0f/4.0*0.0,1.0f/4.0*1.0,1.0f/4.0*2.0,1.0f/4.0*3.0,1.0f/4.0*4.0};
    GXColor palletColors[] = {
        (GXColor){0x3d,0x34,0x8b,0xFF},
        (GXColor){0x76,0x78,0xed,0xFF},
        (GXColor){0xf7,0xb8,0x01,0xFF},
        (GXColor){0xf1,0x87,0x01,0xFF},
        (GXColor){0xf3,0x5b,0x04,0xFF}
    };

    Palette pallet = {palletAnkers,palletColors,sizeof(palletAnkers)/sizeof(float)};

    // Initalize graphics and gamepad
    Graphics_Initalize();
    WPAD_Init();

    // Create freecam on controller 0
    Camera_Instance freecam;
    Camera_Initiate(&freecam, 0);

    // Intinsity texture with near filter.
    TextureAtlas pixilatedMonochromeAtlas;
    TextureAtlas_Initiate(&pixilatedMonochromeAtlas, GX_TF_I8, GX_TEXMAP0, shellNoiseResolution, shellNoiseResolution, sizeof(uint8_t));
    TextureAtlas_SetFilterMode(&pixilatedMonochromeAtlas, GX_NEAR);

    // Intinsity texture with default filtering
    TextureAtlas smoothMonochromeAtlas;
    TextureAtlas_Initiate(&smoothMonochromeAtlas, GX_TF_I8, GX_TEXMAP1, shellDistanceResolution, shellDistanceResolution, sizeof(uint8_t));

    // Geometry pool for our shell geometry.
    GeometryPool shellGeometryPool;
    GeometryPool_Initiate(&shellGeometryPool, GX_VTXFMT0);

    // Geometry pool for our mat colored geometry
    GeometryPool matGeometryPool;
    GeometryPool_Initiate(&matGeometryPool, GX_VTXFMT1);

    // Generate our geometry we wish to render shells on
    GeometryPoolEntry baseGeometry; // Will render our mat base.
    Geometry geometryGenerator;
    {
        //uint32_t totalGeometrySize = Geometry_PlaneItch();
        uint32_t totalGeometrySize = Sphere_NumVerts;

        Geometry_Initiate(&geometryGenerator, totalGeometrySize);

        //Geometry_Plane(&geometryGenerator);
        Geometry_InsertArrays(&geometryGenerator, Sphere_NumVerts, Sphere_Vertices, Sphere_Normals, Sphere_UVs);

        // Scale it a bit
        Transform t;
        MathUtil_Transform(&t, (guVector){0.0f, 0.0f, 0.0f}, (guVector){0.0f, 0.0f, 0.0f}, (guVector){5.0f, 5.0f, 5.0f});

        Geometry_ApplyTransform(&geometryGenerator, &t);

        Geometry_SetColor(&geometryGenerator, pallet.colors[0]);

        baseGeometry = Geometry_PushToPool(&geometryGenerator, &matGeometryPool, GEOMETRY_CHANNEL_MAT);
    }

    // Generate our textures we use to render the shells
    TextureAtlasEntry shellNoiseTexture;
    TextureAtlasEntry shellDistanceMap;
    {
        uint8_t texDataTmp[shellNoiseResolution*shellNoiseResolution];
        Images_NoiseR8(texDataTmp, shellNoiseResolution*shellNoiseResolution, 255, 0);

        TextureAtlasEntry_Initiate(&shellNoiseTexture, &pixilatedMonochromeAtlas, 0, 0, shellNoiseResolution,shellNoiseResolution);
        TextureAtlasEntry_Put(&shellNoiseTexture, texDataTmp);
    }
    {
        uint8_t texDataTmp[shellDistanceResolution*shellDistanceResolution];
        Images_DistanceMapR8(texDataTmp, shellDistanceResolution,shellDistanceResolution, thickness);

        TextureAtlasEntry_Initiate(&shellDistanceMap, &smoothMonochromeAtlas, 0, 0, shellDistanceResolution,shellDistanceResolution);
        TextureAtlasEntry_Put(&shellDistanceMap, texDataTmp);
    }

    // Setup the alpha values. Alpha values are the same as the shell height
    Palette_LinerFill(&pallet, 0, 255, 3);

    // Create our shell renderer
    ShellRenderer shellRenderer;
    ShellRenderer_Initiate(&shellRenderer, shellNoiseTexture, &geometryGenerator, &shellGeometryPool, &pallet, numberOfShells, height, density);
    ShellRenderer_DistanceAttinuation(&shellRenderer, shellDistanceMap);

    // Load our settings onto the "GPU"
    GX_SetNumChans(2);
    GX_SetNumTexGens(2);

    TextureAtlas_Load(&pixilatedMonochromeAtlas);
    TextureAtlas_Load(&smoothMonochromeAtlas);
    GeometryPool_Load(&shellGeometryPool);
    GeometryPool_Load(&matGeometryPool);

    while(true){
        // Update controlls
        WPAD_ScanPads();
        Camera_Update(&freecam);

        // Draw scene
        Graphics_Begin();

        GeometryPool_Setup(&matGeometryPool);
        MatRenderer_Setup();
        MatRenderer_Draw(&baseGeometry);
        
        GeometryPool_Setup(&shellGeometryPool);
        ShellRender_Setup(&shellRenderer);
        ShellRenderer_Draw(&shellRenderer);

        Graphics_End();

        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);
    }

    TextureAtlas_Free(&pixilatedMonochromeAtlas);
    TextureAtlas_Free(&smoothMonochromeAtlas);
    Geometry_Free(&geometryGenerator);
    GeometryPool_Free(&shellGeometryPool);

    return 0;
    /*
    Graphics_Initalize();
    WPAD_Init();

    Camera_Instance camera;
    Camera_Initiate(&camera, 0);

    Geometry plane;
    Geometry_Allocate(&plane, Geometry_PlaneItch() * 64);
    Geometry_SetPallet(&plane, colorPallet);

    uint8_t *noiseImage = malloc(32*32*2);
    Images_NoiseR8(noiseImage, 32*32, 255-2, 2);

    uint8_t *circleImage = noiseImage + 32*32;
    Images_CircleR8(circleImage, 32, 32, 2.0f);

    Geometry_SetTextures(&plane, noiseImage, 32, 32, circleImage, 32, 32);

    for(uint32_t i = 0; i < 64; i++){
        Transform t;
        MathUtil_Transform(&t, (guVector){0.0f, (float)i * 0.01, 0.0f}, (guVector){0.0f, 0.0f, 0.0f}, (guVector){5.0f, 5.0f, 5.0f});

        Geometry_Plane(&plane, &t, i, 2.5f);
        // 13, 46, 29 -> 162, 242, 58

        float a = (float)i / 64.0f;

        colorPallet[i] = (GXColor){mix(13, 162, a), mix(46, 242, a), mix(29, 58, a), (uint8_t)(a*255.0f)};
    }

    Geometry_Load(&plane);
    

    while(true){
        WPAD_ScanPads();
        Camera_Update(&camera);

        Graphics_Begin();

        Geometry_Draw(&plane);

        Graphics_End();

        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);
    }

    free(noiseImage);

    Geometry_Free(&plane);
    */
}