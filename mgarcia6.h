#ifndef MGARCIA_H
#define MGARCIA6_H

#include "fonts.h"
#include "global_game.h"

//Function(s) I had in main

void draw_player_health_bar(float x, float y, int health, int max_health);

void spawn_medkit();
void check_medkit_collision();
void update_medkit();
void render_medkit();

#endif
