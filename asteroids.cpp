
// Group Header Files: 
#include "jsweeny.h"
#include "mgarcia6.h"
#include "types.h"
#include "global_game.h"
#include "jcornejo.h"
#include "djimenezgarc.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>

#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"

// adding in a header for types so we can all
// use them instead of redefining them
#include "types.h"

//defined types, moved to header file
//typedef float Flt;
//typedef float Vec[3];
//typedef Flt	Matrix[4][4];

//macros
#define rnd() (((Flt)rand())/(Flt)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
						(c)[1]=(a)[1]-(b)[1]; \
						(c)[2]=(a)[2]-(b)[2]
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
// #define PI 3.141592653589793
#define ALPHA 1
const int MAX_BULLETS = 22; // Doubled due to 2 pistols
int lastShot = 0; // right gun is zero

//-----------------------------------------------------------------------------
//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern struct timespec timePause;
extern double physicsCountdown;
extern double timeSpan;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);

//-----------------------------------------------------------------------------
// importing images class
class Image {
public:
    int width, height;
    unsigned char *data;
    ~Image() { delete [] data; }
    Image(const char *fname) {
        if (fname[0] == '\0')
            return;
        //printf("fname **%s**\n", fname);
        int ppmFlag = 0;
        char name[40];
        strcpy(name, fname);
        int slen = strlen(name);
        char ppmname[80];
        if (strncmp(name+(slen-4), ".ppm", 4) == 0)
            ppmFlag = 1;
        if (ppmFlag) {
            strcpy(ppmname, name);
        } else {
            name[slen-4] = '\0';
            //printf("name **%s**\n", name);
            sprintf(ppmname,"%s.ppm", name);
            //printf("ppmname **%s**\n", ppmname);
            char ts[100];
            //system("convert eball.jpg eball.ppm");
            sprintf(ts, "convert %s %s", fname, ppmname);
            system(ts);
        }
        //sprintf(ts, "%s", name);
        FILE *fpi = fopen(ppmname, "r");
        if (fpi) {
            char line[200];
            fgets(line, 200, fpi);
            fgets(line, 200, fpi);
            //skip comments and blank lines
            while (line[0] == '#' || strlen(line) < 2)
                fgets(line, 200, fpi);
            sscanf(line, "%i %i", &width, &height);
            fgets(line, 200, fpi);
            //get pixel data
            int n = width * height * 3;
            data = new unsigned char[n];
            for (int i=0; i<n; i++)
                data[i] = fgetc(fpi);
            fclose(fpi);
        } else {
            printf("ERROR opening image: %s\n",ppmname);
            exit(0);
        }
        if (!ppmFlag)
            unlink(ppmname);
    }
};

Image img[6] = {
"./images/hat.png",
"./images/standing-still2.png",
"./images/houndDog2.png",
"./images/pixel_skeleton.png",
"./images/grass.png",
"./images/WitchF.png"
};
// witch


//------------------------------------------------------------------------

Global gl;
Game g;
Hat hat; 
Hound hound;
Witch wich;

//================Slime Enemy==============================
// deleting slime from the linked list
int previous_level = -1;

bool zombie_wave_spawned = false;
bool second_wave_spawned = false;

#define MAX_ZOMBIES 10

struct Zombie {
    Vec pos;
    Vec vel;
    float speed;
    bool active;
};

Zombie zombies[MAX_ZOMBIES];



void deleteSlime(Slime *node)
{
    // remove node from doubly-linked list
    if (node->prev == NULL) {
        if (node->next == NULL) {
            // one item in list.
            g.slimeHead = NULL;
        } else {
            // start of list.
            node->next->prev = NULL;
            g.slimeHead = node->next;
        }
    } else {
        if (node->next == NULL) {
            // end of list.
            node->prev->next = NULL;
        } else {
            // middle of list.
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
    }
    delete node;
    node = NULL;
}

void buildSlimeFragment(Slime *ts, Slime *s)
{
    // Build ts from s
    ts->nverts = 8;
    ts->radius = s->radius / 1.5; // size
    Flt r2 = ts->radius / 2.0;
    Flt angle = 0.0f;
    Flt inc = (PI * 2.0) / (Flt)ts->nverts;
    for (int i=0; i<ts->nverts; i++) {
        // makes slimes round
        ts->vert[i][0] = sin(angle) * (r2 + rnd() * ts->radius * 0.3);
        ts->vert[i][1] = cos(angle) * (r2 + rnd() * ts->radius * 0.3);
        angle += inc;
    }
    
    // positioning
    ts->pos[0] = s->pos[0] + rnd()*10.0-5.0;
    ts->pos[1] = s->pos[1] + rnd()*10.0-5.0;
    ts->pos[2] = 0.0f;
    ts->angle = 0.0;
    ts->rotate = s->rotate + (rnd() * 2.0 - 1.0); // rotating
    
    // color
    ts->color[0] = 0.0; // r
    ts->color[1] = 0.7 + rnd()*0.3; // g
    ts->color[2] = 0.0; // b
    
    // speed
    ts->vel[0] = s->vel[0] + (rnd()*1.0-0.5);  // Was 2.0-1.0
    ts->vel[1] = s->vel[1] + (rnd()*1.0-0.5);
}


int count_slimes() {
    int count = 0;
    Slime *s = g.slimeHead;
    while (s) {
        count++;
        s = s->next;
    }
    return count;
}


void initSlimes() 
{
    if (gl.current_level == 1 || gl.current_level == 3) {
        for (int i = 0; i < 1; i++) { // changed to zerof to remove slimes for movement testing
            Slime *s = new Slime;
            s->nverts = 8;
            s->radius = 30.0 + rnd() * 40.0; // size
        
            // makes slimes round
            Flt r2 = s->radius / 2.0;
            Flt angle = 0.0f;
            Flt inc = (PI * 2.0) / (Flt)s->nverts;
            for (int j=0; j<s->nverts; j++) {
                s->vert[j][0] = sin(angle) * (r2 + rnd() * s->radius * 0.3);
                s->vert[j][1] = cos(angle) * (r2 + rnd() * s->radius * 0.3);
                angle += inc;
            }
        
            // positioning
            float dist = rnd() * 200.0 + 200.0;
            float ang = rnd() * PI * 2.0;
            s->pos[0] = g.ship.pos[0] + cos(ang) * dist;
            s->pos[1] = g.ship.pos[1] + sin(ang) * dist;
            s->pos[2] = 0.0f;
        
            s->angle = 0.0;
            s->rotate = rnd() * 2.0 - 1.0; // rotating
        
            s->color[0] = 0.0;
            s->color[1] = 0.7 + rnd()*0.3;
            s->color[2] = 0.0;
        
            // movement
            s->vel[0] = rnd()*0.4-0.2;  // Was 0.8-0.4
            s->vel[1] = rnd()*0.4-0.2; 
        
            // add to front of slime linked list
            s->next = g.slimeHead;
            if (g.slimeHead != NULL)
                g.slimeHead->prev = s;
            g.slimeHead = s;
            g.nslimes++;
        }
    }
}
//=========================================================
unsigned char *buildAlphaData(Image *img);

GLuint playerTexture;
// Jack
void loadPlayerIcon() {


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &playerTexture);

	glEnable(GL_TEXTURE_2D);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glBindTexture(GL_TEXTURE_2D, playerTexture);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    unsigned char* rgbaData = buildAlphaData(&img[1]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,img[1].width, img[1].height, 0,GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);	
    
	
}
// Jack
void renderPLayerIcon(float playerX, float playerY, float playerSize, float playerAngle) {
    gl.player_center_x = playerX;
    gl.player_center_y = playerY;
    
    glPushMatrix();
	// Fixed the Red issue
	glColor3f(1.0, 1.0, 1.0);
	
	// move player
	glTranslatef(playerX, playerY, 0.0f);

	glRotatef(playerAngle, 0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, playerTexture);

    glBindTexture(GL_TEXTURE_2D, gl.player_sil_texture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);


	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(-playerSize / 2,-playerSize / 2);
	    glTexCoord2f(0.0f, 0.0f); glVertex2i(-playerSize / 2,playerSize / 2);
	    glTexCoord2f(1.0f, 0.0f); glVertex2i( playerSize / 2,playerSize / 2);
	    glTexCoord2f(1.0f, 1.0f); glVertex2i( playerSize / 2,-playerSize / 2);
	glEnd();

	glPopMatrix();    
    glDisable(GL_BLEND);
    
}

//X Windows variables
class X11_wrapper {
private:
	Display *dpy;
	Window win;
	GLXContext glc;
public:
	X11_wrapper() { }
	X11_wrapper(int w, int h) {
		GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
		//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
		XSetWindowAttributes swa;
		setup_screen_res(gl.xres, gl.yres);
		dpy = XOpenDisplay(NULL);
		if (dpy == NULL) {
			std::cout << "\n\tcannot connect to X server" << std::endl;
			exit(EXIT_FAILURE);
		}
		Window root = DefaultRootWindow(dpy);
		XWindowAttributes getWinAttr;
		XGetWindowAttributes(dpy, root, &getWinAttr);
		int fullscreen = 0;
		gl.xres = w;
		gl.yres = h;
		if (!w && !h) {
			//Go to fullscreen.
			gl.xres = getWinAttr.width;
			gl.yres = getWinAttr.height;
			//When window is fullscreen, there is no client window
			//so keystrokes are linked to the root window.
			XGrabKeyboard(dpy, root, False,
				GrabModeAsync, GrabModeAsync, CurrentTime);
			fullscreen=1;
		}
		XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
		if (vi == NULL) {
			std::cout << "\n\tno appropriate visual found\n" << std::endl;
			exit(EXIT_FAILURE);
		} 
		Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
		swa.colormap = cmap;
		swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
			PointerMotionMask | MotionNotify | ButtonPress | ButtonRelease |
			StructureNotifyMask | SubstructureNotifyMask;
		unsigned int winops = CWBorderPixel|CWColormap|CWEventMask;
		if (fullscreen) {
			winops |= CWOverrideRedirect;
			swa.override_redirect = True;
		}
		win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
			vi->depth, InputOutput, vi->visual, winops, &swa);
		//win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
		//vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
		set_title();
		glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
		glXMakeCurrent(dpy, win, glc);
		show_mouse_cursor(0);
	}
	~X11_wrapper() {
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}
	void set_title() {
		//Set the window title bar.
		XMapWindow(dpy, win);
		XStoreName(dpy, win, "GraveyardSurvival");
	}
	void check_resize(XEvent *e) {
		//The ConfigureNotify is sent by the
		//server if the window is resized.
		if (e->type != ConfigureNotify)
			return;
		XConfigureEvent xce = e->xconfigure;
		if (xce.width != gl.xres || xce.height != gl.yres) {
			//Window size did change.
			reshape_window(xce.width, xce.height);
		}
	}
	void reshape_window(int width, int height) {
		//window has been resized.
		setup_screen_res(width, height);
		glViewport(0, 0, (GLint)width, (GLint)height);
		glMatrixMode(GL_PROJECTION); glLoadIdentity();
		glMatrixMode(GL_MODELVIEW); glLoadIdentity();
		glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
		set_title();
	}
	void setup_screen_res(const int w, const int h) {
		gl.xres = w;
		gl.yres = h;
	}
	void swapBuffers() {
		glXSwapBuffers(dpy, win);
	}
	bool getXPending() {
		return XPending(dpy);
	}
	XEvent getXNextEvent() {
		XEvent e;
		XNextEvent(dpy, &e);
		return e;
	}
	void set_mouse_position(int x, int y) {
		XWarpPointer(dpy, None, win, 0, 0, 0, 0, x, y);
	}
	void show_mouse_cursor(const int onoff) {
		printf("show_mouse_cursor(%i)\n", onoff); fflush(stdout);
		if (onoff) {
			//this removes our own blank cursor.
			XUndefineCursor(dpy, win);
			return;
		}
		//vars to make blank cursor
		Pixmap blank;
		XColor dummy;
		char data[1] = {0};
		Cursor cursor;
		//make a blank cursor
		blank = XCreateBitmapFromData (dpy, win, data, 1, 1);
		if (blank == None)
			std::cout << "error: out of memory." << std::endl;
		cursor = XCreatePixmapCursor(dpy, blank, blank, &dummy, &dummy, 0, 0);
		XFreePixmap(dpy, blank);
		//this makes the cursor. then set it using this function
		XDefineCursor(dpy, win, cursor);
		//after you do not need the cursor anymore use this function.
		//it will undo the last change done by XDefineCursor
		//(thus do only use ONCE XDefineCursor and then XUndefineCursor):
	}
} x11(gl.xres, gl.yres);

//function prototypes
void init_opengl(void);
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void physics();
void render();
void normalize2d(Vec v);
int count_slimes();
void title_render();
void game_over(Rect *r, float xres, float yres);
void render_pause_screen(Rect *r);
extern void show_score();
void level_two_render();
extern void checking_level_transition();
// removed from now it 
//void init();
extern void move_hat(); 
extern void move_slimes();
extern void checking_invincible_timer();
extern void rendering_background();
void create_default_scoreboard();
void render_leaderboard_page();
bool verify_high_score();
void render_medkit();

void render_nuke();
void generate_nuke_drop(float size); 
void nuke_collision();
//extern void show_hound();
//void show_hound();
// animation rest
void reset_game_animation();
void reset_title_animation();

void spawnZombie() {
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (!zombies[i].active) {
            zombies[i].active = true;

            int side = rand() % 4;
            float x, y;
            
            if (side == 0) { // Left
             x = -50; // Offscreen left
             y = rand() % (gl.yres + 1);
            } else if (side == 1) { // Right
             x = gl.xres + 50; // Offscreen right
             y = rand() % (gl.yres + 1);
            } else if (side == 2) { // Bottom
             x = rand() % (gl.xres + 1);
             y = -50; // Offscreen bottom
            } else { // Top
             x = rand() % (gl.xres + 1);
             y = gl.yres + 50; // Offscreen top
            }


            
            MakeVector(x, y, 0.0, zombies[i].pos);

            Vec dir;
            VecSub(g.ship.pos, zombies[i].pos, dir);
            normalize2d(dir);
            MakeVector(dir[0] * 2.0, dir[1] * 2.0, 0.0, zombies[i].vel);
            break;
        }
    }
}


//==========================================================================
// M A I N
//==========================================================================
int main() {
    logOpen();
    init_opengl();

    for (int i = 0; i < MAX_ZOMBIES; i++)
        zombies[i].active = false;

    for (int i = 0; i < 3; i++) {
        spawnZombie();
    }


	logOpen();
	init_opengl();
    //checking_invincible_timer();
    gl.title_hat = 1;
    gl.main_hat = 0;
    move_hat();
    create_default_scoreboard();
    //gl.moon_shine_timer +=1;
    //show_hound();
    //init(); show hat should replace this
	srand(time(NULL));
	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	x11.set_mouse_position(200, 200);
	x11.show_mouse_cursor(gl.mouse_cursor_on);
	int done=0;
    // adding in
    bool previous_game_state = gl.game_started;
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			check_mouse(&e);
			done = check_keys(&e);
		}

        

		clock_gettime(CLOCK_REALTIME, &timeCurrent);
		timeSpan = timeDiff(&timeStart, &timeCurrent);
		timeCopy(&timeStart, &timeCurrent);
		physicsCountdown += timeSpan;
		while (physicsCountdown >= physicsRate) {
			physics();
			physicsCountdown -= physicsRate;
		}
        if (gl.game_started != previous_game_state) {
            if (gl.game_started) {
                reset_game_animation();
                reset_game_stats(); // update function in jsweeny.cpp as needed
            } else if (gl.game_over_screen) {
                reset_game_animation();
                reset_title_animation();
                if (gl.player_score > gl.top_scores.back().first) {
                    cout << "high score" << endl;
                }
            }else {
                reset_title_animation();
                //reset_character_screen_animation();
                // next would be reset_character_screen
            }
            previous_game_state = gl.game_started;
        }
        if (gl.game_started) {

            checking_invincible_timer();
            //rendering_background();
            generate_nuke_drop(10.0);
            nuke_collision();
            render();
            show_score();
            //move_slimes();
            checking_level_transition();
        }
        if (gl.player_health <= 0 && gl.game_started) {
            Rect r; 
            gl.game_over_screen = 1;
            integrateBossSystem();
            gl.current_level = 1;
    // -- RESET ZOMBIES ON LEVEL TRANSITION --
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        zombies[i].active = false;
    }   

if (gl.current_level == 1) {
    for (int i = 0; i < 3; i++) {
        spawnZombie();
    }
    zombie_wave_spawned = true;
    second_wave_spawned = false;
}
else if (gl.current_level == 4) {
    for (int i = 0; i < 5; i++) {
        spawnZombie();
    }
    zombie_wave_spawned = false;
    second_wave_spawned = false;
}
else {
    zombie_wave_spawned = false;
    second_wave_spawned = false;
}


        game_over(&r, gl.xres, gl.yres);
        }
        // used to be !gl.game_started
		if (gl.title_screen) {
            title_render();
           
            //previous_game_state = gl.title_screen;
        }
        if (gl.leaderboard_screen) {
            Rect r;
            render_leaderboard_page(&r);
        }
        if (gl.character_screen) {
            show_score();
            //level_two_render();
        //    previous_game_state = gl.character_screen;
        }
        //render();
		x11.swapBuffers();
	}
	cleanup_fonts();
	logClose();
	return 0;
}


//------------------------------------------------
//from rainforest framework
unsigned char *buildAlphaData(Image *img)
{
    //Add 4th component to an RGB stream...
    //RGBA
    //When you do this, OpenGL is able to use the A component to determine
    //transparency information.
    //It is used in this application to erase parts of a texture-map from view.
    int i;
    int a,b,c;
    unsigned char *newdata, *ptr;
    unsigned char *data = (unsigned char *)img->data;
    newdata = (unsigned char *)malloc(img->width * img->height * 4);
    ptr = newdata;
    for (i=0; i<img->width * img->height * 3; i+=3) {
        a = *(data+0);
        b = *(data+1);
        c = *(data+2);
        *(ptr+0) = a;
        *(ptr+1) = b;
        *(ptr+2) = c;
        //-----------------------------------------------
        //get largest color component...
        //*(ptr+3) = (unsigned char)((
        //      (int)*(ptr+0) +
        //      (int)*(ptr+1) +
        //      (int)*(ptr+2)) / 3);
        //d = a;
        //if (b >= a && b >= c) d = b;
        //if (c >= a && c >= b) d = c;
        //*(ptr+3) = d;
        //-----------------------------------------------
        //this code optimizes the commented code above.
        //code contributed by student: Chris Smith
        //
        *(ptr+3) = (a|b|c);
        //-----------------------------------------------
        ptr += 4;
        data += 3;
    }
    return newdata;
}
//-----------------------------------------------------------------

// resseting variables
void reset_game_animation() {
    g.ship.pos[0] = gl.xres / 2;
    g.ship.pos[1] = gl.yres / 2;
    g.ship.vel[0] = 0.0f;
    g.ship.vel[1] = 0.0f;
    g.nbullets = 0; // clears bullet
    gl.regular_timer = 0;
    //gl.current_level = 1;
    // show_hat();
    while (g.slimeHead) {
        Slime *s = g.slimeHead;
        g.slimeHead = g.slimeHead->next;
        delete s;
    }
    g.nslimes = 0;
    initSlimes();
    g.nslimes = count_slimes();


    for (int i = 0; i < MAX_ZOMBIES; i++) {
    zombies[i].active = false;
}

if (gl.current_level == 1 || gl.current_level == 4) {
    for (int i = 0; i < 3; i++) {
        spawnZombie();
    }
    zombie_wave_spawned = true;
    second_wave_spawned = false;
} else {
    zombie_wave_spawned = false;
    second_wave_spawned = false;
}


    integrateBossSystem();
    move_hat();
    hat.damage = 0; 
    
}


void reset_title_animation() {
    // resetting the hat
    //init();
    move_hat();
}



void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, gl.xres, gl.yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
	//
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	//
	//Clear the screen to black, switching this to forest framework rec
    glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();

    //------------------------------------
	//Loading Player Icon

	
    glGenTextures(1, &gl.hat_texture);
    glGenTextures(1, &gl.hat_silhouette_texture);
    glGenTextures(1, &gl.hound_texture);
    glGenTextures(1, &gl.witch_texture);
    glGenTextures(1, &gl.player_texture);
    glGenTextures(1, &gl.player_sil_texture);
	loadPlayerIcon();

    //----------------------------------------------
    // moving this down for testing
    int w = img[0].width;
    int h = img[0].height;
    //-------------------------------------
    //for out hat
    w = img[0].width;
    h = img[0].height;
    glBindTexture(GL_TEXTURE_2D, gl.hat_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
        GL_RGB, GL_UNSIGNED_BYTE, img[0].data);
    //---------------------------------------------------------------
    // silhouette
    glBindTexture(GL_TEXTURE_2D, gl.hat_silhouette_texture);
	//
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//
	//must build a new set of data...
	unsigned char *silhouetteData = buildAlphaData(&img[0]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
								GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
	free(silhouetteData);
    //---------------------------------------------------------------
    w = img[1].width;
    h = img[1].height;
    glBindTexture(GL_TEXTURE_2D, gl.hat_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
        GL_RGB, GL_UNSIGNED_BYTE, img[1].data);
    //---------------------------------------------------------------
    // silhouette
    glBindTexture(GL_TEXTURE_2D, gl.player_sil_texture);
	//
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//
	//must build a new set of data...
	unsigned char *playerSilData = buildAlphaData(&img[1]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
								GL_RGBA, GL_UNSIGNED_BYTE,playerSilData);
	free(playerSilData);
    //---------------------------------------------------------------
    // for the hound dog
    w = img[2].width;
    h = img[2].height;
    glBindTexture(GL_TEXTURE_2D, gl.hound_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
        GL_RGB, GL_UNSIGNED_BYTE, img[2].data);


    //----------------------------------------------------------------------
    // grass
    glBindTexture(GL_TEXTURE_2D, gl.grass_texture);
   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, img[4].width, img[4].height,
                                    0, GL_RGB, GL_UNSIGNED_BYTE, img[4].data);

    
    w = img[5].width;
    h = img[5].height;
    glBindTexture(GL_TEXTURE_2D, gl.witch_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, img[5].width, img[5].height,
                0, GL_RGB, GL_UNSIGNED_BYTE, img[5].data);
                //------------------
    //trying to implement the transparency
//    glBindTexture(GL_TEXTURE_2D, gl.hound_trans_texture);
	//
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//
	//must build a new set of data...
//	w = img[2].width;
//	h = img[2].height;
//	unsigned char *ftData = buildAlphaData(&img[2]);	
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
//											GL_RGBA, GL_UNSIGNED_BYTE, ftData);
//	free(ftData);
    integrateBossSystem();
}
// only for witch lone movement
//void init() 
//{
    //makes it move around, for now we just want to see it
    //modifying the numbers for vel
    //6 0 0 0, to 6 15 0
//    MakeVector(-150.0,180.0,0.0, hat.pos);
//    MakeVector(6.0,15.0,0.0, hat.vel);
//}

//void show_hound()
//{
//    MakeVector(-150.0, 180.0, 0.0, hound.pos);
//    MakeVector(6.0, 15.0, 0.0, hound.vel);
//}
//
void normalize2d(Vec v)
{
	Flt len = v[0]*v[0] + v[1]*v[1];
	if (len == 0.0f) {
		v[0] = 1.0;
		v[1] = 0.0;
		return;
	}
	len = 1.0f / sqrt(len);
	v[0] *= len;
	v[1] *= len;
}

void check_mouse(XEvent *e)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	static int ct=0;
	//std::cout << "m" << std::endl << std::flush;

	// Fires if mouse moves, keeps player looking away from mouse
	// TODO: ADD EXTRA MODE SO PLAYER IS FACING MOUSE
	if (e->type == MotionNotify) {
		// g.ship.angle = update_player_angle(e->xmotion.x,
        //         g.ship.pos[0], e->xmotion.y, g.ship.pos[1]);
	}

	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
				if (g.nbullets < MAX_BULLETS) {
					handle_shot(lastShot);	
				}
		}
	}
	//keys[XK_Up] = 0;
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		// the mouse moved
		int xdiff = savex - e->xbutton.x;
		int ydiff = savey - e->xbutton.y;
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		if (++ct < 10)
			return;		
		if (gl.mouse_cursor_on)
			return;
		//printf("mouse move "); fflush(stdout);
		if (xdiff > 0) {
			//std::cout << "xdiff: " << xdiff << std::endl << std::flush;
			g.ship.angle += 0.05f * (float)xdiff;
			if (g.ship.angle >= 360.0f)
				g.ship.angle -= 360.0f;
		}
		else if (xdiff < 0) {
			//std::cout << "xdiff: " << xdiff << std::endl << std::flush;
			g.ship.angle += 0.05f * (float)xdiff;
			if (g.ship.angle < 0.0f)
				g.ship.angle += 360.0f;
		}
		if (ydiff > 0) {
			//apply thrust
			//convert ship angle to radians
			Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
			//convert angle to a vector
			Flt xdir = cos(rad);
			Flt ydir = sin(rad);
			g.ship.pos[0] += xdir * 2.0f;
			g.ship.pos[1] += ydir * 2.0f;
			// g.mouseThrustOn = true;
			clock_gettime(CLOCK_REALTIME, &g.mouseThrustTimer);
		}
		x11.set_mouse_position(200,200);
		savex = 200;
		savey = 200;
	}
}

int check_keys(XEvent *e)
{
	static int shift=0;
	if (e->type != KeyRelease && e->type != KeyPress) {
		//not a keyboard event
		return 0;
	}
	int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
	//Log("key: %i\n", key);
	if (e->type == KeyRelease) {
		gl.keys[key] = 0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift = 0;
		return 0;
	}
	if (e->type == KeyPress) {
		//std::cout << "press" << std::endl;
		gl.keys[key]=1;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			shift = 1;
			return 0;
		}
	}
	(void)shift;
	switch (key) {
		case XK_Escape:
			return 1;
        case XK_space:  // removing for the moment && !gl.character_screen
            if (!gl.game_started && !gl.leaderboard_screen) {
                gl.title_screen = !gl.title_screen;
                gl.game_started = true;
                //gl.show_witch = true;
                //if (gl.title_witch) {
                //    witch.pos[0] = -250;
               // }
                break;
            }

            if (gl.game_over_screen) {
                gl.title_screen = 1; // space takes you back to title screen
                gl.game_over_screen = 0;
                gl.game_started = false; // game is over
            }
           // gl.title_screen = !gl.title_screen;
            break;
		case XK_m:
			gl.mouse_cursor_on = !gl.mouse_cursor_on;
			x11.show_mouse_cursor(gl.mouse_cursor_on);
			break;
        case XK_c:
            // credits
            if (!gl.credits) {
                gl.credits = 1;
            } else {
                gl.credits = 0;
            }
           // safest way
           //gl.credits = !gl.credits;
           break;
        case XK_t:
           if(gl.game_started) {
               gl.game_started = !gl.game_started;
               gl.title_screen = !gl.title_screen;
               gl.current_level = 1;
           }
           break;
        case XK_k:
           if (gl.title_screen) {
               gl.title_screen = !gl.title_screen;
               gl.character_screen = !gl.character_screen;
           } else if (gl.character_screen) {
               gl.character_screen = !gl.character_screen;
               gl.game_started = !gl.game_started;
           } else if (gl.game_started) {
               gl.character_screen = !gl.character_screen;
               gl.game_started = !gl.game_started;
           }
           break;
        case XK_u:
            if (!gl.game_started && !gl.game_paused) {
                gl.title_screen = !gl.title_screen;
                gl.leaderboard_screen = !gl.leaderboard_screen;
            }

           break;
        case XK_1:
            gl.player_score += 1000;
            break;
        case XK_2:
            gl.grass ^= 1;
		    break;
        case XK_Down:
			break;
		case XK_equal:
			break;
		case XK_minus:
			break;
	}
	return 0;
}
// keeping this for main screen
void extern hat_taking_damage();
void physics_hat()
{
    //move hat
    int addgrav = 1;
    static float speed = 5.0f;
    static float hat_active = 0;
    //Update position
    if (!gl.game_paused) {
        hat.pos[0] += hat.vel[0];
        hat.pos[1] += hat.vel[1];
    }
    if (gl.current_level == 1) {
        //Check for collision with window edges
        if ((hat.pos[0] < -140.0 && hat.vel[0] < 0.0) ||
            (hat.pos[0] >= (float)gl.xres+140.0 &&
            hat.vel[0] > 0.0))
        {   
            hat.vel[0] = -hat.vel[0];
            addgrav = 0;
        }
        //  changing 150 to 0
        if ((hat.pos[1] < 100.0 && hat.vel[1] < 0.0) ||
            (hat.pos[1] >= (float)gl.yres && hat.vel[1] > 0.0)) {
            hat.vel[1] = -hat.vel[1];
            addgrav = 0;
        }
        //Gravity?
        if (addgrav)
            hat.vel[1] -= 0.75;
    }
    //if (gl.current_level == 2) {
    if (gl.current_level == 2 && (!gl.game_paused)) {
        //MakeVector(-150.0, 180.0, 0.0, hat.pos);
        if (!hat_active ||
        hat.pos[0] < -100 || hat.pos[0] > gl.xres + 100 ||
        hat.pos[1] < -100 || hat.pos[1] > gl.yres + 100) {
            // New position pick a corner
            int corner = rand() % 4;
            float X, Y;
            switch (corner) {
                case 0: // top left
                    X = -100.0f;
                    Y = gl.yres + 100.0f;
                    break;
                case 1: // top right
                    X = gl.xres + 100.0f;
                    Y = gl.yres + 100.0f;
                    break;
                case 2: // bottom left
                    X = -100.0f;
                    Y = 100.0f;
                    break;
                case 3: // bottom right
                    X = gl.xres + 100.0f;
                    Y = -100.0f;
                    break;
            }
            // this was a bit unfair for the players
            //float X = -100 + rand() % (gl.xres + 200);
            //float Y = gl.yres + 100; // Top offscreen

            MakeVector(X, Y, 0.0, hat.pos);

            // Calculate direction to player
            float dx = g.ship.pos[0] - hat.pos[0];
            float dy = g.ship.pos[1] - hat.pos[1];
            float dist = sqrt(dx * dx + dy * dy);
            if (dist != 0) {
                dx /= dist;
                dy /= dist;
            }
            MakeVector(dx * speed, dy * speed, 0.0, hat.vel);
            hat_active = 1;

        }
    }

    float dist = sqrt (
            (hat.pos[0] - g.ship.pos[0]) * (hat.pos[0] - g.ship.pos[0])
            + (hat.pos[1] - g.ship.pos[1]) * (hat.pos[1] - g.ship.pos[1])
            );
    float hat_radius = 100.0;
    if (dist < hat_radius + gl.player_size) {
        if (!gl.player_invincible) {
            gl.player_health -= 3;
            gl.player_invincible = 1;
            gl.invincible_timer = 3;

        }
    }
    for (int i = 0; i < g.nbullets; i++) {
        Bullet *b = &g.barr[i];
        float dx = b->pos[0] - hat.pos[0];
        float dy = b->pos[1] - hat.pos[1];
        float dist = sqrt(dx*dx + dy*dy);
        float bullet_radius = 10.0f;
        if (dist < (hat_radius + bullet_radius)) {
            //delete bullet
            memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
            g.nbullets--;
            hat_taking_damage();

        }
    }
   // }
}

void move_hound()
{
    //move hound
    int addgrav = 1;
    //Update position
    hound.pos[0] += hound.vel[0];
    hound.pos[1] += hound.vel[1];
    //Check for collision with window edges
    if ((hound.pos[0] < -140.0 && hound.vel[0] < 0.0) ||
        (hound.pos[0] >= (float)gl.xres+140.0 &&
        hound.vel[0] > 0.0))
    {
        hound.vel[0] = -hound.vel[0];
        addgrav = 0;
    }
    if ((hound.pos[1] < 150.0 && hound.vel[1] < 0.0) ||
        (hound.pos[1] >= (float)gl.yres && hound.vel[1] > 0.0)) {
        hound.vel[1] = -hound.vel[1];
        addgrav = 0;
    }
    //Gravity?
    if (addgrav)
        hound.vel[1] -= 0.75;

}

void witch_phys()
{
    //move witch
    int addgrav = 1;
    static float speed = 5.0f;
    static float wich_active = 0;
    //Update position
    if (!gl.game_paused) {
        wich.pos[0] += wich.vel[0];
        wich.pos[1] += wich.vel[1];
    }
    if (gl.current_level == 1) {
        //Check for collision with window edges
        if ((wich.pos[0] < -140.0 && wich.vel[0] < 0.0) ||
            (wich.pos[0] >= (float)gl.xres+140.0 &&
            wich.vel[0] > 0.0))
        {   
            wich.vel[0] = -wich.vel[0];
            addgrav = 0;
        }
        //  changing 150 to 0
        if ((wich.pos[1] < 100.0 && wich.vel[1] < 0.0) ||
            (wich.pos[1] >= (float)gl.yres && wich.vel[1] > 0.0)) {
            wich.vel[1] = -wich.vel[1];
            addgrav = 0;
        }
        //Gravity?
        if (addgrav)
            wich.vel[1] -= 0.75;
    }
    //if (gl.current_level == 2) {
    if (gl.current_level == 4 && (!gl.game_paused)) {
        //MakeVector(-150.0, 180.0, 0.0, wich.pos);
        if (!wich_active ||
        wich.pos[0] < -100 || wich.pos[0] > gl.xres + 100 ||
        wich.pos[1] < -100 || wich.pos[1] > gl.yres + 100) {
            // New position pick a corner
            int corner = rand() % 4;
            float X, Y;
            switch (corner) {
                case 0: // top left
                    X = -100.0f;
                    Y = gl.yres + 100.0f;
                    break;
                case 1: // top right
                    X = gl.xres + 100.0f;
                    Y = gl.yres + 100.0f;
                    break;
                case 2: // bottom left
                    X = -100.0f;
                    Y = 100.0f;
                    break;
                case 3: // bottom right
                    X = gl.xres + 100.0f;
                    Y = -100.0f;
                    break;
            }
            // this was a bit unfair for the players
            //float X = -100 + rand() % (gl.xres + 200);
            //float Y = gl.yres + 100; // Top offscreen

            MakeVector(X, Y, 0.0, wich.pos);

            // Calculate direction to player
            float dx = g.ship.pos[0] - wich.pos[0];
            float dy = g.ship.pos[1] - wich.pos[1];
            float dist = sqrt(dx * dx + dy * dy);
            if (dist != 0) {
                dx /= dist;
                dy /= dist;
            }
            MakeVector(dx * speed, dy * speed, 0.0, wich.vel);
            wich_active = 1;

        }
    }

    float dist = sqrt (
            (wich.pos[0] - g.ship.pos[0]) * (wich.pos[0] - g.ship.pos[0])
            + (wich.pos[1] - g.ship.pos[1]) * (wich.pos[1] - g.ship.pos[1])
            );
    float wich_radius = 100.0;
    if (dist < wich_radius + gl.player_size) {
        if (!gl.player_invincible) {
            gl.player_health -= 3;
            gl.player_invincible = 1;
            gl.invincible_timer = 3;

        }
    }
    for (int i = 0; i < g.nbullets; i++) {
        Bullet *b = &g.barr[i];
        float dx = b->pos[0] - wich.pos[0];
        float dy = b->pos[1] - wich.pos[1];
        float dist = sqrt(dx*dx + dy*dy);
        float bullet_radius = 10.0f;
        if (dist < (wich_radius + bullet_radius)) {
            //delete bullet
            memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
            g.nbullets--;


        }
    }
   // }
}

extern void tombstone_physics_on_slimes(Slime *s);
extern void tombstone_physics();
extern void witch_forest_physics();
extern void witch_house_physics();





void physics()
{
	//Update ship position
	g.ship.pos[0] += g.ship.vel[0];
	g.ship.pos[1] += g.ship.vel[1];


// --- Force zombie reset if level changed ---
static int previous_level = -1;
if (gl.current_level != previous_level) {
    // Clear all zombies
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        zombies[i].active = false;
    }

    // Only spawn if we're on level 1 or 4
    if (gl.current_level == 1 || gl.current_level == 4) {
        for (int i = 0; i < 3; i++) {
            spawnZombie();
        }
        zombie_wave_spawned = true;
        second_wave_spawned = false;
    } else {
        zombie_wave_spawned = false;
        second_wave_spawned = false;
    }

    previous_level = gl.current_level; // Update tracker
}


    // Add this new logic:
    if (g.ship.pos[0] < 0.0) {
        g.ship.pos[0] = 0.0; // Stop at the left edge
        g.ship.vel[0] = 0.0; // Set velocity to zero
    }
    else if (g.ship.pos[0] > (float)gl.xres) {
        g.ship.pos[0] = (float)gl.xres; // Stop at the right edge
        g.ship.vel[0] = 0.0; // Set velocity to zero
    }

    if (g.ship.pos[1] < 0.0) {
        g.ship.pos[1] = 0.0; // Stop at the bottom edge
        g.ship.vel[1] = 0.0; // Set velocity to zero
    }
    else if (g.ship.pos[1] > (float)gl.yres) {
        g.ship.pos[1] = (float)gl.yres; // Stop at the top edge
        g.ship.vel[1] = 0.0; // Set velocity to zero
    }

	//
	//Update bullet positions
	int i = 0;
	while (i < g.nbullets) {
		Bullet *b = &g.barr[i];
		b->pos[0] += b->vel[0];
		b->pos[1] += b->vel[1];
		//Check for collision with window edges


		// Bullets delete when wall hit
		if (b->pos[0] < 0.0) {
			// b->pos[0] += (float)gl.xres;
			memcpy(&g.barr[i], &g.barr[g.nbullets-1],
				sizeof(Bullet));
			g.nbullets--;
		}
		else if (b->pos[0] > (float)gl.xres) {
			// b->pos[0] -= (float)gl.xres;
			memcpy(&g.barr[i], &g.barr[g.nbullets-1],
				sizeof(Bullet));
			g.nbullets--;
		}
		else if (b->pos[1] < 0.0) {
			memcpy(&g.barr[i], &g.barr[g.nbullets-1],
				sizeof(Bullet));
			g.nbullets--;
		}
		else if (b->pos[1] > (float)gl.yres) {
			memcpy(&g.barr[i], &g.barr[g.nbullets-1],
				sizeof(Bullet));
			g.nbullets--;
		}
		++i;
	}

	//---------------------------------------------------
	//check keys pressed now
    //current g.ship.angle, changing that gl.witch.angle and so on
	if (gl.keys[XK_a] && (!gl.game_paused)) {
        // g.ship.pos[0] -= 1.0f;
		g.ship.angle += 4.0;
		if (g.ship.angle >= 360.0f)
			g.ship.angle -= 360.0f;
	}
	if (gl.keys[XK_d] && (!gl.game_paused)) {
        // g.ship.pos[0] += 1.0f;
		g.ship.angle -= 4.0;
		if (g.ship.angle < 0.0f)
			g.ship.angle += 360.0f;
	}
	if (gl.keys[XK_w] && (!gl.game_paused)) {
		//convert ship angle to radians
		Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
		//convert angle to a vector
		Flt xdir = cos(rad);
		Flt ydir = sin(rad);
		g.ship.pos[0] += xdir * 2.0f;
		g.ship.pos[1] += ydir * 2.0f;
	}
    if (gl.keys[XK_s] && gl.game_paused) {
		//apply thrust
		//convert ship angle to radians
		Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
		//convert angle to a vector
		Flt xdir = -cos(rad);
		Flt ydir = -sin(rad);
		// J ship movemnt, Swtiched ship.vel to ship.pos to remove drift effect
		g.ship.pos[0] += xdir * 2.0f;
		g.ship.pos[1] += ydir * 2.0f;
	}

    if (gl.keys[XK_p]) {
        if (!gl.pause_key_pressed && !gl.title_screen && !gl.leaderboard_screen) {
            gl.game_paused = !gl.game_paused;
            gl.pause_key_pressed = true;
        } 
    } else {
        gl.pause_key_pressed = false;
    }

    if (gl.keys[XK_h]) {
        if (gl.game_paused == true) {
            reset_game_stats();
            gl.title_screen = 1;
            gl.game_started = false;
        }
    }
    //Slime stuff
    // Update slime positions and chase player
    Slime *s = g.slimeHead;
    while (s) {
        // Simply update position based on current velocity
        // (No homing/tracking code)
        s->pos[0] += s->vel[0];
        s->pos[1] += s->vel[1];
        
        // Check for collision with window edges
        if (s->pos[0] < 0.0) {
            s->pos[0] = 0.0;
            s->vel[0] = -s->vel[0] * 0.5;  // Bounce off walls with some energy loss
        }
        else if (s->pos[0] > (float)gl.xres) {
            s->pos[0] = (float)gl.xres;
            s->vel[0] = -s->vel[0] * 0.5;
        }
        
        if (s->pos[1] < 0.0) {
            s->pos[1] = 0.0;
            s->vel[1] = -s->vel[1] * 0.5;
        }
        else if (s->pos[1] > (float)gl.yres) {
            s->pos[1] = (float)gl.yres;
            s->vel[1] = -s->vel[1] * 0.5;
        }
        
        s->angle += s->rotate;  // Still allow rotation for visual effect
        s = s->next;
    }

    // Slimes collision with bullets
    s = g.slimeHead;
    while (s) {
        // Is there a bullet within its radius?
        int i = 0;
        while (i < g.nbullets) {
            Bullet *b = &g.barr[i];
            Flt d0 = b->pos[0] - s->pos[0];
            Flt d1 = b->pos[1] - s->pos[1];
            Flt dist = (d0*d0 + d1*d1);

            if (dist < (s->radius*s->radius)) {
                // This slime is hit

                // Scoring based on slime size
                gl.player_score += (int)(50.0 / (s->radius / 30.0));

                if (s->radius > 15.0 && s->isParent) {
                    // Split into 3 smaller slimes
                    for (int k=0; k<3; k++) {
                        Slime *ts = new Slime;
                        buildSlimeFragment(ts, s);
                        ts->isParent = false;

                        // Add deviation for 3 distinct directions
                        float angle = (2.0 * PI * k) / 3.0;
                        ts->pos[0] += cos(angle) * s->radius * 0.5;
                        ts->pos[1] += sin(angle) * s->radius * 0.5;
                        ts->vel[0] += cos(angle) * 0.5;  // Was 1.0
                        ts->vel[1] += sin(angle) * 0.5; 

                        // Add to front of slime linked list
                        ts->next = g.slimeHead;
                        if (g.slimeHead != NULL)
                            g.slimeHead->prev = ts;
                        g.slimeHead = ts;
                        g.nslimes++;
                    }
                }

                // Delete the original slime
                Slime *temps = s->next;
                deleteSlime(s);
                s = temps;
                g.nslimes--;
                gl.monsters_killed++;
                // Delete the bullet
                memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
                g.nbullets--;

                // Check if we're at the end of the list
                if (s == NULL)
                    break;
                continue;
            }
            i++;
        }

        // Check for collision with player (damage player)
        if (s != NULL) {
            Flt d0 = g.ship.pos[0] - s->pos[0];
            Flt d1 = g.ship.pos[1] - s->pos[1];
            Flt dist = sqrt(d0*d0 + d1*d1);

            if (dist < (s->radius + gl.player_size)) {  // 10 is approximate player radius
                // Damage player based on slime size
                if (!gl.player_invincible) {
                    gl.player_health -= 2 + (int)(s->radius / 50.0);
                    gl.player_invincible = 1;
                    gl.invincible_timer = 3;

                }
                //gl.player_health -= 2 + (int)(s->radius / 50.0);
                if (gl.player_health < 0) gl.player_health = 0;

                // // Knockback player
                // float knockback = 5.0 + s->radius / 10.0;
                // if (dist > 0) {
                //     g.ship.vel[0] += (d0/dist) * knockback;
                //     g.ship.vel[1] += (d1/dist) * knockback;
                // }

                // Don't destroy slime on contact, just push it back
                if (dist > 0) {
                    s->vel[0] -= (d0/dist) * 3.0;
                    s->vel[1] -= (d1/dist) * 3.0;
                }
            }
        }
        // collision with tombstones
        if (s != NULL && gl.current_level == 1) {
            tombstone_physics_on_slimes(s);
        }

        if (s != NULL) {
            s = s->next;
        }
    }

    // If all slimes are destroyed, spawn a new wave
<<<<<<< HEAD
    if (g.nslimes == 0 && (gl.current_level == 1)) {
        initSlimes();
=======
    if (count_slimes() == 0 && (gl.current_level == 1 || gl.current_level == 4)) {
    initSlimes();
>>>>>>> zombie-feature
    }

    update_medkit();
	if (gl.current_level == 1)
        tombstone_physics();
    //&& (!gl.game_paused)
    if ((gl.current_level == 1))
        move_slimes();
    if (gl.current_level == 2)
        witch_forest_physics();
//    if (gl.current_level == 1 || gl.current_level == 2)
//        move_hat();
    if (gl.current_level == 3)
        witch_house_physics();
    if (gl.current_level == 4)
        witch_phys();

    if ((!gl.game_started)||(gl.current_level == 2))
        physics_hat();

    updateSlimeBoss();

// ================= Zombie collision with bullets =================
for (int i = 0; i < MAX_ZOMBIES; i++) {
    if (!zombies[i].active) continue;

    for (int j = 0; j < g.nbullets; j++) {
        Bullet *b = &g.barr[j];
        float dx = b->pos[0] - zombies[i].pos[0];
        float dy = b->pos[1] - zombies[i].pos[1];
        float dist = sqrt(dx * dx + dy * dy);
        float zombie_radius = 20.0f;

        if (dist < zombie_radius) {
            // Kill zombie
            zombies[i].active = false;

            // Remove bullet
            g.barr[j] = g.barr[g.nbullets - 1];
            g.nbullets--;

            break; // stop checking this zombie for other bullets
        }
    }
}


    // ================= Zombie Movement =================
for (int i = 0; i < MAX_ZOMBIES; i++) {
    if (zombies[i].active) {
        zombies[i].pos[0] += zombies[i].vel[0];
        zombies[i].pos[1] += zombies[i].vel[1];

        Vec dir;
        VecSub(g.ship.pos, zombies[i].pos, dir);
        normalize2d(dir);
        MakeVector(dir[0] * 0.5, dir[1] * 0.5, 0.0, zombies[i].vel);

        if (zombies[i].pos[0] < -50 || zombies[i].pos[0] > gl.xres + 50 ||
            zombies[i].pos[1] < -50 || zombies[i].pos[1] > gl.yres + 50) {
            zombies[i].active = false;
        }
    }
}


// ================= Zombie collision with player =================
for (int i = 0; i < MAX_ZOMBIES; i++) {
    if (zombies[i].active) {
        float dx = g.ship.pos[0] - zombies[i].pos[0];
        float dy = g.ship.pos[1] - zombies[i].pos[1];
        float dist = sqrt(dx*dx + dy*dy);
        float zombie_radius = 20.0f;
        float player_radius = gl.player_size;

        if (dist < (zombie_radius + player_radius)) {
            if (!gl.player_invincible) {
                gl.player_health -= 2;
                gl.player_invincible = 1;
                gl.invincible_timer = 3;
            }
        }
    }
}



// === Zombie wave control (fixed logic) ===
if ((gl.current_level == 1 || gl.current_level == 4) &&
    zombie_wave_spawned && !second_wave_spawned) {
    int zombies_alive = 0;
    for (int z = 0; z < MAX_ZOMBIES; z++) {
        if (zombies[z].active)
            zombies_alive++;
    }

    if (zombies_alive == 0) {
        // All zombies from first wave are gone
        for (int z = 0; z < 3; z++) {
            spawnZombie();
        }
        second_wave_spawned = true;
        printf("Second zombie wave incoming...\n");
    }
}

}





// added int xres and int yres so as to not cluter anything up here
//  and instead did all the work in another file
extern void show_all(Rect *r, int xres, int yres,
 float delta_time, int credits_activation);
extern void show_title(Rect *r, int xres, int yres);
//extern void draw_Iris();
// same thing here removing parameters Rect *r,int xres, int yres
extern void show_level_two(Rect *r, int xres, int yres);
extern void levelText(Rect *r);
//extern void show_score();
extern void draw();

/*void level_two_render() 
{
    Rect r;
    show_level_two(&r, gl.xres, gl.yres);
    //draw_Iris(); // removing the gl's
   //&r, gl.xres, gl.yres)
}
*/
void title_render() 
{
    Rect r;
//    Rect stats;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    float wid = 120;
    glClear(GL_COLOR_BUFFER_BIT);
    //glClearColor(0.0, 0.0, 0.0, 0.0);
    //--------------------------------------------
    //forest framework
    //glColor3f(1.0, 1.0, 1.0);
    //--------------------------------------------
    if(!gl.game_started) {
         if (gl.title_screen) {
        show_title(&r, gl.xres, gl.yres);
            gl.moon_shine_timer += 1;
        //draw_Iris(&r, gl.xres, gl.yres);
        gl.title_hat = 1;
        //glColor3f(1.0, 1.0, 1.0);
        glEnable(GL_TEXTURE_2D);
    if(gl.title_hat) {
        glPushMatrix();
        glTranslatef(hat.pos[0], hat.pos[1], hat.pos[2]);
        /// forest frame
        if (gl.hat_silhouette) {
            glBindTexture(GL_TEXTURE_2D, gl.hat_texture);
        } else {
            glBindTexture(GL_TEXTURE_2D, gl.hat_silhouette_texture);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.0f);
			glColor4ub(255,255,255,255);
        }

        /// ----------------------------------------------
        //glBindTexture(GL_TEXTURE_2D, gl.hat_texture);
    }
    glBegin(GL_QUADS);
    if(g.ship.vel[0] > 0.0) {
        glTexCoord2f(0.0f, 1.0f); glVertex2i(-wid,-wid);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(-wid, wid);
        glTexCoord2f(1.0f, 0.0f); glVertex2i( wid, wid);
        glTexCoord2f(1.0f, 1.0f); glVertex2i( wid,-wid);

    } else {
        glTexCoord2f(1.0f, 1.0f); glVertex2i(-wid,-wid);
        glTexCoord2f(1.0f, 0.0f); glVertex2i(-wid, wid);
        glTexCoord2f(0.0f, 0.0f); glVertex2i( wid, wid);
        glTexCoord2f(0.0f, 1.0f); glVertex2i( wid,-wid);
    }
    glEnd();
    glPopMatrix();
    }
         //glDisable(GL_TEXTURE_2D);
         //physics_hat();
     //    return;
    }
    //glDisable(GL_TEXTURE_2D);
}

extern void change_grass_color();
//extern void tombstone_physics();
void render()
{
        
	Rect r;
	Rect stats;
	glClear(GL_COLOR_BUFFER_BIT);
   // check_level_change_color();
    //rendering_background();
/*    if (gl.grass) {
        
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, gl.grass_texture);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
            glTexCoord2f(0.0f, 0.0f); glVertex2i(0, gl.yres);
            glTexCoord2f(1.0f, 0.0f); glVertex2i(gl.xres, gl.yres);
            glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres, 0);
        glEnd();
        
    } */
   // check_level_change_color();
    //rendering_background();
if (gl.game_started) {
    if(!gl.game_paused)
        gl.regular_timer += 1;
    if (gl.game_paused == true) {
        render_pause_screen(&r);
    } else {
        if (gl.grass) {
            change_grass_color();
            //glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
            glBindTexture(GL_TEXTURE_2D, gl.grass_texture);
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
                glTexCoord2f(0.0f, 0.0f); glVertex2i(0, gl.yres);
                glTexCoord2f(1.0f, 0.0f); glVertex2i(gl.xres, gl.yres);
                glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres, 0);
            glEnd();
            //glDisable(GL_TEXTURE_2D);
        }
        //glEnable(GL_TEXTURE_2D);
        //check_level_change_color();
        rendering_background();
        stats.bot = 0;
        stats.left = gl.xres-140;
        stats.center = 0;
        show_player_hearts(&stats, gl.yres);
        draw_player_health_bar(g.ship.pos[0], g.ship.pos[1], gl.player_health, 10);

        show_score();
        renderPLayerIcon(g.ship.pos[0], g.ship.pos[1], 40.0, g.ship.angle);

        
    ///-----------------------------------------------
        float wid = 120.0f;
        float scale =  1.0 - hat.damage;
    ////---------------------------------------------
        r.bot = gl.yres - 20;
        r.left = 10;
        r.center = 0;

        levelText(&r); // Current Level Text
      

        if (gl.credits) {
            Rect credits_rect;
            show_all(&credits_rect, gl.xres, gl.yres, timeSpan, gl.credits);
        }
        glColor3f(1.0, 1.0, 1.0);
        //glEnable(GL_TEXTURE_2D);
        //gl.main_hat = 1;
        if (gl.player_score >= 1000 && (!(gl.player_score >= 2000)))
            gl.main_hat = 1;
        else if (gl.player_score < 1000)
            gl.main_hat = 0;
        if(gl.main_hat && gl.current_level == 2) {
            //render_hound();
            //show_hound();
            glPushMatrix();
            // replacing with hat for the moment
            glTranslatef(hat.pos[0], hat.pos[1], hat.pos[2]);
            if (gl.hat_silhouette) {
                glBindTexture(GL_TEXTURE_2D, gl.hat_texture);
            } else {
                glBindTexture(GL_TEXTURE_2D, gl.hat_silhouette_texture);
                glEnable(GL_ALPHA_TEST);
                glAlphaFunc(GL_GREATER, 0.0f);
                glColor4ub(255,255,255,255);
            }
            
            
        if (gl.current_level == 4) {
            glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, gl.witch_texture);
            glTranslatef(wich.pos[0], wich.pos[1], wich.pos[2]);
        }

            //glBindTexture(GL_TEXTURE_2D, gl.hat_texture);
    // }
        if (hat.being_hit) {
            glColor4f(1.0f, 1.0f - hat.damage, 1.0 - hat.damage, 1.0f);
            glScalef(scale, scale, 1.0f);
        }
        glBegin(GL_QUADS);
        if(g.ship.vel[0] > 0.0) {
            glTexCoord2f(0.0f, 1.0f); glVertex2i(-wid,-wid);
            glTexCoord2f(0.0f, 0.0f); glVertex2i(-wid, wid);
            glTexCoord2f(1.0f, 0.0f); glVertex2i( wid, wid);
            glTexCoord2f(1.0f, 1.0f); glVertex2i( wid,-wid);

        } else {
            glTexCoord2f(1.0f, 1.0f); glVertex2i(-wid,-wid);
            glTexCoord2f(1.0f, 0.0f); glVertex2i(-wid, wid);
            glTexCoord2f(0.0f, 0.0f); glVertex2i( wid, wid);
            glTexCoord2f(0.0f, 1.0f); glVertex2i( wid,-wid);
        }
        glEnd();
        glPopMatrix();
    }

}
    
    renderSlimeBoss();
	
	//-------------------------------------------------------------------------
	//Draw the bullets
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0, 1.0, 1.0);
	for (int i=0; i<g.nbullets; i++) {
		Bullet *b = &g.barr[i];
		//Log("draw bullet...\n");
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_POINTS);
		glVertex2f(b->pos[0],      b->pos[1]);
		glVertex2f(b->pos[0]-1.0f, b->pos[1]);
		glVertex2f(b->pos[0]+1.0f, b->pos[1]);
		glVertex2f(b->pos[0],      b->pos[1]-1.0f);
		glVertex2f(b->pos[0],      b->pos[1]+1.0f);
		glColor3f(0.8, 0.8, 0.8);
		glVertex2f(b->pos[0]-1.0f, b->pos[1]-1.0f);
		glVertex2f(b->pos[0]-1.0f, b->pos[1]+1.0f);
		glVertex2f(b->pos[0]+1.0f, b->pos[1]-1.0f);
		glVertex2f(b->pos[0]+1.0f, b->pos[1]+1.0f);
		glEnd();
	}
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);
    // Draw the slimes
    
        glEnable(GL_TEXTURE_2D);
        Slime *s = g.slimeHead;
        while (s && !gl.game_paused) {
            glColor3fv(s->color);
            glPushMatrix();
            glTranslatef(s->pos[0], s->pos[1], s->pos[2]);
            glRotatef(s->angle, 0.0f, 0.0f, 1.0f);

            // Draw slime body (filled polygon)
            glBegin(GL_POLYGON);
            for (int j=0; j<s->nverts; j++) {
                glVertex2f(s->vert[j][0], s->vert[j][1]);
            }
            glEnd();

            // Draw slime outline
            glColor3f(0.0f, 0.5f, 0.0f);
            glBegin(GL_LINE_LOOP);
            for (int j=0; j<s->nverts; j++) {
                glVertex2f(s->vert[j][0], s->vert[j][1]);
            }
            glEnd();

            // Draw slime eyes (black)
            glColor3f(0.0f, 0.0f, 0.0f);
            float eyeSize = s->radius * 0.15;
            glBegin(GL_POLYGON);
            for (int i = 0; i < 8; i++) {
                float angle = (2.0 * PI * i) / 8.0;
                glVertex2f(s->radius * 0.3 + cos(angle) * eyeSize,
                        s->radius * 0.3 + sin(angle) * eyeSize);
            }
            glEnd();

            glBegin(GL_POLYGON);
            for (int i = 0; i < 8; i++) {
                float angle = (2.0 * PI * i) / 8.0;
                glVertex2f(-s->radius * 0.3 + cos(angle) * eyeSize,
                        s->radius * 0.3 + sin(angle) * eyeSize);
            }
            glEnd();

            // Draw slime mouth (black)
            glBegin(GL_LINE_STRIP);
            for (float i = -0.3; i <= 0.3; i += 0.05) {
                glVertex2f(i * s->radius, -0.1 * s->radius + sin(i*3) * 0.1 * s->radius);
            }
            glEnd();

            glPopMatrix();
            s = s->next;
        }
    
    r.bot = gl.yres - 110;
    render_medkit();
    render_nuke();
    // Add slime count to UI
    if (!gl.game_paused) {
        ggprint8b(&r, 16, 0x00ffff00, "n slimes: %i", g.nslimes);
        // ggprint8b(r, 24, 0xffffff00, bullets_accuracy_string_this_level);
        ggprint8b(&r, 16, 0x00ffff00, "c for credits: ");
        ggprint8b(&r, 15, 0x00ffff00, "t for title");
    }
//--------------------------------------------------------------------------

<<<<<<< HEAD
=======


        for (int i = 0; i < MAX_ZOMBIES; i++) {
    if (zombies[i].active) {
        glPushMatrix();
        glTranslatef(zombies[i].pos[0], zombies[i].pos[1], 0.0f);

        float radius = 22.0f;

        // === BODY (mutant slime style) ===
        glColor3f(0.5f, 0.0f, 0.0f); // deep red
        glBegin(GL_POLYGON);
        for (int j = 0; j < 20; j++) {
            float angle = 2.0f * PI * j / 20.0f;
            float r = radius + (j % 2 == 0 ? 2.5f : -1.5f); // wobble
            glVertex2f(cos(angle) * r, sin(angle) * r);
        }
        glEnd();

        // === EYES ===
        glColor3f(1.0f, 0.0f, 0.0f); // glowing red eyes
        glBegin(GL_QUADS);
        glVertex2f(-6, 6); glVertex2f(-4, 6);
        glVertex2f(-4, 8); glVertex2f(-6, 8);

        glVertex2f(4, 6); glVertex2f(6, 6);
        glVertex2f(6, 8); glVertex2f(4, 8);
        glEnd();

        // === MOUTH ===
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(-6, -4);
        glVertex2f(-4, -6);
        glVertex2f(-2, -4);
        glVertex2f(0, -6);
        glVertex2f(2, -4);
        glVertex2f(4, -6);
        glVertex2f(6, -4);
        glEnd();

        // === Tiny Claw Arms ===
        glColor3f(0.3f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        // Left claw
        glVertex2f(-14, 0);
        glVertex2f(-12, 0);
        glVertex2f(-12, 4);
        glVertex2f(-14, 4);
        // Right claw
        glVertex2f(12, 0);
        glVertex2f(14, 0);
        glVertex2f(14, 4);
        glVertex2f(12, 4);
        glEnd();

        glPopMatrix();
    }
}


       int activeZombies = 0;
for (int i = 0; i < MAX_ZOMBIES; i++) {
    if (zombies[i].active)
        activeZombies++;
}
ggprint8b(&r, 16, 0x00ff8800, "Zombies Active: %i", activeZombies);



    extern void drawSkeleton(); // Julio
>>>>>>> zombie-feature
}
}
