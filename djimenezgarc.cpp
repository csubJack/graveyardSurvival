// header file, with my own contribution to the code
//
//
#include "fonts.h"
#include <GL/glx.h>
#include "types.h"
#include "global_game.h"
//taking this, might just put it in a header file later for macros
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)



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
   // if (!credits_activation) {
   //     is_scrolling = false;
   //     bot_position = yres / 2 + 60;
   // }
    if (is_scrolling) {
        bot_position -= scroll_speed * delta_time;
    }
    //bot_position -= scroll_speed * delta_time;
    // restarts the scrolling after it's off screen
    if (bot_position < -100) {
        //bot_position = yres / 2 + 60;
        is_scrolling = false;
        bot_position = yres / 2 + 60;
        return;
    }

    //bot_position -= scroll_speed * delta_time;

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
extern void draw_ship();
/*
extern void renderPLayerIcon(float playerX, float playerY,
        float playerSize, float playerAngle);
*/
void draw_Iris();
void show_character_screen(Rect *r, int xres, int yres) 
{
//    int char_arr[10];
    glClear(GL_COLOR_BUFFER_BIT);
    r->center = 30;
    r->bot = yres / 2 + 120;
    r->left = xres/2;
    ggprint16(r, 24, 0xffffffff, "Character Select Screen");
    
    r->bot = gl.yres - 20;
    r->left = 10;
    r->center = 0;
    ggprint8b(r, 16, 0x000000ff, "k to return to main screen");
    //glClear(GL_COLOR_BUFFER_BIT);
    //glColor3f(0.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    // need to draw ships
    // switching back to icons ?
    void (*draw_ships[])() = {draw_ship};
    // all the functions draw_ship, draw_Iris
    //draw_ship();
    for(int i = 0; i < 1; i++) {
        draw_ships[i]();

    }
    
}

void show_score(Rect *r, int yres, int player_score)
{
    r->bot = yres - 70;
    r->left = 10;
    r->center = 0;
    ggprint8b(r, 16, 0x00ff00ff, "Score: %i", player_score);
}

void show_hat()
{
    MakeVector(-150.0, 180.0, 0.0, hat.pos);
    MakeVector(6.0, 15.0, 0.0, hat.vel);
}

void show_hound() 
{
    MakeVector(-150.0, 180.0, 0.0, hound.pos);
    MakeVector(6.0, 15.0, 0.0, hound.vel);
}

