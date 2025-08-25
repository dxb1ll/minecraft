#pragma once

#include <nds.h>
#include "../world/BlockTypes.h"

class TextureManager {
public:
    void init();
    int getTexture(BlockType t) const;

private:
    int textures[TEXTURE_GRASS_SIDE + 1];
    void loadTextures();
};
