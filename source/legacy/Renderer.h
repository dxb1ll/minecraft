#pragma once

#include <nds.h>
#include "World.h"
#include "TextureManager.h"
#include "Player.h"

class Renderer {
public:
    Renderer(World* w, TextureManager* tm, Player* p);
    void init();
    void render();

private:
    World* world;
    TextureManager* tex;
    Player* player;
    void drawVisibleFaces(int x,int y,int z);
};
