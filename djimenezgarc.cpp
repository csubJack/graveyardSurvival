// header file, with my own contribution to the code
//
//
#include "fonts.h"
#include <GL/glx.h>

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


void show_character_screen(Rect *r, int xres, int yres) 
{
//    int char_arr[10];
    r->center = 30;
    r->bot = yres / 2 + 120;
    r->left = xres/2;
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);




}


//void draw_Iris(class g.ship) 
//{

    ////glDisable(GL_TEXTURE_2D);
    //glPushMatrix();
    ////glLoadIdentity();
    //glColor3f(1.0,0.0,1.0);
    //glBegin(GL_TRIANGLES);
    //glPolygonMode(GL_FRONT, GL_LINE);
    //glLineWidth(4);
    //glBegin(GL_QUADS_STRIP);
    //glVertex2f(-0.25, 0.5);
    //glVertex2f(-0.25, -0.25);
    //glVertex2f(0.0, 0.0);
    //glVertex2f(0.0, -.25);
    //glVertex2f(0.25, 0.25);
    //glVertex2f(0.25, -0.25);
    //glEnd();
    
    //glVertex2f(-10.0f, -10.0f);
	//glVertex2f(  0.0f, 20.0f);
	//glVertex2f( 10.0f, -10.0f);
    //glColor3f(1.0f, 1.0f, 1.0f);
    //glVertex2f(-12.0f, -10.0f); // bottom left
	//glVertex2f(  0.0f,  20.0f); // top left
	//glVertex2f(  0.0f,  -10.0f); // bottom left center changing from -6 to -10
	//glVertex2f(  0.0f,  -10.0f); //bottom right center same as above
	//glVertex2f(  0.0f,  20.0f); // top right
	//glVertex2f( 12.0f, -10.0f); // bottom right
    //glEnd();
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glBegin(GL_POINTS);
	//glVertex2f(0.0f, 0.0f);
	//glEnd();
    //glPopMatrix();  
    //glEnable(GL_TEXTURE_2D);
    //glColor3fv();
  //  GLunit vbo;
  //  glGenBuffers(1, &vbo); // generates one buffer
  //  glBindBuffer(GL_ARRAY_BUFFER, vbo);
   // float vertices[] = {
     //   0.0f, 0.5f,
     //   0.5f, -0.5f,
    //    -0.5f, -0.5f
   // };
   // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//}

