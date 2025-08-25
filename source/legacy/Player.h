#pragma once

#include <nds.h>
#include "World.h"

class Player {
public:
    Player(World* w);
    void initPosition();
    void update(); // handle input + physics
    float getX() const; float getY() const; float getZ() const;
    float getPitch() const; float getYaw() const;
    void setCameraTouch(const touchPosition &t);

private:
    World* world;
    touchPosition lastTouch;

    float playerX, playerY, playerZ;
    float cameraPitch, cameraYaw;

    float playerHeight = 2.0f;
    float playerHalfWidth = 0.3f;
    float eyeHeight = 1.6f;

    float velocityY = 0.0f;
    const float gravity = -0.06f;
    const float jumpVelocity = 1.6f;
    bool onGround = false;

    bool canMoveTo(float nx,float ny,float nz) const;
};
