#include "fonts.h"
#include "mgarcia6.h"
#include "global_game.h"
#include <iostream>

void draw_player_health_bar(float x, float y, int health, int max_health)
{
    const float bar_width = 40.0f;
    const float bar_height = 5.0f;
    const float bar_y_offset = 30.0f; // Bar placement

    float health_x = x - bar_width/2;
    float health_y = y + bar_y_offset;

    // Draw background bar (missing health)
    glColor3f(0.5f, 0.0f, 0.0f); // Dark red
    glBegin(GL_QUADS);
    glVertex2f(health_x, health_y);
    glVertex2f(health_x + bar_width, health_y);
    glVertex2f(health_x + bar_width, health_y + bar_height);
    glVertex2f(health_x, health_y + bar_height);
    glEnd();

    float fill_width = (float)health / max_health * bar_width;

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(health_x, health_y);
    glVertex2f(health_x + fill_width, health_y);
    glVertex2f(health_x + fill_width, health_y + bar_height);
    glVertex2f(health_x, health_y + bar_height);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(health_x, health_y);
    glVertex2f(health_x + bar_width, health_y);
    glVertex2f(health_x + bar_width, health_y + bar_height);
    glVertex2f(health_x, health_y + bar_height);
    glEnd();
}
