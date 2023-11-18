/*
 * WiiFur
 * By Rainbain
 * Nov 1, 2023
 *
 * MathUtil
 * Some utilitys missing from other locations
*/

#pragma once

#include <gccore.h>

typedef struct {
    float s;
    float t;
} UV;

typedef struct {
    Mtx rotationMatrix;
    Mtx modelMatrix; // Before position for cache coherence
    Mtx scaleMatrix;
    Mtx positionMatrix;
} Transform;

void MathUtil_Rotation(Mtx mat, guVector rot);
void MathUtil_Transform(Transform *t, guVector pos, guVector rot, guVector scale);

uint8_t U8_Lerp(uint8_t a, uint8_t b, float t);
GXColor Color_Lerp(GXColor a, GXColor b, float t);