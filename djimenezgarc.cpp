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
void show_level_three() 
{
    render();
    gl.current_level = 3;
    Rect r;
    r.center = 30;
    r.bot = gl.yres / 2 + 120;
    r.left = gl.xres/2;
    if (gl.player_score < 2200)
    {
        ggprint16(&r, 16, 0x00ffffff, "Level Three");
    }

}
void show_level_four()
{
    render();
    gl.current_level = 4;
    Rect r;
    r.center = 30;
    r.bot = gl.yres / 2 + 120;
    r.left = gl.xres/2;
    if (gl.player_score < 3200)
    {
        ggprint16(&r, 16, 0x00ffffff, "Level Four");
    }

}

//---------------------
// include time in here
void show_score()
{
    if (gl.game_started) {
    Rect r;
    //gl.player_score = 0;
    r.bot = gl.yres - 80;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x00ff00ff, "Score: %i", gl.player_score);
    //if (gl.game_started)
      //  gl.player_score += 1;
    }
}
void checking_level_transition()
{
    if (gl.player_score >= 1000 && (!(gl.player_score >= 2000))) {
        while (g.slimeHead) {
            Slime *s = g.slimeHead;
            g.slimeHead = g.slimeHead->next;
            delete s;
        }
        g.nslimes = 0;

        show_level_two_test();
    }
    else if (gl.player_score >= 2000 && (!(gl.player_score >= 3000))) {
        show_level_three();
    }
    else if (gl.player_score >= 3000) {
        show_level_four();
    }
}

void check_level_change_color()
{
    // changing this to white , all 0's before,
    // three 1's gives a purple look, looks nice actually
    if (gl.current_level == 1) //nevermind it's always red now
       glClearColor(0.0, 0.0, 0.0, 0.0);
    else if (gl.current_level == 2)
        glClearColor(0.2, 0.2, 0.5, 1.0);
    else if (gl.current_level == 3)
        glClearColor(0.3, 0.4, 0.7, 0.4);
    else if (gl.current_level == 4) {
        glClearColor(0.5, 0.5, 0.7, 0.6);
    }
}
//------------------------------------------------------------------
Tombstone tombstones[] = {
    {20, 0, TOMBSTONE_SHORTWIDE},
    {100, 0, TOMBSTONE_NORMAL},
    {100, 200, TOMBSTONE_NORMAL},
    {147, 0, TOMBSTONE_THIN},
    {175, 0, TOMBSTONE_NORMAL},
    {223, 0, TOMBSTONE_THIN},
    {250, 0, TOMBSTONE_CHUNKY},
    {250, 400, TOMBSTONE_CHUNKY},
    {375, 0, TOMBSTONE_THIN},
    {415, 0, TOMBSTONE_THIN},
    {455, 0, TOMBSTONE_THIN},
    {500, 0, TOMBSTONE_SHORTWIDE},
    {500, 200, TOMBSTONE_SHORTWIDE},
    {560, 0, TOMBSTONE_SHORTWIDE},
};
Tree witch[] = {
    {100, 0, WITCH_TREE},
    {100, 110, WITCH_TREE},
    {50, 400, WITCH_TREE},
    {150, 250, WITCH_TREE},
    {220, 600, WITCH_TREE},
    {500, 0, WITCH_TREE},
    {350, 110, WITCH_TREE},
    {600, 310, WITCH_TREE},
    {450, 210, WITCH_TREE},
    {300, 300, WITCH_TREE},
};
int num_tombstones = sizeof(tombstones) / sizeof(Tombstone);
int num_witch_items = sizeof(witch) / sizeof(Tree);

void get_tombstone_size(TombstoneType type, float *width, float *height)
{
    switch (type) {
        case TOMBSTONE_NORMAL:
            *width = 40;
            *height = 80;
            break;
        case TOMBSTONE_CHUNKY:
            *width = 100;
            *height = 70;
            break;
        case TOMBSTONE_THIN:
            *width = 20;
            *height = 100;
            break;
        case TOMBSTONE_SHORTWIDE:
            *width = 50;
            *height = 60;
            break;
        default:
            *width = 40;
            *height = 80;
            break;

    }
}
void get_tree_size(TreeType type, float *width, float *height)
{
    switch(type) {
        case WITCH_TREE:
            *width = 100;
            *height = 100;
            break;
        default:
            *width = 100;
            *height = 100;
            break;
    }
}
void draw_tombstone(float x, float y, float width, float height)
{
    glDisable(GL_TEXTURE_2D);
    // Base rectangle of the tombstone
    // gray is all 0.5, switching to 4f
    glColor3f(0.5f, 0.5f, 0.5f); // gray
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height * 0.8f);
        glVertex2f(x, y + height * 0.8f);
    glEnd();

    // Rounded top (semicircle) // might swap this out for a rectangle
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x + width / 2, y + height); // center
        for (int i = 0; i <= 20; i++) {
            float angle = 3.14159f * i / 20.0f; // 180 degrees
            float dx = cos(angle) * (width / 2);
            float dy = sin(angle) * (height * 0.2f);
            glVertex2f(x + width / 2 + dx, y + height * 0.8f + dy);
        }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}
void draw_trees(float x, float y, float width, float height)
{
    glDisable(GL_TEXTURE_2D);
    // then trunk dark brown
    glColor3f(0.15f, 0.05f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x - width * 0.2f, y);
        glVertex2f(x + width * 0.2f, y);
        glVertex2f(x + width * 0.1f, y + height * 0.5f);
        glVertex2f(x - width * 0.1f, y + height * 0.5f);
    glEnd();
    // leaves, dark green
    glColor3f(0.1f, 0.2f, 0.1f); 
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y + height * 0.7f); // top center
        for (int i = 0; i <= 360; i += 30) {
            float angle = i * 3.14159f / 180.0f;
            float radius = width * 0.6f;
            float px = x + cos(angle) * radius;
            float py = y + height * 0.7f + sin(angle) * radius * 0.5f;
            glVertex2f(px, py);
        }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}
void draw_witch_house(float x, float y, float width, float height)
{
    glDisable(GL_TEXTURE_2D);

    // Log Cabin Body
    glColor3f(0.3f, 0.0f, 0.4f); 
    glBegin(GL_QUADS);
        glVertex2f(x - width / 2, y);
        glVertex2f(x + width / 2, y);
        glVertex2f(x + width / 2, y + height);
        glVertex2f(x - width / 2, y + height);
    glEnd();

    // Roof
    glColor3f(0.4f, 0.1f, 0.3f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x, y + height + height * 0.5f);         // peak
        glVertex2f(x - width * 0.6f, y + height); // left
        glVertex2f(x + width * 0.6f, y + height); // right
    glEnd();

    // Door - center
    float doorWidth = width * 0.25f;
    float doorHeight = height * 0.4f;
    glColor3f(0.7f, 0.5f, 0.9f);
    glBegin(GL_QUADS);
        glVertex2f(x - doorWidth / 2, y);
        glVertex2f(x + doorWidth / 2, y);
        glVertex2f(x + doorWidth / 2, y + doorHeight);
        glVertex2f(x - doorWidth / 2, y + doorHeight);
    glEnd();

    // Window - small square on left
    float windowSize = width * 0.2f;
    glColor3f(0.6f, 0.4f, 0.8f); 
    glBegin(GL_QUADS);
        glVertex2f(x - width * 0.35f, y + height * 0.5f);
        glVertex2f(x - width * 0.35f + windowSize, y + height * 0.5f);
        glVertex2f(x - width * 0.35f + windowSize, y + height * 0.5f + windowSize);
        glVertex2f(x - width * 0.35f, y + height * 0.5f + windowSize);
    glEnd();
    if (gl.current_level == 3) {
        float roadWidth = 20.0f;
        float startY = y;
        float segments = 10;
        float curveAmplitude = 30.0f;
        float segmentHeight = 40.0f;

        glColor3f(0.2f, 0.2f, 0.2f); // dark grey road

        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= segments; ++i) {
            //float t = (float)i / segments;
            float posY = startY - i * segmentHeight;
            float offsetX = sinf(i * 0.6f) * curveAmplitude;

            // left edge of road
            glVertex2f(x + offsetX - roadWidth / 2, posY);
            // right edge of road
            glVertex2f(x + offsetX + roadWidth / 2, posY);
        }
    glEnd();
    }

    glEnable(GL_TEXTURE_2D);
}
void tombstone_physics() 
{
    // how the player interacts with all tombstones, prevents them from going through
    for (int i = 0; i < num_tombstones; i++) {
        float w, h;
        get_tombstone_size(tombstones[i].type, &w, &h);
        Tombstone *tomb_pos = &tombstones[i];
        //printf("just formed all the tomb_pos");
        if (g.ship.pos[0] + gl.player_size > tomb_pos->x 
                && g.ship.pos[0] - gl.player_size < tomb_pos->x + w 
                && g.ship.pos[1] + gl.player_size > tomb_pos->y 
                && g.ship.pos[1] - gl.player_size < tomb_pos->y + h) {
            // stop sliding
            g.ship.vel[0] = 0;
            g.ship.vel[1] = 0;

            float d0 = g.ship.pos[0] - tomb_pos->x;
            float d1 = g.ship.pos[1] - tomb_pos->y;
            float dist = sqrt(d0 * d0 + d1 * d1);
            if (dist > 0) {
                // Left side of tombstone (Player-tombstone collision)
                if (g.ship.pos[0] + gl.player_size > tomb_pos->x
                        && g.ship.pos[0] < tomb_pos->x) {
                    g.ship.pos[0] = tomb_pos->x - gl.player_size;
                }
                // Right side of tombstone
                else if (g.ship.pos[0] - gl.player_size < tomb_pos->x + w
                        && g.ship.pos[0] > tomb_pos->x + w) {
                    g.ship.pos[0] = tomb_pos->x + w + gl.player_size;
                }
                // Top side of tombstone
                else if (g.ship.pos[1] + gl.player_size > tomb_pos->y
                        && g.ship.pos[1] < tomb_pos->y) {
                    g.ship.pos[1] = tomb_pos->y - gl.player_size;
                }
                // Bottom side of tombstone
                else if (g.ship.pos[1] - gl.player_size < tomb_pos->y + h
                        && g.ship.pos[1] > tomb_pos->y + h) {
                    g.ship.pos[1] = tomb_pos->y + h + gl.player_size;
                }
            }
            //break;
        }

    }
    // how slimes react with tombstones
}
void witch_forest_physics()
{
    for (int i = 0; i < num_witch_items; i++) {
         float w, h;
        get_tree_size(witch[i].type, &w, &h);
        Tree *tree_pos = &witch[i];
        //-------------- Trunk collision-----------------------------------//
        // To stop movement at the base, want the leaves to slow down
        // player not stop them
        float trunk_width = w * 0.4f;
        float trunk_height = h * 0.5f;
        float tree_x = tree_pos->x - trunk_width / 2;
        float tree_y = tree_pos->y;

        if (g.ship.pos[0] + gl.player_size > tree_x
                && g.ship.pos[0] - gl.player_size < tree_x + trunk_width
                && g.ship.pos[1] + gl.player_size > tree_y
                && g.ship.pos[1] - gl.player_size < tree_y + trunk_height) {

            // stop movement
            g.ship.vel[0] = 0;
            g.ship.vel[1] = 0;

            float d0 = g.ship.pos[0] - tree_pos->x;
            float d1 = g.ship.pos[1] - tree_pos->y;
            float dist = sqrt(d0 * d0 + d1 * d1);
            if (dist > 0) {
                if (g.ship.pos[0] + gl.player_size > tree_x
                        && g.ship.pos[0] < tree_x) {
                    g.ship.pos[0] = tree_x - gl.player_size;
                }
                else if (g.ship.pos[0] - gl.player_size < tree_x + trunk_width
                        && g.ship.pos[0] > tree_x + trunk_width) {
                    g.ship.pos[0] = tree_x + trunk_width + gl.player_size;
                }
                else if (g.ship.pos[1] + gl.player_size > tree_y
                        && g.ship.pos[1] < tree_y) {
                    g.ship.pos[1] = tree_y - gl.player_size;
                }
                else if (g.ship.pos[1] - gl.player_size < tree_y + trunk_height
                        && g.ship.pos[1] > tree_y + trunk_height) {
                    g.ship.pos[1] = tree_y + trunk_height + gl.player_size;
                }
            }
        }
        //-------------------------Leaves Collision------------------------//
        float leaves_width = w * 1.2f;
        float leaves_height = h * 0.5;
        float leaves_x = tree_pos->x - leaves_width / 2;
        float leaves_y = tree_pos->y + trunk_height;
        if (g.ship.pos[0] + gl.player_size > leaves_x
                && g.ship.pos[0] - gl.player_size < leaves_x + leaves_width
                && g.ship.pos[1] + gl.player_size > leaves_y
                && g.ship.pos[1] - gl.player_size < leaves_y + leaves_height) {
            g.ship.vel[0] *= 0.5f;
            g.ship.vel[1] *= 0.5f;
        }
    }
}
void rendering_background() 
{
    if (gl.current_level == 1) {
        for (int i = 0; i < num_tombstones; i ++) {
            float w,h;
            get_tombstone_size(tombstones[i].type, &w,&h);
            draw_tombstone(tombstones[i].x, tombstones[i].y, w, h);
        }
    }
    else if (gl.current_level == 2) {
        for (int i = 0; i < num_witch_items; i ++) {
            float w,h;
            get_tree_size(witch[i].type, &w,&h);
            draw_trees(witch[i].x, witch[i].y, w, h);
        }
    }
    else if (gl.current_level == 3) {
        draw_witch_house(310,400, 50, 50);

    }
  /*   a series of while loops could work
     level 1 grave stones, tombs, and sarcaphagi
     level 2 witch's trees
     level 3 Witch's House
    */
}

//-------------------------------------------------------------------
void hat_taking_damage()
{
    // going up to 1.0 as in fully damaged
   if (!hat.being_hit) {
       hat.being_hit = true;
       hat.damage = 0.0f;
   }
   if (hat.being_hit) {
       hat.damage += 0.2f;
       gl.player_score += 250;
       if (hat.damage >= 1.0f) {
           hat.being_hit = false;
           hat.damage = 0.0f;
           gl.main_hat = 0;
       }
   }
}
void move_hat()
{
    if (gl.current_level == 1) {
        MakeVector(-150.0, 180.0, 0.0, hat.pos);
        MakeVector(6.0, 15.0, 0.0, hat.vel);
    }
}
void move_slimes()
{
    Slime * s = g.slimeHead;
    float speed = 0.5f;
    while (s) {
        float dx = gl.player_center_x - s->pos[0];
        float dy = gl.player_center_y - s->pos[1];

        float dist = sqrt(dx * dx + dy * dy);
        if(dist >0.1f) {
            dx /= dist;
            dy /= dist;

            s->vel[0] = dx * speed;
            s->vel[1] = dy * speed;
        } else {
            s->vel[0] = 0.0f;
            s->vel[1] = 0.0f;
        }
        s->pos[0] += s->vel[0];
        s->pos[1] += s->vel[1];

        s= s->next;
    }
}

void checking_invincible_timer() 
{
    if (gl.player_invincible) {
        gl.invincible_timer--;
        if (gl.invincible_timer <= 0) {
            gl.player_invincible = 0;
            gl.invincible_timer = 0;
        }
    }
}
