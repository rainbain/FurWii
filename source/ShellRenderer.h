/*
 * WiiFur
 * By Rainbain
 * Nov 2, 2023
 * 
 * ShellRenderer
 * Used to render shells around some geometry.
*/

#include "TextureAtlas.h"
#include "GeometryPool.h"
#include "Geometry.h"

#include "Palette.h"

typedef struct {
    bool useDistanceAttinuation; // Attinuate based on distance if specifyed.
    uint32_t numVerts;
    uint16_t numShells;
    UV uvScale;

    TextureAtlasEntry noiseTex;
    TextureAtlasEntry distanceTex;

    GeometryPoolEntry geometry;
} ShellRenderer;

/*
 * ShellRenderer_Initiate
 * Creates a shell renderer context
 * 
 * TextureAtlasEntry noiseTexture. Texture of varieus heights of the fur at diffrent points
 * Geometry geometry. Geometry we wish to create shells of
 * GeometryPool pool. Pool geometry will be stored in.
 * Palette *colorPallete. Color pallete to create colors from. The first colors will be the deeper shells
 * uint16_t numShells. The number of shells to create.
 * float height. Total height covered by the shell layers.
 * float densiry. Amount of "furs" over an aria. A value of one will be one big fur assuming uvs are square.
*/
extern void ShellRenderer_Initiate(ShellRenderer *sr, TextureAtlasEntry noiseTexture, Geometry* geometry, GeometryPool* pool, Palette *colorPallete, uint16_t numShells, float height, float density);


/*
 * ShellRenderer_DistanceAttinuation
 * Enable distance attenuation and generate the geometry (uvs) needed for that
 * 
 * TextureAtlasEntry distanceTex. A texture representing the distance from the middle divided by the thickness. Sets fur "shape"
*/
extern void ShellRenderer_DistanceAttinuation(ShellRenderer *sr, TextureAtlasEntry distanceTex);

/*
 * ShellRenderer_Setup
 * 
 * Must be called before drawing shells.
*/

extern void ShellRender_Setup(ShellRenderer *sr);

/*
 * ShellRenderer_Draw
 *
 * Draws the shells
*/

extern void ShellRenderer_Draw(ShellRenderer *sr);