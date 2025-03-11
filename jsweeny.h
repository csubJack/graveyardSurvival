#define JSWEENY_H

#include "fonts.h"

// Functions to be used in Main file

class Game;

void show_player_hearts(Rect *r, int yres, int playerHealth);
float update_player_angle(float xCoordinate, float xPlayer, float yCoordinate, float yPlater);