#include "Player.h"
#include <cmath>

Player::Player(World* w) : world(w) {
}

void Player::initPosition() {
    playerX = CHUNK_WIDTH * 0.5f;
    playerZ = CHUNK_DEPTH * 0.5f;
    cameraPitch = -15;
    cameraYaw = 0;
    playerY = world->getHighestBlockHeight((int)playerX, (int)playerZ) + 1.0f + 0.01f;
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

    // clamp to world bounds
    if (ix0 < 0) ix0 = 0;
    if (iy0 < 0) iy0 = 0;
    if (iz0 < 0) iz0 = 0;

    if (ix1 >= CHUNK_WIDTH) ix1 = CHUNK_WIDTH - 1;
    if (iy1 >= CHUNK_HEIGHT) iy1 = CHUNK_HEIGHT - 1;
    if (iz1 >= CHUNK_DEPTH) iz1 = CHUNK_DEPTH - 1;

    for (int ix = ix0; ix <= ix1; ++ix) {
        for (int iy = iy0; iy <= iy1; ++iy) {
            for (int iz = iz0; iz <= iz1; ++iz) {
                if (world->isSolidBlock(ix, iy, iz)) return false;
            }
        }
    }
    return true;
}

void Player::update() {
    int held = keysHeld();
    int pressed = keysDown();

    if (pressed & KEY_TOUCH) {
        touchRead(&lastTouch);
    } else if (held & KEY_TOUCH) {
        touchPosition touch;
        touchRead(&touch);
        int dx = touch.px - lastTouch.px;
        int dy = touch.py - lastTouch.py;
        cameraYaw -= dx * 0.3f;
        cameraPitch -= dy * 0.3f;
        if (cameraPitch > 89.0f) cameraPitch = 89.0f;
        if (cameraPitch < -89.0f) cameraPitch = -89.0f;
        lastTouch = touch;
    }

    if ((pressed & KEY_B) && onGround) {
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

    float stepHeight = 1.0f;
    float newX = playerX + moveX;
    float newZ = playerZ + moveZ;

    if (canMoveTo(newX, playerY, newZ)) { playerX = newX; playerZ = newZ; }
    else if (canMoveTo(newX, playerY + stepHeight, newZ)) { playerX = newX; playerZ = newZ; playerY += stepHeight; onGround = true; velocityY = 0.0f; }
    else {
        int tx = (int)floor(newX);
        int tz = (int)floor(newZ);
        if (tx >= 0 && tz >= 0 && tx < CHUNK_WIDTH && tz < CHUNK_DEPTH) {
            int th = world->getHighestBlockHeight(tx, tz);
            float targetY = th + 1.0f + 0.01f;
            if (canMoveTo(newX, targetY, newZ)) { playerX = newX; playerZ = newZ; playerY = targetY; onGround = true; velocityY = 0.0f; }
            else { if (canMoveTo(newX, playerY, playerZ)) playerX = newX; if (canMoveTo(playerX, playerY, newZ)) playerZ = newZ; }
        } else { if (canMoveTo(newX, playerY, playerZ)) playerX = newX; if (canMoveTo(playerX, playerY, newZ)) playerZ = newZ; }
    }
}
