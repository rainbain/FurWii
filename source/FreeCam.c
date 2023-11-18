/*
 * WiiFur
 * By Rainbain
 * Oct 31, 2023
 *
 * FreeCam
 * A simple 3D camera, not very plesent on a Wiimote.
*/

#include "FreeCam.h"

#include "Graphics.h"

#include <string.h>
#include <math.h>

#include <wiiuse/wpad.h>

void Camera_Initiate(Camera_Instance *cam, int controllerChannel){
    cam->position.x = 0.0f;
    cam->position.y = 0.0f;
    cam->position.z = 0.0f;
    cam->rotation.x = 0.0f;
    cam->rotation.y = 0.0f;
    cam->rotation.z = 0.0f;
    cam->controllerChannel = controllerChannel;
    cam->movementSpeed = 50.0f; // Units per second
    cam->rotationSpeed = 1.0f; // Radians per second
    cam->acceleration = 1.0f;
    cam->currentSpeed = 0.0f;
}

void Camera_Update(Camera_Instance* cam){
    uint32_t buttons = WPAD_ButtonsHeld(cam->controllerChannel);

    // Rotations
    float yawMovemenet = 0; // Y axis
    float pitchMovement = 0; // X axis
    if(buttons & WPAD_BUTTON_UP) pitchMovement -= cam->rotationSpeed;
    if(buttons & WPAD_BUTTON_DOWN) pitchMovement += cam->rotationSpeed;
    if(buttons & WPAD_BUTTON_LEFT) yawMovemenet -= cam->rotationSpeed;
    if(buttons & WPAD_BUTTON_RIGHT) yawMovemenet += cam->rotationSpeed;
    cam->rotation.y += yawMovemenet * Graphics_DeltaTime;
    cam->rotation.x += pitchMovement * Graphics_DeltaTime;

    if(cam->rotation.x > M_PI_2) cam->rotation.x = M_PI_2;
    if(cam->rotation.x < -M_PI_2) cam->rotation.x = -M_PI_2;

    // Rotatation Matrix
    Mtx viewRotationMatrix;
    Mtx moveRotationMatrix;
    Mtx pitchMat;
    Mtx yawMat;
    c_guMtxRotRad(pitchMat, 'X', cam->rotation.x);
    c_guMtxRotRad(yawMat, 'Y', cam->rotation.y);
    c_guMtxConcat(pitchMat, yawMat, viewRotationMatrix);
    c_guMtxConcat(yawMat, pitchMat, moveRotationMatrix);

    // Movements with rotation direction
    guVector movementVector = {0.0f, 0.0f, -1.0f};
    c_guVecMultiply(moveRotationMatrix, &movementVector, &movementVector);
    if(buttons & WPAD_BUTTON_A) cam->currentSpeed -= cam->acceleration * Graphics_DeltaTime; // Forward
    if(buttons & WPAD_BUTTON_B) cam->currentSpeed += cam->acceleration * Graphics_DeltaTime; // Backward
    cam->position.x += movementVector.x * cam->currentSpeed;
    cam->position.y += movementVector.y * cam->currentSpeed;
    cam->position.z -= movementVector.z * cam->currentSpeed;

    if(cam->currentSpeed > cam->movementSpeed){
        cam->currentSpeed = cam->movementSpeed;
    }

    cam->currentSpeed *= powf(0.01f, Graphics_DeltaTime);

    // Translation Matrix
    Mtx translatationMatrix;
    c_guMtxTrans(translatationMatrix, cam->position.x, cam->position.y, cam->position.z);

    // Multiply into view matrix
    c_guMtxCopy(viewRotationMatrix, Graphics_NormalMatrix);
    c_guMtxConcat(viewRotationMatrix, translatationMatrix, Graphics_ViewMatrix);
}