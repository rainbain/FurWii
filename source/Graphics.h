/*
 * WiiFur
 * By Rainbain
 * Oct 31, 2023
 *
 * Graphics
 * Holds graphics functions and handles, not designed to create
 * a layer of abstraction since a whole game engine is not needed here.
*/

#pragma once

#include <gccore.h>

#include <stdbool.h>
#include <stdint.h>



// I would normally use the GX for the namespace here, but thats taken by gccore
#define GRAPHICS_FIFO_SIZE (256*1024)

// Stored in XF/BP mem, much like uniforms
extern Mtx Graphics_ViewMatrix;
extern Mtx Graphics_NormalMatrix;
extern Mtx44 Graphics_ProjectionMatrix;
extern GXColor Graphics_BackgroundColor;
extern GXRModeObj* Graphics_ScreenMode;

// A circular buffer of graphics commands
extern void* Graphics_FIFOBuffer;
extern void* Graphics_FrameBuffer;

// Our copy buffer function is called during vblank, this will keep track of if a frame is ready to copy
extern bool Graphics_FrameReady;

// Other usefull things
extern float Graphics_DeltaTime;


// Initalize vide interface
void Graphics_Initalize();

// Called at the beggining and end of a frame, not gl geometry calls!
void Graphics_Begin();
void Graphics_End();

void Graphics_OnFrameCopy(uint32_t unk0 /*Forgot what this argument is for*/);