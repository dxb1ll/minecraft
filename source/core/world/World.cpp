#include "World.h"
#include <cmath>
#include <stdlib.h>

World::World() {
    for (int x=0;x<CHUNK_WIDTH;++x) for (int y=0;y<CHUNK_HEIGHT;++y) for (int z=0;z<CHUNK_DEPTH;++z) blocks[x][y][z]=BLOCK_AIR;
}

void World::generate() {
    // flatter base terrain with gentle rolling hills
    float baseHeightF = CHUNK_HEIGHT * 0.25f;        // base ground level (lower than before)
    float hillFreq = 0.25f;                          // lower frequency => larger gentle hills
    float hillAmp = CHUNK_HEIGHT * 0.12f;            // small amplitude for hills

    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int z = 0; z < CHUNK_DEPTH; ++z) {
            float dx = (float)x;
            float dz = (float)z;

            // combine sines to produce smooth rolling terrain
            float noise = (sinf(dx * hillFreq) + cosf(dz * hillFreq * 0.9f) * 0.8f + sinf((dx + dz) * hillFreq * 0.5f) * 0.6f) / 3.0f;
            // small random jitter to break perfect repetition
            float jitter = ((rand() % 100) / 100.0f - 0.5f) * 0.6f;

            int height = (int)(baseHeightF + noise * hillAmp + jitter);
            if (height < 1) height = 1;
            if (height >= CHUNK_HEIGHT) height = CHUNK_HEIGHT - 1;

            for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                if (y > height) blocks[x][y][z] = BLOCK_AIR;
                else if (y == height) blocks[x][y][z] = BLOCK_GRASS;
                else blocks[x][y][z] = BLOCK_STONE;
            }
        }
    }

    // reduce tree density for flatter feel
    int treeCount = (CHUNK_WIDTH * CHUNK_DEPTH) / 120; // fewer trees
    for (int i = 0; i < treeCount; ++i) {
        int x = rand() % (CHUNK_WIDTH - 4) + 2;
        int z = rand() % (CHUNK_DEPTH - 4) + 2;
        int y = 0;
        for(int j = CHUNK_HEIGHT - 1; j >= 0; j--) if(blocks[x][j][z] == BLOCK_GRASS) { y = j; break; }
        if (y > 0) {
            int treeHeight = 3 + (rand() % 3);
            for (int h = 1; h <= treeHeight; ++h) if(y + h < CHUNK_HEIGHT) blocks[x][y + h][z] = BLOCK_WOOD;
            for (int ly = treeHeight - 1; ly <= treeHeight + 1; ++ly) {
                for (int lx = -2; lx <= 2; ++lx) {
                    for (int lz = -2; lz <= 2; ++lz) {
                        if(lx == 0 && lz == 0 && ly <= treeHeight) continue;
                        int checkX = x + lx;
                        int checkY = y + ly;
                        int checkZ = z + lz;
                        if(checkX >= 0 && checkX < CHUNK_WIDTH && checkY >= 0 && checkY < CHUNK_HEIGHT && checkZ >= 0 && checkZ < CHUNK_DEPTH) {
                            if (lx*lx + lz*lz < (ly == treeHeight ? 5 : 3)) {
                                if(blocks[checkX][checkY][checkZ] == BLOCK_AIR) blocks[checkX][checkY][checkZ] = BLOCK_LEAVES;
                            }
                        }
                    }
                }
            }
        }
    }
}

uint8_t World::getBlock(int x,int y,int z) const { return blocks[x][y][z]; }
void World::setBlock(int x,int y,int z,uint8_t t) { blocks[x][y][z]=t; }

bool World::isTransparent(int x,int y,int z) const {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH) return true;
    return blocks[x][y][z] == BLOCK_AIR || blocks[x][y][z] == BLOCK_LEAVES;
}

bool World::isSolidBlock(int x,int y,int z) const {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_DEPTH) return false;
    return blocks[x][y][z] != BLOCK_AIR && blocks[x][y][z] != BLOCK_LEAVES;
}

int World::getHighestBlockHeight(int x,int z) const {
    if (x < 0 || x >= CHUNK_WIDTH || z < 0 || z >= CHUNK_DEPTH) return 0;
    for (int y = CHUNK_HEIGHT - 1; y >= 0; --y) {
        if (blocks[x][y][z] != BLOCK_AIR) return y;
    }
    return 0;
}
