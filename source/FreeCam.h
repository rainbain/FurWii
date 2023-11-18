/*
 * WiiFur
 * By Rainbain
 * Oct 31, 2023
 *
 * FreeCam
 * A simple 3D camera, not very plesent on a Wiimote.
*/

#pragma once

#include <gccore.h>

typedef struct {
    guVector position;
    guVector rotation;
    int controllerChannel;
    float movementSpeed;
    float rotationSpeed;
    float acceleration;
    float currentSpeed;
} Camera_Instance;

extern void Camera_Initiate(Camera_Instance *cam, int controllerChannel);

extern void Camera_Update(Camera_Instance* cam);