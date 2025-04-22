#define JSWEENY_H

#include "fonts.h"
#include "global_game.h"

// Functions to be used in Main file

void show_player_hearts(Rect *r, int yres);
float update_player_angle(float xCoordinate, float xPlayer, float yCoordinate, float yPlater);
void handle_shot(int &lastShot);
void game_over(Rect *r, int xres, int yres);
void reset_game_stats();
void render_pause_screen(Rect *r);