// jack sweeny source

// INCLUDES
#include "fonts.h"
#include "jsweeny.h"
#include "types.h"
#include "global_game.h"
#include <cmath>


#include <iostream>




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

// void loadPlayerIcon() {


// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 	glGenTextures(1, &playerTexture);

// 	glEnable(GL_TEXTURE_2D);
// 	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


// 	glBindTexture(GL_TEXTURE_2D, playerTexture);


// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,img[1].width, img[1].height, 0,GL_RGB, GL_UNSIGNED_BYTE, img[1].data);	
	
// }

void handle_shot( int &lastShot) {

    // std::cout << g.ship.angle << std::endl;

	Bullet &b = g.barr[g.nbullets];
	// 	//convert ship angle to radians
		Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
		//convert angle to a vector
		Flt xdir = cos(rad);
		Flt ydir = sin(rad);

		Flt sideX = -ydir;
		Flt sideY = xdir;


		Flt rightOffset = 18.0f;
		Flt leftOffset = -18.0f;
		if (lastShot == 0) {
			// Right pistol shoots

			b.pos[0] = g.ship.pos[0] + (sideX * rightOffset);
			b.pos[1] = g.ship.pos[1] + (sideY * rightOffset);
			b.vel[0] = g.ship.vel[0];
			b.vel[1] = g.ship.vel[1];

			
			// b->pos[0] += xdir*20.0f;
			// b->pos[1] += ydir*20.0f;
			b.vel[0] += xdir*6.0f;
			b.vel[1] += ydir*6.0f;
			lastShot = 1;
			
		} else {
			// Left pistol shoots
			
			b.pos[0] = g.ship.pos[0] + (sideX * leftOffset);
			b.pos[1] = g.ship.pos[1] + (sideY * leftOffset);
			b.vel[0] = g.ship.vel[0];
			b.vel[1] = g.ship.vel[1];
		
			b.vel[0] += xdir*6.0f;
			b.vel[1] += ydir*6.0f;
			lastShot = 0;
		}

        ++g.nbullets;
}