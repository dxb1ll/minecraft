#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "core/world/BlockTypes.h"
#include "core/rendering/TextureManager.h"
#include "core/world/World.h"
#include "core/player/Player.h"
#include "core/rendering/Renderer.h"

int main(void) {
    srand(time(NULL));

    World world;
    world.generate();

    TextureManager tex;
    Player player(&world);
    Renderer renderer(&world, &tex, &player);

    renderer.init();

    player.initPosition();

    while (pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();

        player.update();
        renderer.render();
        glFlush(0);
    }

    return 0;
}