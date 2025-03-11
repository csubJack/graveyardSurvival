// jack sweeny source

// INCLUDES
#include "fonts.h"
#include "jsweeny.h"
#include <cmath>


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

float update_player_angle (float xCoordinate, float xPlayer,float yCoordinate, float yPlayer) 
{
	float x = xCoordinate - xPlayer;
	float y = yCoordinate - yPlayer;
	return atan2(x,y) * (180 / M_PI);
	// std::cout << g.ship.angle << std::endl;
}

// void collision_detection (float shipXPosition, float shipYPosition, ) 
// {

// }