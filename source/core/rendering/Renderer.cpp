#include "Renderer.h"
#include <nds.h>
#include <cmath>

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

    // draw target outline if player points at a block
    if (player->hasTarget()) {
        int tx = player->getTargetX();
        int ty = player->getTargetY();
        int tz = player->getTargetZ();

        glPushMatrix();
        glTranslatef(tx + 0.5f, ty + 0.5f, tz + 0.5f);

        // draw slightly larger white outline (approximate lines with quads)
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glScalef(1.02f, 1.02f, 1.02f);

        float t = 0.06f; // thicker than before
        glBegin(GL_QUADS);
        // edges along X (y,z fixed)
        // y=-0.5, z=-0.5
        glVertex3f(-0.5f, -0.5f - t, -0.5f - t); glVertex3f( 0.5f, -0.5f - t, -0.5f - t); glVertex3f( 0.5f, -0.5f + t, -0.5f + t); glVertex3f(-0.5f, -0.5f + t, -0.5f + t);
        // y=-0.5, z=0.5
        glVertex3f(-0.5f, -0.5f - t,  0.5f - t); glVertex3f( 0.5f, -0.5f - t,  0.5f - t); glVertex3f( 0.5f, -0.5f + t,  0.5f + t); glVertex3f(-0.5f, -0.5f + t,  0.5f + t);
        // y=0.5, z=-0.5
        glVertex3f(-0.5f,  0.5f - t, -0.5f - t); glVertex3f( 0.5f,  0.5f - t, -0.5f - t); glVertex3f( 0.5f,  0.5f + t, -0.5f + t); glVertex3f(-0.5f,  0.5f + t, -0.5f + t);
        // y=0.5, z=0.5
        glVertex3f(-0.5f,  0.5f - t,  0.5f - t); glVertex3f( 0.5f,  0.5f - t,  0.5f - t); glVertex3f( 0.5f,  0.5f + t,  0.5f + t); glVertex3f(-0.5f,  0.5f + t,  0.5f + t);

        // edges along Y (x,z fixed)
        // x=-0.5, z=-0.5dwmmam
        glVertex3f(-0.5f - t, -0.5f, -0.5f - t); glVertex3f(-0.5f + t, -0.5f, -0.5f - t); glVertex3f(-0.5f + t,  0.5f, -0.5f + t); glVertex3f(-0.5f - t,  0.5f, -0.5f + t);
        // x=0.5, z=-0.5
        glVertex3f( 0.5f - t, -0.5f, -0.5f - t); glVertex3f( 0.5f + t, -0.5f, -0.5f - t); glVertex3f( 0.5f + t,  0.5f, -0.5f + t); glVertex3f( 0.5f - t,  0.5f, -0.5f + t);
        // x=-0.5, z=0.5
        glVertex3f(-0.5f - t, -0.5f,  0.5f - t); glVertex3f(-0.5f + t, -0.5f,  0.5f - t); glVertex3f(-0.5f + t,  0.5f,  0.5f + t); glVertex3f(-0.5f - t,  0.5f,  0.5f + t);
        // x=0.5, z=0.5
        glVertex3f( 0.5f - t, -0.5f,  0.5f - t); glVertex3f( 0.5f + t, -0.5f,  0.5f - t); glVertex3f( 0.5f + t,  0.5f,  0.5f + t); glVertex3f( 0.5f - t,  0.5f,  0.5f + t);

        // edges along Z (x,y fixed)
        // x=-0.5, y=-0.5
        glVertex3f(-0.5f - t, -0.5f - t, -0.5f); glVertex3f( 0.5f - t, -0.5f - t, -0.5f); glVertex3f( 0.5f + t, -0.5f + t, -0.5f); glVertex3f(-0.5f + t, -0.5f + t, -0.5f);
        // x=0.5, y=-0.5
        glVertex3f(-0.5f - t, -0.5f - t,  0.5f); glVertex3f( 0.5f - t, -0.5f - t,  0.5f); glVertex3f( 0.5f + t, -0.5f + t,  0.5f); glVertex3f(-0.5f + t, -0.5f + t,  0.5f);
        // x=-0.5, y=0.5
        glVertex3f(-0.5f - t,  0.5f - t, -0.5f); glVertex3f( 0.5f - t,  0.5f - t, -0.5f); glVertex3f( 0.5f + t,  0.5f + t, -0.5f); glVertex3f(-0.5f + t,  0.5f + t, -0.5f);
        // x=0.5, y=0.5
        glVertex3f(-0.5f - t,  0.5f - t,  0.5f); glVertex3f( 0.5f - t,  0.5f - t,  0.5f); glVertex3f( 0.5f + t,  0.5f + t,  0.5f); glVertex3f(-0.5f + t,  0.5f + t,  0.5f);
        glEnd();

        glEnable(GL_TEXTURE_2D);
        glPopMatrix(1);
    }

    // draw crosshair in the center of the screen (2D overlay)
    // switch to orthographic projection matching the viewport (256x192)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 256, 192, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // draw a simple white crosshair (two thin quads) in pixel coordinates
    glDisable(GL_TEXTURE_2D);
    // force white color
    glColor3f(1.0f, 1.0f, 1.0f);

    // use a 2D-friendly poly format so the NDS renders the overlay reliably
    // add POLY_MODULATION so vertex color is honored for untextured polys
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(1));

    const float cx = 128.0f; // center x (exact)
    const float cy = 96.0f;  // center y (exact)
    const float arm = 6.0f;       // half-length of crosshair arms in pixels
    const float thickness = 1.0f; // half-thickness in pixels (reduce to 1)

    // translate to center and draw around origin to avoid coordinate-space issues
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);

    // ensure color right before drawing
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    // horizontal arm (centered at origin)
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(-arm, -thickness, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f( arm, -thickness, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f( arm,  thickness, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(-arm,  thickness, 0.0f);
    // vertical arm
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(-thickness, -arm, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f( thickness, -arm, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f( thickness,  arm, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(-thickness,  arm, 0.0f);
    glEnd();

    glPopMatrix(1); // pop the translate

    // restore 3D poly format used for blocks
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);

    glEnable(GL_TEXTURE_2D);

    glPopMatrix(1); // pop modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix(1); // pop projection
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::drawVisibleFaces(int x,int y,int z) {
    glPushMatrix();
    glTranslatef(x + 0.5f, y + 0.5f, z + 0.5f);
    t16 u_max = inttot16(16), v_max = inttot16(16);

    // block type
    BlockType blockType = static_cast<BlockType>(world->getBlock(x,y,z));

    // quick skylight check (stop early)
    bool skylit = true;
    for (int yy = y + 1; yy < CHUNK_HEIGHT; ++yy) {
        if (world->getBlock(x, yy, z) != BLOCK_AIR) { skylit = false; break; }
    }

    // compute simple per-column surface height once (cheap)
    int surfaceY = world->getHighestBlockHeight(x, z);

     // sun direction (normalized)
     float sx = 0.4f, sy = 0.8f, sz = -0.4f;
     float slen = sqrtf(sx*sx + sy*sy + sz*sz);
     if (slen != 0.0f) { sx /= slen; sy /= slen; sz /= slen; }

     // brighter, simpler lighting: ambient + directional sun
     auto faceBrightness = [&](float nx, float ny, float nz) {
         float ambient = 0.60f;   // increased ambient for overall brightness
         float sunStrength = 0.40f;
         float dot = nx*sx + ny*sy + nz*sz;
         if (dot < 0.0f) dot = 0.0f;
         float b = ambient + dot * sunStrength;
         if (!skylit) b *= 0.75f; // slightly darker if blocked from sky
         if (b < 0.0f) b = 0.0f;
         if (b > 1.0f) b = 1.0f;
         return b;
     };

    // simple per-face lighting (fast and safe)
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_MODULATION);
    glBegin(GL_QUADS);
        // top face (0,1,0)
        if (world->isTransparent(x, y + 1, z)) {
            float b = faceBrightness(0.0f, 1.0f, 0.0f);
            glColor3f(b,b,b);
            BlockType topTex = (blockType == BLOCK_GRASS) ? BLOCK_GRASS : blockType;
            glBindTexture(0, tex->getTexture(topTex));
            glTexCoord2t16(0, 0); glVertex3f(-0.5f, 0.5f,  0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f, 0.5f,  0.5f);
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, 0.5f, -0.5f);
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, 0.5f, -0.5f);
        }

        // bottom face (0,-1,0)
        if (world->isTransparent(x, y - 1, z)) {
            float b = faceBrightness(0.0f, -1.0f, 0.0f);
            glColor3f(b,b,b);
            BlockType bottomTex = (blockType == BLOCK_GRASS) ? BLOCK_STONE : blockType;
            glBindTexture(0, tex->getTexture(bottomTex));
            glTexCoord2t16(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f, -0.5f, -0.5f);
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, -0.5f,  0.5f);
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, -0.5f,  0.5f);
        }

        // side faces
        BlockType sideTex = (blockType == BLOCK_GRASS) ? TEXTURE_GRASS_SIDE : blockType;
        glBindTexture(0, tex->getTexture(sideTex));

        if (world->isTransparent(x, y, z - 1)) {
            float b = faceBrightness(0.0f, 0.0f, -1.0f);
            glColor3f(b,b,b);
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, -0.5f, -0.5f);
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2t16(0, 0); glVertex3f(-0.5f,  0.5f, -0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f,  0.5f, -0.5f);
        }
        if (world->isTransparent(x, y, z + 1)) {
            float b = faceBrightness(0.0f, 0.0f, 1.0f);
            glColor3f(b,b,b);
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, -0.5f,  0.5f);
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, -0.5f,  0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f,  0.5f,  0.5f);
            glTexCoord2t16(0, 0); glVertex3f(-0.5f,  0.5f,  0.5f);
        }
        if (world->isTransparent(x - 1, y, z)) {
            float b = faceBrightness(-1.0f, 0.0f, 0.0f);
            glColor3f(b,b,b);
            glTexCoord2t16(u_max, v_max); glVertex3f(-0.5f, -0.5f, -0.5f);
            glTexCoord2t16(0, v_max); glVertex3f(-0.5f, -0.5f,  0.5f);
            glTexCoord2t16(0, 0); glVertex3f(-0.5f,  0.5f,  0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f(-0.5f,  0.5f, -0.5f);
        }
        if (world->isTransparent(x + 1, y, z)) {
            float b = faceBrightness(1.0f, 0.0f, 0.0f);
            glColor3f(b,b,b);
            glTexCoord2t16(0, v_max); glVertex3f( 0.5f, -0.5f,  0.5f);
            glTexCoord2t16(u_max, v_max); glVertex3f( 0.5f, -0.5f, -0.5f);
            glTexCoord2t16(u_max, 0); glVertex3f( 0.5f,  0.5f, -0.5f);
            glTexCoord2t16(0, 0); glVertex3f( 0.5f,  0.5f,  0.5f);
        }
    glEnd();

     // restore default polyfmt
     glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);
     glPopMatrix(1);
 }
