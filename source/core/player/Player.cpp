#include "Player.h"
#include <cmath>
#include <math.h>

Player::Player(World* w) : world(w) {}

void Player::initPosition() {
    playerX = CHUNK_WIDTH * 0.5f;
    playerZ = CHUNK_DEPTH * 0.5f;
    cameraPitch = -15;
    cameraYaw = 0;
    // spawn slightly higher to avoid immediate clipping when standing on edges
    playerY = world->getHighestBlockHeight((int)playerX, (int)playerZ) + 1.1f + 0.01f;
    if (playerY < 1.0f) playerY = 1.0f;
    velocityY = 0.0f; onGround = true;
}

float Player::getX() const { return playerX; }
float Player::getY() const { return playerY; }
float Player::getZ() const { return playerZ; }
float Player::getPitch() const { return cameraPitch; }
float Player::getYaw() const { return cameraYaw; }

void Player::setCameraTouch(const touchPosition &t) { lastTouch = t; }

bool Player::canMoveTo(float nx,float ny,float nz) const {
    float halfW = playerHalfWidth;
    float playerH = playerHeight;
    const float eps = 0.0001f;

    float minX = nx - halfW;
    float maxX = nx + halfW - eps;
    float minY = ny;
    float maxY = ny + playerH - 0.001f - eps;
    float minZ = nz - halfW;
    float maxZ = nz + halfW - eps;

    int ix0 = (int)floor(minX);
    int ix1 = (int)floor(maxX);
    int iy0 = (int)floor(minY);
    int iy1 = (int)floor(maxY);
    int iz0 = (int)floor(minZ);
    int iz1 = (int)floor(maxZ);

    if (ix0 < 0) ix0 = 0; if (iy0 < 0) iy0 = 0; if (iz0 < 0) iz0 = 0;
    if (ix1 >= CHUNK_WIDTH) ix1 = CHUNK_WIDTH - 1; if (iy1 >= CHUNK_HEIGHT) iy1 = CHUNK_HEIGHT - 1; if (iz1 >= CHUNK_DEPTH) iz1 = CHUNK_DEPTH - 1;

    for (int ix = ix0; ix <= ix1; ++ix) {
        for (int iy = iy0; iy <= iy1; ++iy) {
            for (int iz = iz0; iz <= iz1; ++iz) {
                if (world->isSolidBlock(ix, iy, iz)) return false;
            }
        }
    }
    return true;
}

bool Player::hasTarget() const { return targetValid; }
int Player::getTargetX() const { return targetX; }
int Player::getTargetY() const { return targetY; }
int Player::getTargetZ() const { return targetZ; }
int Player::getTargetNX() const { return targetNX; }
int Player::getTargetNY() const { return targetNY; }
int Player::getTargetNZ() const { return targetNZ; }

void Player::updateRaycast() {
    // ray origin = eye position
    float ox = playerX;
    float oy = playerY + eyeHeight;
    float oz = playerZ;

    float pitchRad = cameraPitch * M_PI / 180.0f;
    float yawRad = cameraYaw * M_PI / 180.0f;

    // direction
    float dx = sinf(yawRad) * cosf(pitchRad);
    float dy = sinf(-pitchRad);
    float dz = -cosf(yawRad) * cosf(pitchRad);

    float maxDist = 6.0f; // zasięg raycastu
    float step = 0.05f;

    targetValid = false;

    for (float t = 0.0f; t <= maxDist; t += step) {
        float px = ox + dx * t;
        float py = oy + dy * t;
        float pz = oz + dz * t;

        int bx = (int)floor(px);
        int by = (int)floor(py);
        int bz = (int)floor(pz);

        if (bx < 0 || bx >= CHUNK_WIDTH || by < 0 || by >= CHUNK_HEIGHT || bz < 0 || bz >= CHUNK_DEPTH) continue;
        if (world->getBlock(bx,by,bz) != BLOCK_AIR) {
            targetValid = true;
            targetX = bx; targetY = by; targetZ = bz;
            // normal approximated by previous step position
            float prevx = px - dx * step;
            float prevy = py - dy * step;
            float prevz = pz - dz * step;
            targetNX = (int)floor(prevx);
            targetNY = (int)floor(prevy);
            targetNZ = (int)floor(prevz);
            // clamp
            if(targetNX<0) targetNX=0; if(targetNY<0) targetNY=0; if(targetNZ<0) targetNZ=0;
            if(targetNX>=CHUNK_WIDTH) targetNX=CHUNK_WIDTH-1; if(targetNY>=CHUNK_HEIGHT) targetNY=CHUNK_HEIGHT-1; if(targetNZ>=CHUNK_DEPTH) targetNZ=CHUNK_DEPTH-1;
            break;
        }
    }
}

void Player::update() {
    int held = keysHeld();
    int pressed = keysDown();

    // Camera controlled by face buttons: Y=left, A=right, X=up, B=down
    const float camSpeed = 3.5f; // degrees per frame while held (increased)
    if (held & KEY_Y) cameraYaw -= camSpeed;
    if (held & KEY_A) cameraYaw += camSpeed;
    if (held & KEY_X) cameraPitch -= camSpeed; // look up
    if (held & KEY_B) cameraPitch += camSpeed; // look down
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;

    // Jump on START button
    if ((pressed & KEY_START) && onGround) {
        velocityY = jumpVelocity; onGround = false;
    }

    velocityY += gravity;
    float newY = playerY + velocityY;

    if (canMoveTo(playerX, newY, playerZ)) {
        playerY = newY;
        int fx = (int)floor(playerX);
        int fz = (int)floor(playerZ);
        int h = world->getHighestBlockHeight(fx, fz);
        float groundY = h + 1.0f + 0.01f;
        if (playerY <= groundY + 0.001f) { playerY = groundY; velocityY = 0.0f; onGround = true; }
        else onGround = false;
    } else {
        if (velocityY < 0) {
            int hx = (int)floor(playerX);
            int hz = (int)floor(playerZ);
            int h = world->getHighestBlockHeight(hx, hz);
            playerY = h + 1.0f + 0.01f;
            velocityY = 0.0f; onGround = true;
        } else if (velocityY > 0) {
            int headStart = (int)floor(playerY + playerHeight);
            for (int y = headStart; y < CHUNK_HEIGHT; ++y) {
                if (world->isSolidBlock((int)floor(playerX), y, (int)floor(playerZ))) {
                    playerY = y - playerHeight - 0.001f; velocityY = 0.0f; break;
                }
            }
        }
    }

    // raycast update
    updateRaycast();

    // handle triggers (R = break, L = place)
    if ((pressed & KEY_R) && targetValid) {
        // destroy block at target
        world->setBlock(targetX, targetY, targetZ, BLOCK_AIR);
    }
    if ((pressed & KEY_L) && targetValid) {
        // place stone at nx/ny/nz if empty
        if (world->getBlock(targetNX, targetNY, targetNZ) == BLOCK_AIR) {
            world->setBlock(targetNX, targetNY, targetNZ, BLOCK_STONE);
        }
    }

    float speed = 0.15f;
    float angleRad = (cameraYaw * 3.14159265f) / 180.0f;
    float forwardX = sinf(angleRad);
    float forwardZ = -cosf(angleRad);
    float rightX = cosf(angleRad);
    float rightZ = sinf(angleRad);

    float moveX = 0.0f;
    float moveZ = 0.0f;

    if (held & KEY_UP)    { moveX += forwardX; moveZ += forwardZ; }
    if (held & KEY_DOWN)  { moveX -= forwardX; moveZ -= forwardZ; }
    if (held & KEY_LEFT)  { moveX -= rightX;   moveZ -= rightZ; }
    if (held & KEY_RIGHT) { moveX += rightX;   moveZ += rightZ; }

    float len = sqrtf(moveX*moveX + moveZ*moveZ);
    if (len > 0.001f) { moveX = (moveX / len) * speed; moveZ = (moveZ / len) * speed; }

    // No horizontal clipping: apply horizontal movement directly.
    // Vertical motion (falling / jumping) is still handled above by gravity and collisions.
    playerX += moveX;
    playerZ += moveZ;

    // keep player inside world bounds to avoid out-of-range indexing elsewhere
    const float margin = 0.05f;
    if (playerX < margin) playerX = margin;
    if (playerZ < margin) playerZ = margin;
    if (playerX > CHUNK_WIDTH - margin) playerX = CHUNK_WIDTH - margin;
    if (playerZ > CHUNK_DEPTH - margin) playerZ = CHUNK_DEPTH - margin;
}
