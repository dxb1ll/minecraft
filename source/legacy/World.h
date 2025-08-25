#pragma once

#include <stdint.h>
#include "BlockTypes.h"

class World {
public:
    World();
    void generate();
    uint8_t getBlock(int x,int y,int z) const;
    void setBlock(int x,int y,int z,uint8_t t);
    bool isTransparent(int x,int y,int z) const;
    bool isSolidBlock(int x,int y,int z) const;
    int getHighestBlockHeight(int x,int z) const;

private:
    uint8_t blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
};
