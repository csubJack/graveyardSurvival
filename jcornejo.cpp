#include "fonts.h"
#include "global_game.h"
#include "jcornejo.h"

void levelText (Rect *r)
{
    ggprint8b(r, 16, 0x00ffff00, "Level %d", gl.current_level);
}

/*class SpriteSheet {
    int spriteWidth, spriteHeight;
    int texWidth, texHeight;

    int tex;

public:
    SpriteSheet(int t, int tW, int tH, int sW, int sH)
    : tex(t), texWidth(tW), texHeight(tH), spriteWidth(sW), spriteHeight(sH)
    {}

    void drawSprite(float posX, float posY, int frameIndex);
};


void drawSprite(float posX, float posY, int frameIndex) {
    const float verts[] = {
        posX, posY,
        posX + spriteWidth, posY,
        posX + spriteWidth, posY + spriteHeight,
        posX, posY + spriteHeight
    };
    const float tw = float(spriteWidth) / texWidth;
    const float th = float(spriteHeight) / texHeight;
    const int numPerRow = texWidth / spriteWidth;
    const float tx = (frameIndex % numPerRow) * tw;
    const float ty = (frameIndex / numPerRow + 1) * th;
    const float texVerts[] = {
        tx, ty,
        tx + tw, ty,
        tx + tw, ty + th,
        tx, ty + th
    };

    // ... Bind the texture, enable the proper arrays

    glVertexPointer(2, GL_FLOAT, verts);
    glTexCoordPointer(2, GL_FLOAT, texVerts);
    glDrawArrays(GL_TRI_STRIP, 0, 4);
}

};*/