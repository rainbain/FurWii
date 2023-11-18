/*
 * WiiFur
 * By Rainbain
 * Oct 31, 2023
 *
 * Graphics
 * Holds graphics functions and handles, not designed to create
 * a layer of abstraction since a whole game engine is not needed here.
*/

#include "Graphics.h"

#include <gccore.h>

#include <string.h>
#include <malloc.h>

// Variables defined in headers, look there for better comments
Mtx Graphics_ViewMatrix;
Mtx Graphics_NormalMatrix;
Mtx44 Graphics_ProjectionMatrix;
GXColor Graphics_BackgroundColor;
GXRModeObj* Graphics_ScreenMode;
void* Graphics_FIFOBuffer;
void* Graphics_FrameBuffer;
bool Graphics_FrameReady;
float Graphics_DeltaTime;

s16	vertices[] ATTRIBUTE_ALIGN(32) = {
	0, 15, 0,
	-15, -15, 0,
	15,	-15, 0};

u8 colors[]	ATTRIBUTE_ALIGN(32)	= {
	255, 0,	0, 255,		// red
	0, 255,	0, 255,		// green
	0, 0, 255, 255};	// blue

// Furture rain, dont forget to check culling and gamma
void Graphics_Initalize(){
    Graphics_BackgroundColor.r = 135;
    Graphics_BackgroundColor.g = 206;
    Graphics_BackgroundColor.b = 235;
    Graphics_BackgroundColor.a = 255;
    Graphics_DeltaTime = 0.0f;
    
    VIDEO_Init();

    // Video presentation settings
    Graphics_ScreenMode = VIDEO_GetPreferredMode(0);
    Graphics_FrameBuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(Graphics_ScreenMode));

    // Video interface settings
    VIDEO_Configure(Graphics_ScreenMode);
    VIDEO_SetNextFramebuffer(Graphics_FrameBuffer);
    VIDEO_SetPostRetraceCallback(Graphics_OnFrameCopy);
    VIDEO_SetBlack(FALSE); // maybe wait a second, but na
    VIDEO_Flush();

    // FIFO alloc
    Graphics_FIFOBuffer = MEM_K0_TO_K1(memalign(32,GRAPHICS_FIFO_SIZE)); //32 byte alined fifo due to 128 bit addressing 16 byte burst
    memset(Graphics_FIFOBuffer,	0, GRAPHICS_FIFO_SIZE);

    // GX Settings, pretty standured, just a tad bit more involved.
    GX_Init(Graphics_FIFOBuffer, GRAPHICS_FIFO_SIZE);
    GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    GX_SetCopyClear(Graphics_BackgroundColor, 0x00ffffff);
    GX_SetViewport(0,0,Graphics_ScreenMode->fbWidth,Graphics_ScreenMode->efbHeight,0,1); // Classic viewport settings
    GX_SetDispCopyYScale((f32)Graphics_ScreenMode->xfbHeight/(f32)Graphics_ScreenMode->efbHeight);
    GX_SetScissor(0,0,Graphics_ScreenMode->fbWidth,Graphics_ScreenMode->efbHeight);
    GX_SetDispCopySrc(0,0,Graphics_ScreenMode->fbWidth,Graphics_ScreenMode->efbHeight);
    GX_SetDispCopyDst(Graphics_ScreenMode->fbWidth,Graphics_ScreenMode->xfbHeight);
    GX_SetCopyFilter(Graphics_ScreenMode->aa,Graphics_ScreenMode->sample_pattern, GX_TRUE,Graphics_ScreenMode->vfilter);
    GX_SetFieldMode(Graphics_ScreenMode->field_rendering, ((Graphics_ScreenMode->viHeight==2*Graphics_ScreenMode->xfbHeight)?GX_ENABLE:GX_DISABLE));
    
    GX_SetCullMode(GX_CULL_NONE);
    GX_CopyDisp(Graphics_FrameBuffer,GX_TRUE);
    GX_SetDispCopyGamma(GX_GM_1_0);

    // Camera Settings
    guVector camPos = {0.0f, 0.0f, 0.0f};
    guVector camUp = {0.0f, 1.0f, 0.0f}; // Y up
    guVector camLook = {0.0f, 0.0f, -1.0f}; // Looking into the screen
    guLookAt(Graphics_ViewMatrix, &camPos,	&camUp, &camLook);

    guPerspective(Graphics_ProjectionMatrix, 60, 1.33F, 0.1F,	100.0F); // 60 fov, 10 near 300 far.
    GX_LoadProjectionMtx(Graphics_ProjectionMatrix, GX_PERSPECTIVE);

    // CP vertex settings and standured TEV
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
    GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,	GX_POS_XYZ,	GX_S16,	0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8,	0);
    GX_SetArray(GX_VA_POS, vertices, 3*sizeof(s16));
    GX_SetArray(GX_VA_CLR0,	colors,	4*sizeof(u8));
    GX_SetNumChans(1);
    GX_SetNumTexGens(0);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    // Light
    GXLightObj light0;
    GX_InitLightPos(&light0, 0.0f, 100.0f, 0.0f);
    GX_InitLightColor(&light0, (GXColor){255,254,232,255});
    GX_InitLightDir(&light0, -100.0f, -500.0f, -100.0f);
    GX_InitLightDistAttn(&light0, 100.0f, 1.0f, GX_DA_GENTLE);
    GX_InitLightSpot(&light0,15.0f,GX_SP_FLAT);

    GX_SetChanCtrl(GX_COLOR0, GX_TRUE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_SPOT);

    GX_SetChanAmbColor(0, (GXColor){61, 60, 54, 255});
    //GX_SetChanMatColor(0, (GXColor){139, 95, 227, 255});

    GX_LoadLightObj(&light0,GX_LIGHT0);

    guMtxIdentity(Graphics_NormalMatrix);
}


void Graphics_Begin(){
    GX_SetViewport(0,0,Graphics_ScreenMode->fbWidth,Graphics_ScreenMode->efbHeight,0,1);
    GX_InvVtxCache(); // Do this every frame to be safe, or at least just when modifying geometry or vertex settings
    GX_InvalidateTexAll();

    Mtx	modelView;
    Mtx	model;
    guMtxIdentity(model);
    guMtxTransApply(model, model, 0.0F,	-1.0F, -3.0F);
    guMtxConcat(Graphics_ViewMatrix,model,modelView);
    GX_LoadPosMtxImm(modelView,	GX_PNMTX0);
    GX_LoadNrmMtxImm(Graphics_NormalMatrix,GX_PNMTX0);
    GX_SetCurrentMtx(0);

    Graphics_DeltaTime = 1.0f / 60.0f; // Best not to ask
}

void Graphics_End(){
    GX_DrawDone();
    Graphics_FrameReady = true;
    VIDEO_WaitVSync();
}


void Graphics_OnFrameCopy(uint32_t unk0){
    if(Graphics_FrameReady) {
        GX_SetZMode(GX_TRUE, GX_LEQUAL,	GX_TRUE);
        GX_SetColorUpdate(GX_TRUE);
        GX_CopyDisp(Graphics_FrameBuffer,GX_TRUE);
        GX_Flush();
        Graphics_FrameReady = false;
    }
}