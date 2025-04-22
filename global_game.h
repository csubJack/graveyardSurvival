#ifndef GLOBAL_GAME_H
#define GLOBAL_GAME_H


#include <iostream>
#include <cmath>
#include <ctime>
#include <GL/gl.h>
#include <cstring>

#include "types.h"


// #define MAX_BULLETS 100
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define PI 3.14159265358979323846

// typedef float Vec[3];
// typedef double Flt;

class Global {
    public:
        int xres, yres;
        char keys[65536];
        int mouse_cursor_on;
        int credits;
        int title_screen;
        int game_over_screen;
        int title_hat;
        int main_hat;
        int character_screen;
        // Player metrics 
        int player_score;
        int bullets_shot;
        int monsters_killed;
        int player_size;
        int player_invincible;
        int invincible_timer;
        float bullet_accuracy;
        int current_level;
        // end Player Metrics 
        
        int grass;
        int hat_silhouette;
        bool game_started;
        bool game_paused;
        bool pause_key_pressed;
        GLuint witch_texture;
        GLuint hat_texture;
        GLuint hat_silhouette_texture;
        GLuint grass_texture;
        GLuint hound_texture;
        GLuint hound_trans_texture;
        float mouseXCoordinate;
        float mouseYCoordinate;
        int player_health;
        float player_center_x;
        float player_center_y;
    
        Global() {
            xres = 640;
            yres = 480;
            memset(keys, 0, 65536);
            mouse_cursor_on = 1;
            credits = 0;
            title_screen = 1;
            game_over_screen = 0;
            character_screen = 0;
            current_level = 1;
            game_started = false;
            game_paused = false;
            pause_key_pressed = false;
            grass = 1;
            //-----------------------
            //player stats
            player_size = 10.0;
            player_health = 100;
            player_invincible = 0;
            hat_silhouette = 0;
            player_center_x = 0.0f;
            player_center_y = 0.0f;
        }
    };
    
    class Ship {
    public:
        Vec pos;
        Vec dir;
        Vec vel;
        Vec acc;
        float angle;
        float color[3];
    public:
        Ship() {
            pos[0] = (Flt)(640/2); //xres 
            pos[1] = (Flt)(480/2); // yres
            pos[2] = 0.0f;
            VecZero(dir);
            VecZero(vel);
            VecZero(acc);
            angle = 0.0;
            color[0] = color[1] = color[2] = 1.0;
        }
    };
    
    class Bullet {
    public:
        Vec pos;
        Vec vel;
        float color[3];
        struct timespec time;
    public:
        Bullet() { }
    };
    
    
    class Hat {
    public:
        Vec pos;
        Vec vel;
        float damage = 0.0f;
        bool being_hit = false;
    };
    class Hound {
    public:
        Vec pos;
        Vec vel;
    };
    
    class Slime {
    public:
        Vec pos;
        Vec vel;
        int nverts;
        Flt radius;
        Vec vert[8];
        float angle;
        float rotate;
        float color[3];
        struct Slime *prev;
        struct Slime *next;
        bool isParent;
    public:
        Slime() {
            prev = NULL;
            next = NULL;
            isParent = true;
        }
    };
    class MedKit {
    public:
        Vec pos;           // Position
        float radius;      // Collision radius
        bool active;       // Whether the med kit is currently active
        float glowIntensity; // For the glow animation (0.0-1.0)
        float glowDirection;  // Whether the glow is increasing or decreasing
    public:
        MedKit() {
            active = false;
            radius = 15.0f;
            glowIntensity = 0.5f;
            glowDirection = 1.0f;
        }
    };
    //---------------------------------------------
    //background materials
    typedef enum {
        TOMBSTONE_NORMAL,
        TOMBSTONE_CHUNKY,
        TOMBSTONE_THIN,
        TOMBSTONE_SHORTWIDE
    } TombstoneType;
    typedef struct {
        /// maybe add radius
        float x, y;
        TombstoneType type;
    } Tombstone;
    // other descriptive terms for these trees, gnarly tree, dead tree, apple tree
    typedef enum {
        WITCH_TREE,
        GRAVEYARD_TREE,
        LEVEL_THREE_TREE
    } TreeType;
    typedef struct {
        float x, y;
        TreeType type;
    }Tree;
    //---------------------------------------------
     // Will use for skeleton enemy --  Julio 
    /*class Skeleton {
        public:
            Vec pos;
            Vec vel;
        public:
            Skeleton() {
                
            }
    };*/
    
    class Game {
    public:
        Ship ship;
      //  Witch witch;
        Bullet *barr;
        int nbullets;
        Slime *slimeHead; //new
        int nslimes; //new
        MedKit medkit;
        // struct timespec bulletTimer;
        struct timespec mouseThrustTimer;
        bool mouseThrustOn;
    public:
        Game() {
            barr = new Bullet[22];
            nbullets = 0;
            slimeHead = NULL;
            nslimes = 0;
        }
        ~Game() {
            delete [] barr;
            // Clean up slimes
            Slime *s = slimeHead;
            while (s) {
                Slime *next = s->next;
                delete s;
                s = next;
            }
        }
    };

    

    extern Global gl;
    extern Game g;
    extern Hat hat;
    extern Hound hound;

#endif

