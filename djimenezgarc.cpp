// Author: Diego Jimenez
// Purpoes: header file, with my own contribution to the code
// Application: Rendering text and backgrounds, keeping track of score
//
#include "fonts.h"
#include <GL/glx.h>
#include "types.h"
#include "global_game.h"
//#include "defs.h"
//taking this, might just put it in a header file later for macros
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
//---------------------------------------------------------------------------
void show_score();

void show_all(Rect *r, int xres, int yres,
        float delta_time, int credits_activation)
{
    static float scroll_speed = 50.0f;
    // changing this to keep track of credit positoin
    // used to be yres / 2 + 60
    static float bot_position = yres / 2 + 60;
    static bool is_scrolling = false;
    // changing this used to be !credits_actiavtion
    if (credits_activation && !is_scrolling) {
        //bot_position = yres / 2 + 60;
        is_scrolling = true; // starts the scroll
    }
    if (is_scrolling) {
        bot_position -= scroll_speed * delta_time;
    }
    // restarts the scrolling after it's off screen
    if (bot_position < -100) {
        //bot_position = yres / 2 + 60;
        is_scrolling = false;
        bot_position = yres / 2 + 60;
        return;
    }

    r->center = 30;
    r->bot = (int)bot_position;
    //r->bot = yres / 2 + 60;
    r->left = xres / 2;
    // edditing some values, started at 16
    // moving to 32 
    ggprint8b(r, 32, 0x00ff00ff, "Diego - Programmer");
    ggprint8b(r, 32, 0x00ff00ff, "Kenneth - Programmer");
    ggprint8b(r, 32, 0x00ff00ff, "Jack - Programmer");
    ggprint8b(r, 32, 0x00ff00ff, "Julio - Programmer");
    ggprint8b(r, 32, 0x00ff00ff, "Miguel - Programmer");
}

void show_title(Rect *r, int xres, int yres)
{
    gl.player_score = 0;
    //centering the title screen
    r->center = 30;
    r->bot = yres / 2 + 120;
    r->left = xres/2;

    ggprint16(r, 36, 0x00ff0000, "Graveyard Survival");

    r->bot = yres / 2 + 30;
    ggprint12(r, 24, 0xffffffff, "Press space to start");
    // any other instructions we want to give to the player
    r->bot = yres / 2 - 30;
    ggprint8b(r, 16, 0x00ff00ff, "up arrow to move");
    ggprint8b(r, 16, 0x00ff00ff, "left and right arrow to rotate");
    ggprint8b(r, 16, 0x00ff00ff, "space to fire");
}
//------------------------------

extern void levelText (Rect  *r);
extern void render();

void show_level_two_test()
{
    render();
    gl.current_level = 2;
    Rect r;
    r.center = 30;
    r.bot = gl.yres / 2 + 120;
    r.left = gl.xres/2;
    if (gl.player_score < 200)
    {
        ggprint16(&r, 16, 0x00ffffff, "Level Two");
    }
}

//---------------------
void show_score()
{
    if (gl.game_started) {
    Rect r;
    //gl.player_score = 0;
    r.bot = gl.yres - 80;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x00ff00ff, "Score: %i", gl.player_score);
    if (gl.game_started)
        gl.player_score += 1;
    }
}
void checking_level_transition()
{
    if (gl.player_score >= 100) {
        show_level_two_test();
    }
}

void check_level_change_color()
{
    if (gl.current_level == 1) 
        glClearColor(0.0, 0.0, 0.0, 0.0);
    else if (gl.current_level == 2)
        glClearColor(0.2, 0.2, 0.5, 1.0);
}

//void rendering_background() 
//{
    // a series of while loops could work
    //
//}


//-------------------------------------------------------------------
void show_hat()
{
    MakeVector(-150.0, 180.0, 0.0, hat.pos);
    MakeVector(6.0, 15.0, 0.0, hat.vel);
}
