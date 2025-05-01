#include "fonts.h"
#include "global_game.h"
#include "jcornejo.h"
#include "types.h"
#include <GL/glx.h>
#include <GLFW/glfw3.h>

extern void render();

void levelText(Rect *r)
{
    ggprint8b(r, 16, 0x00ffff00, "Level %d", gl.current_level);
} 


void draw() { 

    glColor3fv(g.ship.color);
    glPushMatrix();
    glTranslatef(g.ship.pos[0], g.ship.pos[1], g.ship.pos[2]);

    glRotatef(g.ship.angle, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-10.0f, -10.0f);
    glVertex2f(  0.0f, 20.0f);
    glVertex2f( 10.0f, -10.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(-12.0f, -10.0f);
    glVertex2f(  0.0f,  20.0f);
    glVertex2f(  0.0f,  -10.0f);
    glVertex2f(  0.0f,  -10.0f);
    glVertex2f(  0.0f,  20.0f);
    glVertex2f( 12.0f, -10.0f);
    glEnd();
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    glVertex2f(0.0f, 0.0f);
    glEnd();
    glPopMatrix();
    if (gl.keys[XK_Up] || g.mouseThrustOn) {
        int i;
        //draw thrust
        Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
        //convert angle to a vector
        Flt xdir = cos(rad);
        Flt ydir = sin(rad);
        Flt xs,ys,xe,ye,r;
        glBegin(GL_LINES);
        for (i=0; i<16; i++) {
            xs = -xdir * 11.0f * 4.0 - 2.0;
            ys = -ydir * 11.0f * 4.0 - 2.0;
            r = 40.0+40.0;
            xe = -xdir * r * 18.0 - 9.0;
            ye = -ydir * r * 18.0 - 9.0;
            glColor3f(.3+.7, .3+.7, 0);
            glVertex2f(g.ship.pos[0]+xs,g.ship.pos[1]+ys);
            glVertex2f(g.ship.pos[0]+xe,g.ship.pos[1]+ye);
        }
        glEnd();
    }

}