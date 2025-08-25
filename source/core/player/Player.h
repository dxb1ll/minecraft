#pragma once

#include <nds.h>
#include "../world/World.h"

class Player {
public:
    Player(World* w);
    void initPosition();
    void update();
    float getX() const; float getY() const; float getZ() const;
    float getPitch() const; float getYaw() const;
    void setCameraTouch(const touchPosition &t);

    // raycast target getters
    bool hasTarget() const;
    int getTargetX() const; int getTargetY() const; int getTargetZ() const;
    int getTargetNX() const; int getTargetNY() const; int getTargetNZ() const;

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
    // adjusted so max jump height ≈ 1.0 block (v = sqrt(2*g*h))
    const float jumpVelocity = 0.35f;
    bool onGround = false;

    bool canMoveTo(float nx,float ny,float nz) const;

    // raycast target info
    bool targetValid = false;
    int targetX = 0, targetY = 0, targetZ = 0;
    int targetNX = 0, targetNY = 0, targetNZ = 0;

    void updateRaycast();
};
