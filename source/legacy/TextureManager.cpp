#include "TextureManager.h"
#include "grass_top.h"
#include "grass_side.h"
#include "stone.h"
#include "wood.h"
#include "leaves.h"

void TextureManager::init() {
    glEnable(GL_TEXTURE_2D);
    vramSetBankA(VRAM_A_TEXTURE);
    loadTextures();
}

void TextureManager::loadTextures() {
    glGenTextures(TEXTURE_GRASS_SIDE + 1, textures);

    glBindTexture(0, textures[BLOCK_GRASS]);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)grass_top_bin);

    glBindTexture(0, textures[TEXTURE_GRASS_SIDE]);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)grass_side_bin);

    glBindTexture(0, textures[BLOCK_STONE]);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)stone_bin);

    glBindTexture(0, textures[BLOCK_WOOD]);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)wood_bin);

    glBindTexture(0, textures[BLOCK_LEAVES]);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_16, TEXTURE_SIZE_16, 0, TEXGEN_TEXCOORD, (u8*)leaves_bin);
}

int TextureManager::getTexture(BlockType t) const {
    return textures[t];
}
