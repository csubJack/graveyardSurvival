// jack sweeny source

// INCLUDES
#include "fonts.h"
#include "jsweeny.h"
#include "types.h"
#include "global_game.h"
#include <cmath>
#include <cstdio>
#include <iostream>



// TODO: 	   add bullet hit counter -- Not all monsters are one shot
//	     	   remove bg from player icon
//			   fix transparent part of hat from dealing damage
			// make window bigger -- adjust spawn points and barriers
			// collision detection between slime and barriers
			// spin render functions into personal file
			// add settings menu
			// add ability to jump from level to level

void show_player_hearts (Rect *r, int yres)
{
    r->bot = yres - 30;
	char monsters_killed_this_level[100];
	char bullets_accuracy_string_this_level[100];
	float bullets_accuracy_this_level = 0.0f;
	if (gl.bullets_shot > 0.0) {
		bullets_accuracy_this_level = (float)gl.monsters_killed / (float)gl.bullets_shot;

	}

	sprintf(monsters_killed_this_level, "Monsters Killed: %d", gl.monsters_killed);
	ggprint8b(r, 24, 0xffffff00, monsters_killed_this_level);

	sprintf(bullets_accuracy_string_this_level, "Accuracy: %f", bullets_accuracy_this_level);
	ggprint8b(r, 24, 0xffffff00, bullets_accuracy_string_this_level);
}

float update_player_angle (float xCoordinate, float xPlayer,float yCoordinate, float yPlayer) 
{
	float x = xCoordinate - xPlayer;
	float y = yCoordinate - yPlayer;
	return atan2(x,y) * (180 / M_PI) + 180;
	// std::cout << g.ship.angle << std::endl;
}


void reset_game_stats () {
	gl.player_health = 10;
	gl.bullets_shot = 0;
	gl.monsters_killed = 0;
	gl.bullet_accuracy = 0;
	gl.pause_key_pressed = false;
	gl.game_paused = false;
	gl.title_hat = 1;
} 
void game_over(Rect *r, int xres, int yres) {

	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_QUADS);

	glVertex2i(0,0);
	glVertex2i(0,yres);
	glVertex2i(xres,yres);
	glVertex2i(xres,0);

	glEnd();


	gl.player_score = 0;
    //centering the title screen
    r->center = 30;
    r->bot = yres / 2 + 120;
    r->left = xres/2;
	// Overall Accuracy Calculation
	float bullets_accuracy = 0.0f;
	// if not shots fired accurcay is zero
	if (gl.bullets_shot > 0.0) {
		bullets_accuracy = (float)gl.monsters_killed / (float)gl.bullets_shot;
	}


	char monsters_killed[100];
	char bullets_shot[100];
	char bullets_accuracy_string[100];

	sprintf(monsters_killed, "Monsters Killed: %d", gl.monsters_killed);
	sprintf(bullets_shot, "Bullers Shot: %d", gl.bullets_shot);
	sprintf(bullets_accuracy_string, "Bullet Accuracy: %f", bullets_accuracy);

    ggprint16(r, 36, 0x00ff0000, "Wasted");

    r->bot = yres / 2 + 30;
    ggprint16(r, 36, 0xffffffff, "Press space to return to title screen");
    ggprint12(r, 24, 0xffffffff, monsters_killed);
    ggprint12(r, 24, 0xffffffff, bullets_shot);
    ggprint12(r, 24, 0xffffffff, bullets_accuracy_string);

    r->bot = yres / 2 - 30;
   
	
}

void handle_shot( int &lastShot) {

    // std::cout << g.ship.angle << std::endl;

	if (!gl.game_paused && !gl.game_over_screen) {
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
		gl.bullets_shot++;  
	}

	
}

void render_pause_screen(Rect *r) {
	float xres = gl.xres;
	float yres = gl.yres;

	gl.game_paused = true;
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_QUADS);

	glVertex2i(0,0);
	glVertex2i(0,yres);
	glVertex2i(xres,yres);
	glVertex2i(xres,0);

	glEnd();
	r->center = 30;
    r->bot = yres / 2 + 120;
    r->left = xres/2;

	
    ggprint16(r, 36, 0x00ff0000, "Game Paused");

    r->bot = yres / 2 + 30;
    ggprint16(r, 36, 0xffffffff, "Press H to return to title screen");
    ggprint12(r, 24, 0xffffffff, "press P to Resume Game");
   

    r->bot = yres / 2 - 30;
   

}