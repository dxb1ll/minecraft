#pragma once

#include <stdint.h>

// Wymiary świata
#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 16
#define CHUNK_DEPTH 16

// Typy bloków
enum BlockType : uint8_t {
    BLOCK_AIR = 0,
    BLOCK_GRASS,
    BLOCK_STONE,
    BLOCK_WOOD,
    BLOCK_LEAVES,
    TEXTURE_GRASS_SIDE
};
