// jack sweeny source

// INCLUDES
#include "fonts.h"
#include "jsweeny.h"
#include "types.h"
#include "global_game.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <ctime>

// TODO: add randomness  to the bomb power ups spawn time
		//  add collision detection for bullets and tombstones

bool verify_high_score() 
{
	return ((gl.player_score > gl.top_scores.back().first) && gl.player_score > 1);
}

void create_default_scoreboard() 
{
	// ensures size is empty before adding defualt values
	if (gl.top_scores.size() < 1) {
		for (int i=0; i <5; i++) {
			gl.top_scores.push_back({10, "ANON"}); // score, name
		}
	}
}

void render_leaderboard_page(Rect *r) 
{
	glColor3f(4.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2i(0,0);
	glVertex2i(0,gl.yres);
	glVertex2i(gl.xres,gl.yres);
	glVertex2i(gl.xres,0);
	glEnd();
    r->center = 30;
    r->bot = gl.yres / 2 + 160;
    r->left = gl.xres/2;
	ggprint16(r, 36, 0xffffffff, "Top 5 scores");
	ggprint8b(r, 24, 0xffffffff, "press U to return");
	char columnHeadings[100];
	sprintf(columnHeadings, "%-10s %-5s", "Name", "Score" );
	ggprint12(r, 24, 0xffffffff, columnHeadings);
	for (int i =0; i < 5; i++) {
		char row[100];
		sprintf(row, "%-10s  %-5d", gl.top_scores[i].second.c_str(), gl.top_scores[i].first);
		ggprint12(r, 24, 0xffffffff, row);

	}	

}

void show_player_hearts (Rect *r, int yres)
{
	// for (int i=0; i < gl.player_hearts; i++) {

	// }
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
	// std::cout << g.player.angle << std::endl;
}
void reset_game_stats () 
{
	gl.player_health = 10;
	gl.bullets_shot = 0;
	gl.monsters_killed = 0;
	gl.bullet_accuracy = 0;
	gl.pause_key_pressed = false;
	gl.game_paused = false;
	gl.title_hat = 1;
	gl.player_score = 0;
} 
void game_over(Rect *r, int xres, int yres) 
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2i(0,0);
	glVertex2i(0,yres);
	glVertex2i(xres,yres);
	glVertex2i(xres,0);

	glEnd();


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

	if (verify_high_score() && gl.player_score > 1) {
		ggprint16(r, 36, 0xffffffff, "New Top 5 Score!");

		// add high score
		gl.top_scores.push_back({gl.player_score, "ANON"}); // score, name
		for (size_t i=0; i < gl.top_scores.size(); i++) {
			for (size_t j=0; j< gl.top_scores.size()-1; j++ ) {
				if (gl.top_scores[j].first < gl.top_scores[j+1].first) {
					// save 1st as temp variable
					string tempName = gl.top_scores[j].second; 

					int tempScore = gl.top_scores[j].first;

					gl.top_scores[j].first = gl.top_scores[j+1].first;
					gl.top_scores[j].second = gl.top_scores[j+1].second;

					gl.top_scores[j+1].first = tempScore;
					gl.top_scores[j+1].second = tempName;

				}
			}

		}
		gl.top_scores.pop_back();
		gl.player_score = 0;

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

void handle_shot( int &lastShot) 
{
	if (!gl.game_paused && !gl.game_over_screen) {
		Bullet &b = g.barr[g.nbullets];
	// 	//convert player angle to radians
		Flt rad = ((g.player.angle+90.0) / 360.0f) * PI * 2.0;
		//convert angle to a vector
		Flt xdir = cos(rad);
		Flt ydir = sin(rad);
		Flt sideX = -ydir;
		Flt sideY = xdir;
		Flt rightOffset = 18.0f;
		Flt leftOffset = -18.0f;


		if (lastShot == 0) {
			// Right pistol shoots

			b.pos[0] = g.player.pos[0] + (sideX * rightOffset);
			b.pos[1] = g.player.pos[1] + (sideY * rightOffset);
			b.vel[0] = g.player.vel[0];
			b.vel[1] = g.player.vel[1];
			b.vel[0] += xdir*6.0f;
			b.vel[1] += ydir*6.0f;
			lastShot = 1;
			
		} else {
			// Left pistol shoots
			
			b.pos[0] = g.player.pos[0] + (sideX * leftOffset);
			b.pos[1] = g.player.pos[1] + (sideY * leftOffset);
			b.vel[0] = g.player.vel[0];
			b.vel[1] = g.player.vel[1];
		
			b.vel[0] += xdir*6.0f;
			b.vel[1] += ydir*6.0f;
			lastShot = 0;
		}

        ++g.nbullets;
		gl.bullets_shot++;  
	}

	
}

void render_pause_screen(Rect *r) 
{
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


void generate_nuke_drop(float size) 
{

	if (g.nuke.count < 1 && gl.current_level ==1 ) {
		srand(time(0));
		Nuke nuke;
		g.nuke.x_coordinate = 0 + rand() % (gl.xres + 1);
		g.nuke.y_coordinate = 0 + rand() % (gl.yres + 1);
		g.nuke.size = size;
		g.nuke.count = 1;
		g.nuke.nuke_active = true;
	}

}

void render_nuke() 
{
	if (gl.current_level != 1) {
		g.nuke.nuke_active = false;
	}

	if (g.nuke.nuke_active) {
		// glClear(GL_COLOR_BUFFER_BIT);
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_ONE, GL_ZERO);

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 0.0f);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(g.nuke.x_coordinate, g.nuke.y_coordinate);
		for (int i=0; i<20;i++) {
			float angle = i * 2.0f * PI / 19;
			float edge_x = cos(angle) * g.nuke.radius;
			float edge_y = sin(angle) * g.nuke.radius;
			glVertex2f(edge_x + g.nuke.x_coordinate, edge_y + g.nuke.y_coordinate);
		}
		glEnd();
		glEnable(GL_BLEND);
    	glEnable(GL_TEXTURE_2D);
		glPopAttrib();
	}
}

void delete_all_slimes(Slime *node) 
{
	// ensure player gets points for slimes destoted with power up
	gl.player_score += (int)(50.0 / (node->radius / 30.0));

	if (node->prev == NULL) {
        if (node->next == NULL) {
            // one item in list.
            g.slimeHead = NULL;
        } else {
            // start of list.
            node->next->prev = NULL;
            g.slimeHead = node->next;
        }
    } else {
        if (node->next == NULL) {
            // end of list.
            node->prev->next = NULL;
        } else {
            // middle of list.
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
    }
    delete node;
    node = NULL;
	g.nslimes--;
	
}
void nuke_collision() 
{
	float distance_x = g.player.pos[0] - g.nuke.x_coordinate;
	float distance_y = g.player.pos[1] - g.nuke.y_coordinate;

	float total_distance = sqrt(distance_x * distance_x  + distance_y * distance_y);

	if (total_distance < (g.nuke.radius + gl.player_size) ) {
		cout << "nuke collision" << endl;
		g.nuke.nuke_active = false;
		// g.nslimes = 0;
		int i=0;
		while (g.slimeHead && i < 6) {
			delete_all_slimes(g.slimeHead);
			i++;

		}
	}

}

