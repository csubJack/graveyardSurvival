#ifndef GLOBAL_GAME_H
#define GLOBAL_GAME_H

class Global {
public:
    int xres, yres;
};

class Game {
public:
    struct Witch {
        float pos[3];
        float vel[3];
    } witch;
};

extern Global gl;
extern Game g;

#endif

