#include "fonts.h"
#include "global_game.h"
#include "jcornejo.h"
#include "types.h"
#include <GL/glx.h>
#include <string>
#include <cstdlib>
#include <sys/time.h>
#include <cmath>
#include <ctime>

extern void render();

// Cloud properties arrays
const int MAX_CLOUDS = 30;
float cloudX[MAX_CLOUDS];
float cloudY[MAX_CLOUDS];
float cloudSpeed[MAX_CLOUDS];
float cloudSize[MAX_CLOUDS];
float cloudOpacity[MAX_CLOUDS];
bool cloudActive[MAX_CLOUDS];

// Time variables
int lastTime = 0;
float cloudSpawnTimer = 0;
int activeCloudCount = 0;

// Get current time in milliseconds
int getCurrentTimeMillis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// Initialize a cloud at given index, the 'f's are to specify that those nums are floats
void initCloud(int index) {
    cloudX[index] = -100.0f + (rand() % 100);
    cloudY[index] = gl.yres - 60.0f - (rand() % 100);
    cloudSpeed[index] = 0.05f + ((rand() % 10) / 100.0f);
    cloudSize[index] = 30.0f + (rand() % 40);
    cloudOpacity[index] = 0.5f + ((rand() % 50) / 100.0f);
    cloudActive[index] = true;
}

// Draw an ellipse for cloud parts
void drawEllipse(float centerX, float centerY, float radiusX, float radiusY) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);
    
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        float x = centerX + radiusX * cos(angle);
        float y = centerY + radiusY * sin(angle);
        glVertex2f(x, y);
    }
    
    glEnd();
}

// Draw a single cloud
void drawCloud(int index) {
    if (!cloudActive[index]) 
        return;
    
    glPushMatrix();
    glTranslatef(cloudX[index], cloudY[index], 0.0f);
    glScalef(cloudSize[index], cloudSize[index] / 2, 1.0f);
    
    // Set cloud color
    glColor4f(1.0f, 1.0f, 1.0f, cloudOpacity[index]);
    
    // Draw base ellipse
    drawEllipse(0.0f, 0.0f, 1.0f, 1.0f);
    
    // Draw random "puffs" to form cloud shape
    int numPuffs = 4 + (index % 4);
    for (int i = 0; i < numPuffs; i++) {
        float puffX = ((index * 7 + i * 13) % 100) / 100.0f;
        float puffY = ((index * 11 + i * 17) % 60) / 100.0f - 0.3f;
        float puffSize = 0.5f + ((index * 3 + i * 5) % 50) / 100.0f;
        
        drawEllipse(puffX, puffY, puffSize, puffSize);
    }
    
    glPopMatrix();
}

// Find an inactive cloud slot
int findFreeCloudSlot() {
    for (int i = 0; i < MAX_CLOUDS; i++) {
        if (!cloudActive[i]) {
            return i;
        }
    }
    return -1; // No free slots
}

// Function to initialize the cloud system
void initClouds() {
    srand(static_cast<unsigned int>(time(nullptr)));
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set all clouds as not used
    for (int i = 0; i < MAX_CLOUDS; i++) {
        cloudActive[i] = false;
    }
    
    for (int i = 0; i < 20; i++) {
        // For first few clouds to spawn scattered across screen instead of all just the left
        cloudX[i] = (rand() % gl.xres);
        cloudY[i] = gl.yres - 60.0f - (rand() % 100);
        cloudSpeed[i] = 0.05f + ((rand() % 10) / 100.0f);
        cloudSize[i] = 30.0f + (rand() % 40);
        cloudOpacity[i] = 0.5f + ((rand() % 50) / 100.0f);
        cloudActive[i] = true;
        activeCloudCount++;
    }
    
    lastTime = getCurrentTimeMillis();
}

// Render clouds
void renderClouds() {
    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw all active clouds
    for (int i = 0; i < MAX_CLOUDS; i++) {
        if (cloudActive[i]) {
            drawCloud(i);
        }
    }
    glPopMatrix();
}

// Function to update cloud positions and states
void updateClouds() {
    // Calculate time
    unsigned long currentTime = getCurrentTimeMillis();
    float deltaTime = (currentTime - lastTime) * 0.1f;
    lastTime = currentTime;
    
    // Update cloud spawn timer
    cloudSpawnTimer += deltaTime;
    
    // Spawn new cloud based on timer
    if (cloudSpawnTimer > 200 && activeCloudCount < MAX_CLOUDS) {
        int newCloudIndex = findFreeCloudSlot();
        if (newCloudIndex >= 0) {
            initCloud(newCloudIndex);
            activeCloudCount++;
        }
        cloudSpawnTimer = 0;
    }
    
    // Update all active clouds
    for (int i = 0; i < MAX_CLOUDS; i++) {
        if (cloudActive[i]) {
            // Move cloud
            cloudX[i] += cloudSpeed[i] * deltaTime;
            
            // Check if cloud is off screen if so delete
            if (cloudX[i] > gl.xres + 100) {
                cloudActive[i] = false;
                activeCloudCount--;
            }
        }
    }
}

void cloudSystemInit() {
    initClouds();
}

void cloudSystemRender() {
    renderClouds();
    updateClouds();
}

string generateLevelName(int rand, int curLevel) 
{
    string lev1Names[5] = {"Graveborn Awakening", "Crypt of the Restless", "The Ooze and the Dead", "Rot and Resin", "Mire of the Undead"};
    string lev2Names[5] = {"Hexwood Clearing", "Tip of the Hex", "Brimveil Woods", "The Hollow Brim", "The Silent Stitch"};
    string lev3Names[5] = {"The Slime King's March", "Siege by Slime", "Sanctum Under Slime", "The Slime King's Descent", "The Withering Hold"};
    string lev4Names[5] = {"The Last Meadow", "Roots Soaked in Resolve", "The Grass Stands Red", "Ashes in the Wilds", "Field of the Forgotten Stand"};
    
    string nameHolder;
    switch(curLevel) {
        case 1:
            nameHolder = lev1Names[rand];
            break;
        case 2:
            nameHolder = lev2Names[rand];
            break;
        case 3:
            nameHolder = lev3Names[rand];
            break;
        case 4:
            nameHolder = lev4Names[rand];
            break;
    }
    
    return nameHolder;
}

string throwLevelName(int level) 
{
    // Only generate a new name when the level changes or on first call
    static int lastLevel = -1;
    
    // Checks if current level is same as lastLevel var if not call function to pull
    if (level != lastLevel || !gl.levelNameGenerated) {
        int randNum = rand() % 5;

        gl.currentLevelName = generateLevelName(randNum, level);

        lastLevel = level;

        gl.levelNameGenerated = true;
    }
    
    return gl.currentLevelName;
}

void levelText(Rect *r)
{    
    // Level name
    ggprint8b(r, 16, 0x00ffffff, "%s", throwLevelName(gl.current_level).c_str());

    // Level Number
    ggprint8b(r, 16, 0x00ffff00, "LEVEL %i", gl.current_level);
}

void resetLevelName() {
    gl.levelNameGenerated = false;
}

void draw() { 
    glColor3fv(g.player.color);
    glPushMatrix();
    glTranslatef(g.player.pos[0], g.player.pos[1], g.player.pos[2]);
    glRotatef(g.player.angle, 0.0f, 0.0f, 1.0f);

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

        Flt rad = ((g.player.angle+90.0) / 360.0f) * PI * 2.0;
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
            glVertex2f(g.player.pos[0]+xs,g.player.pos[1]+ys);
            glVertex2f(g.player.pos[0]+xe,g.player.pos[1]+ye);
        }
        glEnd();
    }

}