// header file, with my own contribution to the code
//
//
#include "fonts.h"

void show_all(Rect *r)
{
    ggprint8b(r, 16, 0x00ff00ff, "Diego - Programmer");
    ggprint8b(r, 16, 0x00ff00ff, "Kenneth - Programmer");
    ggprint8b(r, 16, 0x00ff00ff, "Jack - Programmer");
    ggprint8b(r, 16, 0x00ff00ff, "Julio - Programmer");
    ggprint8b(r, 16, 0x00ff00ff, "Miguel - Programmer");
}
