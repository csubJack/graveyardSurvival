#include "fonts.h"
#include "global_game.h"
#include "jcornejo.h"

void levelText(Rect *r)
{
    ggprint8b(r, 16, 0x00ffff00, "Level %d", gl.current_level);
}

void timeLeft(Rect *r)
{
    int time = 40;

    ggprint8b(r, 40, 0x00fff00, "TIME LEFT: %d", time);

}

