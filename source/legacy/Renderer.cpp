#include "Renderer.h"
#include <nds.h>

Renderer::Renderer(World* w, TextureManager* tm, Player* p) : world(w), tex(tm), player(p) {}

void Renderer::init() {
    lcdMainOnTop();
    videoSetMode(MODE_0_3D);
    glInit();

    glEnable(GL_ANTIALIAS);
    tex->init();

    glClearColor(140, 206, 235, 31);
    glClearPolyID(63);
    glClearDepth(0x7FFF);
    glViewport(0, 0, 255, 191);

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 100);

    consoleDemoInit();
}

void Renderer::render() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(player->getPitch(), 1, 0, 0);
    glRotatef(player->getYaw(), 0, 1, 0);
    glTranslatef(-player->getX(), -(player->getY() + 1.6f), -player->getZ());

    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int y = 0; y < CHUNK_HEIGHT; ++y) {
            for (int z = 0; z < CHUNK_DEPTH; ++z) {
                if (world->getBlock(x,y,z) != BLOCK_AIR) drawVisibleFaces(x,y,z);
            }
        }
    }
}

void Renderer::drawVisibleFaces(int x,int y,int z) {
    glPushMatrix();
    glTranslatef(x + 0.5f, y + 0.5f, z + 0.5f);
    glColor3f(1,1,1);
    t16 u_max = inttot16(16), v_max = inttot16(16);

    // Block type as enum
    BlockType blockType = static_cast<BlockType>(world->getBlock(x,y,z));

    glBegin(GL_QUADS);
        if (world->isTransparent(x, y + 1, z)) {
            BlockType topTex = (blockType == BLOCK_GRASS) ? BLOCK_GRASS : blockType;
            glBindTexture(0, tex->getTexture(topTex));
            glTexCoord2t16(0, 0); glVertex3f(-0.5f, 0.5f,  0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f, 0.5f,  0.5f);
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, 0.5f, -0.5f);
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, 0.5f, -0.5f);
        }
        if (world->isTransparent(x, y - 1, z)) {
            BlockType bottomTex = (blockType == BLOCK_GRASS) ? BLOCK_STONE : blockType;
            glBindTexture(0, tex->getTexture(bottomTex));
            glTexCoord2t16(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f, -0.5f, -0.5f);
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, -0.5f,  0.5f);
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, -0.5f,  0.5f);
        }
        BlockType sideTex = (blockType == BLOCK_GRASS) ? TEXTURE_GRASS_SIDE : blockType;
        glBindTexture(0, tex->getTexture(sideTex));
        if (world->isTransparent(x, y, z - 1)) {
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, -0.5f, -0.5f);
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2t16(0, 0); glVertex3f(-0.5f,  0.5f, -0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f,  0.5f, -0.5f);
        }
        if (world->isTransparent(x, y, z + 1)) {
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, -0.5f,  0.5f);
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, -0.5f,  0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f,  0.5f,  0.5f);
            glTexCoord2t16(0, 0); glVertex3f(-0.5f,  0.5f,  0.5f);
        }
        if (world->isTransparent(x - 1, y, z)) {
            glTexCoord2t16(u_max, v_max); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, -0.5f,  0.5f);
            glTexCoord2t16(0, 0); glVertex3f(-0.5f,  0.5f,  0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f(-0.5f,  0.5f, -0.5f);
        }
        if (world->isTransparent(x + 1, y, z)) {
            glTexCoord2t16(0, v_max); glVertex3f( 0.5f, -0.5f,  0.5f);
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, -0.5f, -0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f,  0.5f, -0.5f);
            glTexCoord2t16(0, 0); glVertex3f( 0.5f,  0.5f,  0.5f);
        }
    glEnd();
    glPopMatrix(1);
}
