// jack sweeny source

// INCLUDES
#include "fonts.h"
#include "jsweeny.h"

void show_player_hearts (Rect *r, int yres, int playerHealth)
{
    r->bot = yres - 30;

    for (int i=0; i < playerHealth; i++) {
        ggprint8b(r, 24, 0xffff0000, "H");
        // Displays Hearts All in one row with 20 Gap between them.
        r->left += 20;
        r->bot = yres - 30;

        // Add score Here and other Game metrics as needed
    }
}

// void collision_detection (float shipXPosition, float shipYPosition, ) 
// {

// }