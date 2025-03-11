#include "global_game.h"  // Include the header that defines Game and Global
#include <iostream>
#include <cmath>

extern Global gl;  // Access global variables
extern Game g;     // Access the game object

// Function to update witch movement and keep it inside screen bounds
void updateWitchMovement() {
    g.witch.pos[0] += g.witch.vel[0];
    g.witch.pos[1] += g.witch.vel[1];

    // Prevent Witch from going beyond the screen borders
    if (g.witch.pos[0] < 0) {
        g.witch.pos[0] = 0;
        g.witch.vel[0] = 0;
    } else if (g.witch.pos[0] > gl.xres) {
        g.witch.pos[0] = gl.xres;
        g.witch.vel[0] = 0;
    }

    if (g.witch.pos[1] < 0) {
        g.witch.pos[1] = 0;
        g.witch.vel[1] = 0;
    } else if (g.witch.pos[1] > gl.yres) {
        g.witch.pos[1] = gl.yres;
        g.witch.vel[1] = 0;
    }

    // Print Witch position for debugging
    std::cout << "Witch Position: (" << g.witch.pos[0] << ", " << g.witch.pos[1] << ")\n";
}

