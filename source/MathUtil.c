/*
 * WiiFur
 * By Rainbain
 * Nov 1, 2023
 *
 * MathUtil
 * Some utilitys missing from other locations
*/

#include "MathUtil.h"

void MathUtil_Rotation(Mtx mat, guVector rot){
    Mtx xm, ym, zm;
    c_guMtxRotRad(xm, 'X', rot.x);
    c_guMtxRotRad(ym, 'Y', rot.y);
    c_guMtxRotRad(zm, 'Z', rot.z);

    c_guMtxIdentity(mat);
    c_guMtxConcat(mat, xm, mat);
    c_guMtxConcat(mat, ym, mat);
    c_guMtxConcat(mat, zm, mat);
}

void MathUtil_Transform(Transform *t, guVector pos, guVector rot, guVector scale){
    c_guMtxTrans(t->positionMatrix, pos.x, pos.y, pos.z);
    MathUtil_Rotation(t->rotationMatrix, rot);
    c_guMtxScale(t->scaleMatrix, scale.x, scale.y, scale.z);
    c_guMtxConcat(t->positionMatrix, t->scaleMatrix, t->modelMatrix);
    c_guMtxConcat(t->modelMatrix, t->rotationMatrix, t->modelMatrix);
}


uint8_t U8_Lerp(uint8_t a, uint8_t b, float t){
    return a*(1.0f-t) + b*t;
}

GXColor Color_Lerp(GXColor a, GXColor b, float t){
    float tb = 1.0f - t;
    GXColor ret;
    ret.r = a.r*tb + b.r*t;
    ret.g = a.g*tb + b.g*t;
    ret.b = a.b*tb + b.b*t;
    ret.a = a.a*tb + b.a*t;
    return ret;
}