#define DJIMENEZGARC_H
#include "fonts.h"
#include "global_game.h"

// functions for main file
void show_all(Rect *r, int xres, int yres, 
        float delta_time, int credits_activation);
void show_title(Rect *r, int xres, int yres);
void show_level_two_test();
void show_score();
void checking_level_transition();
void check_level_change_color();
void show_hat();
void move_slimes();
void checking_invincible_timer();

