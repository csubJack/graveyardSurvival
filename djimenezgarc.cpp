// Author: Diego Jimenez 
// Purpose: header file, with my own contribution to the code 
// Application: Rendering text and background 
// Drawing objects and applying physics, plus some transition functions 

#include "fonts.h" 
#include <GL/glx.h> 
#include "types.h" 
#include "global_game.h" 
//#include "defs.h" //taking this, might just put it in a header file later for macros 

#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0 
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z) 

//--------------------------------------------------------------------------- 
// Text and other functions related to it 
void show_score(); 
void integrateBossSystem(); 
struct SlimeBoss; 
extern SlimeBoss slimeBoss; 
extern bool slimeBossDefeated; 

void show_all(Rect *r, int xres, int yres, float delta_time, int credits_activation) { 
    static float scroll_speed = 50.0f; 
    static float bot_position = yres; 
    static bool is_scrolling = false; 
    
    if (credits_activation && !is_scrolling) { 
        //bot_position = yres / 2 + 60; 
        is_scrolling = true; // starts the scroll 
    } 
    
    //Allow for text to scroll 
    if (is_scrolling) { 
        bot_position -= scroll_speed * delta_time; 
        // stops scrolling after offscreen 
        if (bot_position < -100) { 
            is_scrolling = false; 
            gl.credits = 0; 
        } 
    } 
    
    // resetting the position of the text 
    if (gl.credits == 0 || credits_activation == 0) { 
        bot_position = yres; 
    } 
    
    if (is_scrolling) { 
        r->center = 30; 
        r->bot = (int)bot_position; 
        r->left = xres / 2; 
        ggprint8b(r, 32, 0x00ffff00, "Diego - Programmer"); 
        ggprint8b(r, 32, 0x00ffff00, "Kenneth - Programmer"); 
        ggprint8b(r, 32, 0x00ffff00, "Jack - Programmer"); 
        ggprint8b(r, 32, 0x00ffff00, "Julio - Programmer"); 
        ggprint8b(r, 32, 0x00ffff00, "Miguel - Programmer"); 
    } 
} 

void show_title(Rect *r, int xres, int yres) { 
    gl.player_score = 0; 
    
    // Setting the bounds for the moon 
    float moon_radius = ((xres < yres) ? xres : yres) / 7.0f; 
    float moon_x = xres - moon_radius - 20; 
    float moon_y = yres - moon_radius - 20; 
    float shine = 0.5f + 0.5f * sin((2 * M_PI / 30.0f) * gl.moon_shine_timer); 
    float moon_brightness = 0.5f + 0.5f * shine; 
    
    glDisable(GL_TEXTURE_2D); 
    glColor3f(moon_brightness, moon_brightness, moon_brightness); 
    glBegin(GL_TRIANGLE_FAN); 
    glVertex2f(moon_x, moon_y); // center 
    
    for (int i = 0; i <= 100; ++i) { 
        float angle = i * 2.0f * M_PI / 100; 
        float dx = cos(angle) * moon_radius; 
        float dy = sin(angle) * moon_radius; 
        glVertex2f(moon_x + dx, moon_y + dy); 
    } 
    glEnd(); 
    glEnable(GL_TEXTURE_2D); 
    
    //centering the title screen 
    r->center = 30; 
    r->bot = yres / 2 + 120; 
    r->left = xres/2; 
    ggprint16(r, 36, 0x00ff0000, "Graveyard Survival"); 
    
    r->bot = yres / 2 + 30; 
    ggprint12(r, 24, 0xffffffff, "Press space to start"); 
    
    r->bot = yres / 2 - 30; 
    ggprint8b(r, 16, 0x00ff00ff, "WAD to move"); 
    ggprint8b(r, 16, 0x00ff00ff, "Left Click to fire"); 
} 

extern void levelText (Rect *r); 
extern void render(); 

void show_level_one() { 
    render(); 
    Rect r; 
    r.center = 30; 
    r.bot = gl.yres / 2 + 120; 
    r.left = gl.xres/2; 
    if (gl.player_score == 0 && !gl.game_paused) { 
        ggprint16(&r, 16, 0x00ffffff, "Level One"); 
    } 
} 

void show_level_two_test() { 
    render(); 
    gl.current_level = 2; 
    Rect r; 
    r.center = 30; 
    r.bot = gl.yres / 2 + 120; 
    r.left = gl.xres/2; 
    if (gl.player_score < 1250 && !gl.game_paused) { 
        ggprint16(&r, 16, 0x00ffffff, "Level Two"); 
        r.bot = gl.yres / 2 + 150; 
        ggprint16(&r, 1, 0x00ff0000, "The Witch's Hat is Here"); 
    } 
} 

void show_level_three() { 
    render(); 
    gl.current_level = 3; 
    Rect r; 
    r.center = 30; 
    r.bot = gl.yres / 2 + 120; 
    r.left = gl.xres/2; 
    if (gl.player_score < 2200 && !gl.game_paused) { 
        ggprint16(&r, 16, 0x00ffffff, "Level Three"); 
    } 
} 

void show_level_four() { 
    render(); 
    gl.current_level = 4; 
    Rect r; 
    r.center = 30; 
    r.bot = gl.yres / 2 + 120; 
    r.left = gl.xres/2; 
    if (gl.player_score < 3200 && !gl.game_paused) { 
        ggprint16(&r, 16, 0x00ffffff, "Level Four"); 
    } 
} 

void show_score() { 
    if (gl.game_started) { 
        Rect r; 
        r.bot = gl.yres - 80; 
        r.left = 10; 
        r.center = 0; 
        ggprint8b(&r, 16, 0x00ffff00, "Score: %i", gl.player_score); 
        ggprint8b(&r, 16, 0x00ffff00, "Time: %i", gl.regular_timer); 
    } 
} 

//---------------------------------------------------------------- 
// Miscellaneous

void checking_level_transition() { 
    if (gl.player_score == 0 && gl.game_started) 
        show_level_one(); 
    
    if (gl.player_score >= 1000 && (!(gl.player_score >= 2000))) { 
        while (g.slimeHead) { 
            Slime *s = g.slimeHead; 
            g.slimeHead = g.slimeHead->next; 
            delete s; 
        } 
        g.nslimes = 0; 
        show_level_two_test(); 
    } else if (gl.player_score >= 2000 && (!(gl.player_score >= 3000))) { 
        show_level_three(); 
    } else if (gl.player_score >= 3000) { 
        // Check if we're in level 3, boss is defeated, and all slimes are cleared 
        if (gl.current_level == 3) { 
            if (!slimeBossDefeated) { 
                // Don't transition - boss not defeated yet 
                return; 
            } 
            // Boss is defeated, but check if there are still slimes to clear 
            if (g.nslimes > 0) { 
                // Don't transition - still have slimes to clear 
                return; 
            } 
            // Boss defeated and all slimes cleared, now transition 
        } 

        // Clean up all slimes when transitioning to level 4 
        while (g.slimeHead) { 
            Slime *s = g.slimeHead; 
            g.slimeHead = g.slimeHead->next; 
            delete s; 
        } 
        g.nslimes = 0; 
        show_level_four(); 
    } 
} 

void change_grass_color() { 
    if (gl.current_level == 1) 
        glColor4f(0.0f, 0.5f, 1.0f, 1.0f); 
    else if (gl.current_level == 2) 
        glColor4f(0.9f, 0.8f, 1.0f, 1.0f); 
    else if (gl.current_level == 3) 
        glColor4f(1.0f, 1.0f, 0.7f, 1.0f); 
    else if (gl.current_level == 4) { 
        glColor4f(1.0f, 0.7f, 0.7f, 1.0f); 
    } else { 
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); 
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
    {100, 400, TOMBSTONE_NORMAL}, 
    {150, 400, TOMBSTONE_NORMAL}, 
    {50, 400, TOMBSTONE_NORMAL}, 
    {370, 400, TOMBSTONE_NORMAL}, 
    {500, 400, TOMBSTONE_CHUNKY}, 
    {640, 400, TOMBSTONE_CHUNKY}, 
    {500, 600, TOMBSTONE_CHUNKY}, 
    {640, 600, TOMBSTONE_CHUNKY}, 
    {375, 0, TOMBSTONE_THIN}, 
    {415, 0, TOMBSTONE_THIN}, 
    {455, 0, TOMBSTONE_THIN}, 
    {500, 0, TOMBSTONE_SHORTWIDE}, 
    {500, 200, TOMBSTONE_SHORTWIDE}, 
    {580, 200, TOMBSTONE_SHORTWIDE}, 
    {420, 200, TOMBSTONE_SHORTWIDE}, 
    {560, 0, TOMBSTONE_SHORTWIDE}, 
    {650, 0, TOMBSTONE_CHUNKY}, 
    {700, 200, TOMBSTONE_CHUNKY}, 
    {250, 200, TOMBSTONE_THIN}, 
    {300, 200, TOMBSTONE_THIN}, 
    {850, 200, TOMBSTONE_THIN}, 
    {900, 200, TOMBSTONE_THIN}, 
    {900, 500, TOMBSTONE_THIN}, 
    {800, 500, TOMBSTONE_THIN}, 
    {875, 400, TOMBSTONE_THIN}, 
    {825, 400, TOMBSTONE_THIN}, 
    {875, 600, TOMBSTONE_THIN}, 
    {825, 600, TOMBSTONE_THIN}, 
    {760, 0, TOMBSTONE_THIN}, 
    {790, 0, TOMBSTONE_THIN}, 
    {820, 0, TOMBSTONE_THIN}, 
    {850, 0, TOMBSTONE_NORMAL}, 
    {900, 0, TOMBSTONE_NORMAL}, 
    {0, 600, TOMBSTONE_SHORTWIDE}, 
    {60, 600, TOMBSTONE_SHORTWIDE}, 
    {120, 600, TOMBSTONE_SHORTWIDE}, 
    {180, 600, TOMBSTONE_SHORTWIDE}, 
    {0, 500, TOMBSTONE_SHORTWIDE}, 
    {60, 500, TOMBSTONE_SHORTWIDE}, 
    {120, 500, TOMBSTONE_SHORTWIDE}, 
    {180, 500, TOMBSTONE_SHORTWIDE}, 
}; 

Tree witch[] = { 
    {100, 0, WITCH_TREE}, 
    {100, 110, WITCH_TREE}, 
    {50, 400, WITCH_TREE}, 
    {150, 250, WITCH_TREE}, 
    {220, 600, WITCH_TREE}, 
    {300, 450, WITCH_TREE}, 
    {20, 650, WITCH_TREE}, 
    {100, 500, WITCH_TREE}, 
    {500, 0, WITCH_TREE}, 
    {350, 110, WITCH_TREE}, 
    {600, 310, WITCH_TREE}, 
    {450, 210, WITCH_TREE}, 
    {300, 300, WITCH_TREE}, 
    {520, 440, WITCH_TREE}, 
    {750, 200, WITCH_TREE}, 
    {600, 800, WITCH_TREE}, 
    {700, 800, WITCH_TREE}, 
    {650, 0, WITCH_TREE}, 
    {800, 550, WITCH_TREE}, 
    {900, 600, WITCH_TREE}, 
    {700, 500, WITCH_TREE}, 
    {800, 300, GRAVEYARD_TREE}, 
    {400, 500, GRAVEYARD_TREE}, 
    {600, 600, GRAVEYARD_TREE}, 
    {800, 0, GRAVEYARD_TREE}, 
}; 

int num_tombstones = sizeof(tombstones) / sizeof(Tombstone); 
int num_witch_items = sizeof(witch) / sizeof(Tree); 

void get_tombstone_size(TombstoneType type, float *width, float *height) { 
    switch (type) { 
        case TOMBSTONE_NORMAL: 
            *width = 40; 
            *height = 80; 
            // *radius = 
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

void get_tree_size(TreeType type, float *width, float *height) { 
    switch(type) { 
        case WITCH_TREE: 
            *width = 100; 
            *height = 100; 
            break; 
        case GRAVEYARD_TREE: 
            *width = 200; 
            *height = 200; 
            break; 
        default: 
            *width = 100; 
            *height = 100; 
            break; 
    } 
} 

void draw_tombstone(float x, float y, float width, float height) { 
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
    
    // Rounded top (semicircle) 
    // might swap this out for a rectangle 
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

void draw_trees(float x, float y, float width, float height) { 
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

void draw_witch_house(float x, float y, float width, float height) { 
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
    glVertex2f(x, y + height + height * 0.5f); // peak 
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
        // determines road size and length 
        float roadWidth = 20.0f; 
        float startY = y; 
        float segments = 20; 
        float curveAmplitude = 30.0f; 
        float segmentHeight = 40.0f; 
        
        glColor3f(0.2f, 0.2f, 0.2f); // dark grey 
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

void tombstone_physics_on_slimes(Slime *s) { 
    for (int i = 0; i < num_tombstones; i++) { 
        float w, h; 
        get_tombstone_size(tombstones[i].type, &w, &h); 
        Tombstone *tomb_pos = &tombstones[i]; 
        
        //option 2 
        float slime_left = s->pos[0] - s->radius / 2; 
        float slime_right = s->pos[0] + s->radius / 2; 
        float slime_top = s->pos[1] + s->radius / 2; 
        float slime_bottom = s->pos[1] - s->radius / 2; 
        
        float tomb_left = tomb_pos->x; 
        float tomb_right = tomb_pos->x + w; 
        float tomb_top = tomb_pos->y + h; 
        float tomb_bottom = tomb_pos->y; 
        
        // Checking collision 
        if (slime_right > tomb_left && slime_left < tomb_right && 
            slime_top > tomb_bottom && slime_bottom < tomb_top) { 
            
            // Find the overlap in both axes 
            float overlap_x = fmin(slime_right, tomb_right) - fmax(slime_left, tomb_left); 
            float overlap_y = fmin(slime_top, tomb_top) - fmax(slime_bottom, tomb_bottom); 
            
            // Resolve in the direction of least overlap 
            if (overlap_x < overlap_y) { 
                if (s->pos[0] < tomb_pos->x) { 
                    s->pos[0] -= overlap_x; 
                } else { 
                    s->pos[0] += overlap_x; 
                } 
                s->vel[0] = 0; 
                //s->pos[1] += 5; 
                //s->vel[1] += 5; 
            } else { 
                if (s->pos[1] < tomb_pos->y) { 
                    s->pos[1] -= overlap_y; 
                } else { 
                    s->pos[1] += overlap_y; 
                } 
                s->vel[1] = 0; 
                //s->vel[0] += 5; 
                //s->pos[0] += 5; 
            } 
        } 
    } 
} 

void tombstone_physics() { 
    //----------How Player reacts with Tombstones---------// 
    for (int i = 0; i < num_tombstones; i++) { 
        float w, h; 
        get_tombstone_size(tombstones[i].type, &w, &h); 
        Tombstone *tomb_pos = &tombstones[i]; 
        
        if (g.player.pos[0] + gl.player_size > tomb_pos->x && 
            g.player.pos[0] - gl.player_size < tomb_pos->x + w && 
            g.player.pos[1] + gl.player_size > tomb_pos->y && 
            g.player.pos[1] - gl.player_size < tomb_pos->y + h) { 
            
            // stop sliding 
            g.player.vel[0] = 0; 
            g.player.vel[1] = 0; 
            
            float d0 = g.player.pos[0] - tomb_pos->x; 
            float d1 = g.player.pos[1] - tomb_pos->y; 
            float distance_between_player_tomb = sqrt(d0 * d0 + d1 * d1); 
            
            if (distance_between_player_tomb > 0) { 
                // Left side of tombstone 
                if (g.player.pos[0] + gl.player_size > tomb_pos->x && 
                    g.player.pos[0] < tomb_pos->x) { 
                    g.player.pos[0] = tomb_pos->x - gl.player_size; 
                } 
                // Right side of tombstone 
                else if (g.player.pos[0] - gl.player_size < tomb_pos->x + w && 
                        g.player.pos[0] > tomb_pos->x + w) { 
                    g.player.pos[0] = tomb_pos->x + w + gl.player_size; 
                } 
                // Top of tombstone 
                else if (g.player.pos[1] + gl.player_size > tomb_pos->y && 
                        g.player.pos[1] < tomb_pos->y) { 
                    g.player.pos[1] = tomb_pos->y - gl.player_size; 
                } 
                // Bottom of tombstone 
                else if (g.player.pos[1] - gl.player_size < tomb_pos->y + h && 
                        g.player.pos[1] > tomb_pos->y + h) { 
                    g.player.pos[1] = tomb_pos->y + h + gl.player_size; 
                } 
            } 
        } 
        
        //------------- How Bullets React with Tombstones---------// 
        int j = 0; 
        while (j < g.nbullets) { 
            Bullet *b = &g.barr[j]; 
            
            // Collision detection 
            if (b->pos[0] > tomb_pos->x && b->pos[0] < tomb_pos->x + w && 
                b->pos[1] > tomb_pos->y && b->pos[1] < tomb_pos->y + h) { 
                // removing bullets 
                memcpy(&g.barr[j], &g.barr[g.nbullets-1], sizeof(Bullet)); 
                g.nbullets--; 
                continue; 
            } 
            ++j; 
        } 
    } 
} 

void witch_forest_physics() { 
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
        
        if (g.player.pos[0] + gl.player_size > tree_x && 
            g.player.pos[0] - gl.player_size < tree_x + trunk_width && 
            g.player.pos[1] + gl.player_size > tree_y && 
            g.player.pos[1] - gl.player_size < tree_y + trunk_height) { 
            
            // stop movement 
            g.player.vel[0] = 0; 
            g.player.vel[1] = 0; 
            
            float d0 = g.player.pos[0] - tree_pos->x; 
            float d1 = g.player.pos[1] - tree_pos->y; 
            float dist = sqrt(d0 * d0 + d1 * d1); 
            
            if (dist > 0) { 
                if (g.player.pos[0] + gl.player_size > tree_x && 
                    g.player.pos[0] < tree_x) { 
                    g.player.pos[0] = tree_x - gl.player_size; 
                } else if (g.player.pos[0] - gl.player_size < tree_x + trunk_width && 
                        g.player.pos[0] > tree_x + trunk_width) { 
                    g.player.pos[0] = tree_x + trunk_width + gl.player_size; 
                } else if (g.player.pos[1] + gl.player_size > tree_y && 
                        g.player.pos[1] < tree_y) { 
                    g.player.pos[1] = tree_y - gl.player_size; 
                } else if (g.player.pos[1] - gl.player_size < tree_y + trunk_height && 
                        g.player.pos[1] > tree_y + trunk_height) { 
                    g.player.pos[1] = tree_y + trunk_height + gl.player_size; 
                } 
            } 
        } 
        
        //-------------------------Leaves Collision------------------------// 
        float leaves_width = w * 1.2f; 
        float leaves_height = h * 0.5; 
        float leaves_x = tree_pos->x - leaves_width / 2; 
        float leaves_y = tree_pos->y + trunk_height; 
        
        if (g.player.pos[0] + gl.player_size > leaves_x && 
            g.player.pos[0] - gl.player_size < leaves_x + leaves_width && 
            g.player.pos[1] + gl.player_size > leaves_y && 
            g.player.pos[1] -gl.player_size < leaves_y + leaves_height) { 
            
            g.player.vel[0] *= 0.25f; 
            g.player.vel[1] *= 0.25f; 
        } 
        
        //--------------------------Bullet-Trunk Collision----------------// 
        int j = 0; 
        while (j < g.nbullets) { 
            Bullet *b = &g.barr[j]; 
            
            // Collision detection 
            if (b->pos[0] > tree_x && b->pos[0] < tree_x + trunk_width && 
                b->pos[1] > tree_y && b->pos[1] < tree_y + trunk_height) { 
                
                // now removing bullet 
                memcpy(&g.barr[j], &g.barr[g.nbullets-1], sizeof(Bullet)); 
                g.nbullets--; 
                
                if (j != g.nbullets) { 
                    memcpy(&g.barr[j], &g.barr[g.nbullets-1], sizeof(Bullet)); 
                } 
            } 
            ++j; 
        } 
    } 
} 

void witch_house_physics() { 
    float house_x = 310.0f - 50.0f / 2.0f; 
    float house_y = 400.0f; 
    float house_width = 50.0f; 
    float house_height = 50.0f; 
    
    if (g.player.pos[0] + gl.player_size > house_x && 
        g.player.pos[0] - gl.player_size < house_x + house_width && 
        g.player.pos[1] + gl.player_size > house_y && 
        g.player.pos[1] - gl.player_size < house_y + house_height) { 
        
        g.player.vel[0] = 0; 
        g.player.vel[1] = 0; 
        
        // dealing with collision 
        if (g.player.pos[0] < house_x) 
            g.player.pos[0] = house_x - gl.player_size; 
        else if (g.player.pos[0] > house_x + house_width) 
            g.player.pos[0] = house_x + house_width + gl.player_size; 
        
        if (g.player.pos[1] < house_y) 
            g.player.pos[1] = house_y - gl.player_size; 
        else if (g.player.pos[1] > house_y + house_height) 
            g.player.pos[1] = house_y + house_height + gl.player_size; 
    } 
} 

void rendering_background() { 
    if (gl.current_level == 1) { 
        for (int i = 0; i < num_tombstones; i++) { 
            float w, h; 
            get_tombstone_size(tombstones[i].type, &w, &h); 
            draw_tombstone(tombstones[i].x, tombstones[i].y, w, h); 
        } 
    } else if (gl.current_level == 2) { 
        for (int i = 0; i < num_witch_items; i++) { 
            float w, h; 
            get_tree_size(witch[i].type, &w, &h); 
            draw_trees(witch[i].x, witch[i].y, w, h); 
        } 
    } else if (gl.current_level == 3) { 
        draw_witch_house(500, 600, 50, 50); 
    } 
} 

//--------------------------Enemy physics--------------------// 
void hat_taking_damage() { 
    // accumilating damange max 1.0 
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

void move_hat() { 
    if (gl.current_level == 1) { 
        MakeVector(-150.0, 180.0, 0.0, hat.pos); 
        MakeVector(6.0, 15.0, 0.0, hat.vel); 
    } 
} 

void move_slimes() { 
    Slime * s = g.slimeHead; 
    float speed = 0.5f; //determines if stationary or in movement 
    
    while (s) { 
        if (gl.game_paused) { 
            s->vel[0] = 0; 
            s->vel[1] = 0; 
            s->pos[0] = s->pos[0]; 
            s->pos[1] = s->pos[1]; 
            s = s->next; 
            speed = 0; 
        } else { 
            float dx = gl.player_center_x - s->pos[0]; 
            float dy = gl.player_center_y - s->pos[1]; 
            float dist = sqrt(dx * dx + dy * dy); 
            
            if (dist > 0.1f) { 
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
            s = s->next; 
        } 
    } 
} 

void checking_invincible_timer() { 
    if (gl.player_invincible) { 
        gl.invincible_timer--; 
        if (gl.invincible_timer <= 0) { 
            gl.player_invincible = 0; 
            gl.invincible_timer = 0; 
        } 
    } 
}

