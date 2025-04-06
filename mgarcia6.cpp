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
void spawn_medkit()
{
    if (!g.medkit.active && g.nslimes == 0) {
        // If there was a slime head before all slimes were killed
        if (g.slimeHead != NULL) {
            // Spawn at the location of the last slime
            g.medkit.pos[0] = g.slimeHead->pos[0];
            g.medkit.pos[1] = g.slimeHead->pos[1];
            g.medkit.pos[2] = 0.0f;
            g.medkit.active = true;
        } else {
            // Fallback position if there's no slime to reference
            g.medkit.pos[0] = gl.xres / 2 + (rand() % 300 - 150);
            g.medkit.pos[1] = gl.yres / 2 + (rand() % 300 - 150);
            g.medkit.pos[2] = 0.0f;
            g.medkit.active = true;
        }
    }
}

void check_medkit_collision()
{
    if (!g.medkit.active)
        return;
    
    // Calculate distance between player and med kit
    float dx = g.ship.pos[0] - g.medkit.pos[0];
    float dy = g.ship.pos[1] - g.medkit.pos[1];
    float distance = sqrt(dx*dx + dy*dy);
    
    // If player is close enough, restore health and deactivate med kit
    if (distance < (g.medkit.radius + gl.player_size)) {
        // Only restore up to maximum health (10)
        gl.player_health = 10; // Set to exact max health value
        g.medkit.active = false;
    }
}

void update_medkit()
{
    // Check if the last slime was just killed
    static int last_nslimes = g.nslimes;

    if (last_nslimes > 0 && g.nslimes == 0) {
        // We just killed the last slime, spawn a medkit
        spawn_medkit();
    }

    // Update last count
    last_nslimes = g.nslimes;

    // Update glow effect
    g.medkit.glowIntensity += 0.02f * g.medkit.glowDirection;
    if (g.medkit.glowIntensity > 1.0f) {
        g.medkit.glowIntensity = 1.0f;
        g.medkit.glowDirection = -1.0f; // Start decreasing
    } else if (g.medkit.glowIntensity < 0.4f) {
        g.medkit.glowIntensity = 0.4f;
        g.medkit.glowDirection = 1.0f; // Start increasing
    }

    check_medkit_collision();
}

void render_medkit()
{
    if (!g.medkit.active)
        return;

    // Outer glow
    float glow_radius = g.medkit.radius * (1.5f + 0.5f * g.medkit.glowIntensity);
    glColor4f(0.0f, g.medkit.glowIntensity * 0.8f, 0.0f, 0.5f * g.medkit.glowIntensity);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(g.medkit.pos[0], g.medkit.pos[1]); // Center
    for (int i = 0; i <= 36; i++) {
        float angle = 2 * PI * i / 36;
        glVertex2f(
            g.medkit.pos[0] + cos(angle) * glow_radius,
            g.medkit.pos[1] + sin(angle) * glow_radius
        );
    }
    glEnd();

    // Draw med kit body (white box)
    glColor3f(1.0f, 1.0f, 1.0f);
    float size = g.medkit.radius * 0.7f;

    glBegin(GL_QUADS);
    glVertex2f(g.medkit.pos[0] - size, g.medkit.pos[1] - size);
    glVertex2f(g.medkit.pos[0] + size, g.medkit.pos[1] - size);
    glVertex2f(g.medkit.pos[0] + size, g.medkit.pos[1] + size);
    glVertex2f(g.medkit.pos[0] - size, g.medkit.pos[1] + size);
    glEnd();

    // Draw cross (green)
    glColor3f(0.0f, 0.8f, 0.0f);
    float cross_thickness = size * 0.3f;

    // Horizontal part of the cross
    glBegin(GL_QUADS);
    glVertex2f(g.medkit.pos[0] - size * 0.8f, g.medkit.pos[1] - cross_thickness/2);
    glVertex2f(g.medkit.pos[0] + size * 0.8f, g.medkit.pos[1] - cross_thickness/2);
    glVertex2f(g.medkit.pos[0] + size * 0.8f, g.medkit.pos[1] + cross_thickness/2);
    glVertex2f(g.medkit.pos[0] - size * 0.8f, g.medkit.pos[1] + cross_thickness/2);
    glEnd();

    // Vertical part of the cross
    glBegin(GL_QUADS);
    glVertex2f(g.medkit.pos[0] - cross_thickness/2, g.medkit.pos[1] - size * 0.8f);
    glVertex2f(g.medkit.pos[0] + cross_thickness/2, g.medkit.pos[1] - size * 0.8f);
    glVertex2f(g.medkit.pos[0] + cross_thickness/2, g.medkit.pos[1] + size * 0.8f);
    glVertex2f(g.medkit.pos[0] - cross_thickness/2, g.medkit.pos[1] + size * 0.8f);
    glEnd();

    glDisable(GL_BLEND);
}
