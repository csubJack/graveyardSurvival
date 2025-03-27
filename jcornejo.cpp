#include "fonts.h"
#include "global_game.h"
void levelText (Rect *r)
{
    ggprint8b(r, 16, 0x00ffff00, "Level %d", gl.current_level);
}
