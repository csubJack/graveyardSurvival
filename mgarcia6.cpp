#include "fonts.h"
#include "mgarcia6.h"
#include "global_game.h"
#include <iostream>
#include <cmath>
#include <ctime>

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
    float dx = g.player.pos[0] - g.medkit.pos[0];
    float dy = g.player.pos[1] - g.medkit.pos[1];
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

//================================================
// Slime Boss Implementation
//================================================

// Slime boss states
enum SlimeBossState {
    SLIME_BOSS_IDLE,
    SLIME_BOSS_CHASE,
    SLIME_BOSS_ATTACK,
    SLIME_BOSS_SPLIT,
    SLIME_BOSS_DEAD
};

// Slime boss structure
struct SlimeBoss {
    Vec pos;
    Vec vel;
    float radius;
    float angle;
    float rotate;
    float color[3];
    int health;
    int max_health;
    bool active;
    SlimeBossState state;
    struct timespec stateTimer;
    struct timespec attackTimer;
    int minionsSpawned;

    // Special attack variables
    float bounceForce;
    bool isBouncing;
    float splitTimer;

    // Constructor
    SlimeBoss() {
        pos[0] = pos[1] = pos[2] = 0.0f;
        vel[0] = vel[1] = vel[2] = 0.0f;
        radius = 85.0f;
        angle = 0.0f;
        rotate = 0.5f;
        color[0] = 0.4f;
        color[1] = 0.7f;
        color[2] = 1.0f;
        health = 100;
        max_health = 100;
        active = false;
        state = SLIME_BOSS_IDLE;
        minionsSpawned = 0;
        bounceForce = 0.0f;
        isBouncing = false;
        splitTimer = 0.0f;
    }
};

// Acid pool structure
struct AcidPool {
    float x, y;
    float radius;
    float lifetime;
    bool active;

    AcidPool() {
        x = y = 0.0f;
        radius = 30.0f;
        lifetime = 0.0f;
        active = false;
    }
};

// Create global instance of the slime boss
SlimeBoss slimeBoss;

// right now it is forced to spawn on level 3 with the house. We can change this later
// Wave tracking variables
static int slimeWavesDefeated = 0;
static int lastSlimeCount = 0;
static float bossWarningTimer = 0.0f;

// Acid pools
#define MAX_ACID_POOLS 10
static AcidPool acidPools[MAX_ACID_POOLS];

// Time related functions from main code
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
extern struct timespec timeCurrent;

bool isSlimeBossActive() {
    return slimeBoss.active;
}

// Function to create acid pools
void createAcidPool(float x, float y) {
    // Find an inactive acid pool slot
    for (int i = 0; i < MAX_ACID_POOLS; i++) {
        if (!acidPools[i].active) {
            acidPools[i].x = x;
            acidPools[i].y = y;
            acidPools[i].radius = 20.0f + rand() % 20;
            acidPools[i].lifetime = 0.0f;
            acidPools[i].active = true;
            return;
        }
    }
}

void integrateBossSystem() {
    // Initialize the acid pools
    for (int i = 0; i < MAX_ACID_POOLS; i++) {
        acidPools[i].active = false;
    }

    // Reset boss state
    slimeBoss.active = false;
    slimeBoss.health = slimeBoss.max_health;
    slimeBoss.state = SLIME_BOSS_IDLE;
    slimeBoss.minionsSpawned = 0;
    slimeBoss.isBouncing = false;
    slimeBoss.splitTimer = 0.0f;
    slimeWavesDefeated = 0;
    bossWarningTimer = 0.0f;
}

// Initialize the slime boss
void initSlimeBoss() {
    // Reset slime boss properties
    slimeBoss.health = slimeBoss.max_health;
    slimeBoss.active = true;
    slimeBoss.state = SLIME_BOSS_IDLE;
    slimeBoss.minionsSpawned = 0;
    slimeBoss.isBouncing = false;
    slimeBoss.splitTimer = 0.0f;

    // Position the boss at a random edge of the screen
    int side = rand() % 4;
    switch(side) {
        case 0: // Top
            slimeBoss.pos[0] = gl.xres / 2;
            slimeBoss.pos[1] = gl.yres + slimeBoss.radius;
            break;
        case 1: // Right
            slimeBoss.pos[0] = gl.xres + slimeBoss.radius;
            slimeBoss.pos[1] = gl.yres / 2;
            break;
        case 2: // Bottom
            slimeBoss.pos[0] = gl.xres / 2;
            slimeBoss.pos[1] = -slimeBoss.radius;
            break;
        case 3: // Left
            slimeBoss.pos[0] = -slimeBoss.radius;
            slimeBoss.pos[1] = gl.yres / 2;
            break;
    }

    // Set initial velocity toward player
    float dx = g.player.pos[0] - slimeBoss.pos[0];
    float dy = g.player.pos[1] - slimeBoss.pos[1];
    float dist = sqrt(dx*dx + dy*dy);
    if (dist > 0) {
        slimeBoss.vel[0] = dx/dist * 1.5f;
        slimeBoss.vel[1] = dy/dist * 1.5f;
    }

    // Initialize timers
    clock_gettime(CLOCK_REALTIME, &slimeBoss.stateTimer);
    clock_gettime(CLOCK_REALTIME, &slimeBoss.attackTimer);
}

// Spawn slime minions from the boss
void spawnSlimeBossMinions(int count, float size) {
    for (int i = 0; i < count; i++) {
        Slime *s = new Slime;
        s->nverts = 8;
        s->radius = size;

        Flt r2 = s->radius / 2.0;
        Flt angle = 0.0f;
        Flt inc = (PI * 2.0) / (Flt)s->nverts;
        for (int j = 0; j < s->nverts; j++) {
            s->vert[j][0] = sin(angle) * (r2 + (rand() / (float)RAND_MAX) * s->radius * 0.3);
            s->vert[j][1] = cos(angle) * (r2 + (rand() / (float)RAND_MAX) * s->radius * 0.3);
            angle += inc;
        }

        // Position around the boss
        float spawn_angle = (2.0 * PI * i) / count;
        s->pos[0] = slimeBoss.pos[0] + cos(spawn_angle) * (slimeBoss.radius + 10.0f);
        s->pos[1] = slimeBoss.pos[1] + sin(spawn_angle) * (slimeBoss.radius + 10.0f);
        s->pos[2] = 0.0f;

        // Inherit boss color but lighter
        s->color[0] = slimeBoss.color[0];
        s->color[1] = slimeBoss.color[1] * 0.8f;
        s->color[2] = slimeBoss.color[2];

        // Movement - move away from boss
        s->vel[0] = cos(spawn_angle) * 1.2f;
        s->vel[1] = sin(spawn_angle) * 1.2f;
        s->angle = 0.0f;
        s->rotate = slimeBoss.rotate + ((rand() / (float)RAND_MAX) * 2.0 - 1.0);

        // Add to slime linked list
        s->next = g.slimeHead;
        if (g.slimeHead != NULL)
            g.slimeHead->prev = s;
        g.slimeHead = s;
        g.nslimes++;
    }

    slimeBoss.minionsSpawned += count;
}

// Handle boss taking damage
void slimeBossTakeDamage(int damage) {
    slimeBoss.health -= damage;

    // Flash red when damaged
    slimeBoss.color[0] = 1.0f; // Set to red

    // After enough damage, change to split state
    if (slimeBoss.health <= slimeBoss.max_health * 0.5 && slimeBoss.state != SLIME_BOSS_SPLIT) {
        slimeBoss.state = SLIME_BOSS_SPLIT;
        clock_gettime(CLOCK_REALTIME, &slimeBoss.stateTimer);
    }

    // If boss is dead, spawn a bunch of smaller slimes and deactivate
    if (slimeBoss.health <= 0) {
        slimeBoss.state = SLIME_BOSS_DEAD;

        // Spawn 8-12 medium sized slimes after death
        int spawn_count = 8 + rand() % 5;
        for (int i = 0; i < spawn_count; i++) {
            spawnSlimeBossMinions(1, 20.0f + (rand() / (float)RAND_MAX) * 10.0f);
        }

        // Award a large score bonus
        gl.player_score += 1000;
        slimeBoss.active = false;
    }
}

// Bounce attack implementation
void slimeBossBounceAttack() {
    static float bounceCooldown = 0.0f;

    if (bounceCooldown > 0) {
        bounceCooldown -= 0.016f; // Using approximate dt
        return;
    }

    if (!slimeBoss.isBouncing) {
        // Initiate bounce attack
        slimeBoss.isBouncing = true;

        // Target the player
        float dx = g.player.pos[0] - slimeBoss.pos[0];
        float dy = g.player.pos[1] - slimeBoss.pos[1];
        float dist = sqrt(dx*dx + dy*dy);

        if (dist > 0) {
            //faster movement than base
            slimeBoss.vel[0] = dx/dist * 8.0f;
            slimeBoss.vel[1] = dy/dist * 8.0f;
        }

        // bounce cooldown
        bounceCooldown = 3.0f;
    }
    else {
        // Currently in a bounce, check if we need to stop
        float speed = sqrt(slimeBoss.vel[0]*slimeBoss.vel[0] + slimeBoss.vel[1]*slimeBoss.vel[1]);

        if (speed < 2.0f) {
            slimeBoss.isBouncing = false;

            // Slow down 
            slimeBoss.vel[0] *= 0.2f;
            slimeBoss.vel[1] *= 0.2f;
        }
    }
}

// Acid attack
void slimeBossAcidAttack() {
    static float acidCooldown = 0.0f;

    if (acidCooldown > 0) {
        acidCooldown -= 0.016f; // Using approximate dt
        return;
    }

    // Spawn 4-6 small acid slimes around the boss
    int acidCount = 4 + rand() % 3;
    for (int i = 0; i < acidCount; i++) {
        Slime *s = new Slime;
        s->nverts = 8;
        s->radius = 10.0f + (rand() / (float)RAND_MAX) * 5.0f;

        Flt r2 = s->radius / 2.0;
        Flt angle = 0.0f;
        Flt inc = (PI * 2.0) / (Flt)s->nverts;
        for (int j = 0; j < s->nverts; j++) {
            s->vert[j][0] = sin(angle) * (r2 + (rand() / (float)RAND_MAX) * s->radius * 0.3);
            s->vert[j][1] = cos(angle) * (r2 + (rand() / (float)RAND_MAX) * s->radius * 0.3);
            angle += inc;
        }

        // Position around the boss
        float spawn_angle = (2.0 * PI * i) / acidCount;
        s->pos[0] = slimeBoss.pos[0] + cos(spawn_angle) * (slimeBoss.radius);
        s->pos[1] = slimeBoss.pos[1] + sin(spawn_angle) * (slimeBoss.radius);
        s->pos[2] = 0.0f;

        // Make these acid slimes yellowish-green
        s->color[0] = 0.7f;
        s->color[1] = 0.9f;
        s->color[2] = 0.1f;

        // Fire them toward player
        float dx = g.player.pos[0] - s->pos[0];
        float dy = g.player.pos[1] - s->pos[1];
        float dist = sqrt(dx*dx + dy*dy);

        if (dist > 0) {
            s->vel[0] = dx/dist * 3.0f;
            s->vel[1] = dy/dist * 3.0f;
        }

        s->angle = 0.0f;
        s->rotate = slimeBoss.rotate * 2.0;

        // Add to slime linked list
        s->next = g.slimeHead;
        if (g.slimeHead != NULL)
            g.slimeHead->prev = s;
        g.slimeHead = s;
        g.nslimes++;
    }

    acidCooldown = 4.0f;
}

// Update acid pools
void updateAcidPools() {
    const float MAX_LIFETIME = 10.0f; // Acid pools last 10 seconds

    for (int i = 0; i < MAX_ACID_POOLS; i++) {
        if (!acidPools[i].active)
            continue;

        acidPools[i].lifetime += 0.016f; // Approximate frame time

        // Check if acid pool should disappear
        if (acidPools[i].lifetime >= MAX_LIFETIME) {
            acidPools[i].active = false;
            continue;
        }

        // Check if player is standing in acid
        float dx = g.player.pos[0] - acidPools[i].x;
        float dy = g.player.pos[1] - acidPools[i].y;
        float dist = sqrt(dx*dx + dy*dy);

        if (dist < acidPools[i].radius + gl.player_size) {
            // Deal damage over time
            if (!gl.player_invincible) {
                gl.player_health -= 1;
                gl.player_invincible = 1;
                gl.invincible_timer = 1; // Shorter invincibility for acid
            }
        }
    }
}

// Update the slime boss
void updateSlimeBoss() {
    // Check if we're on level 3 - only spawn boss there
    if (!slimeBoss.active) {
        // If we're on level 3, show the boss immediately
        if (gl.current_level == 3) {
            if (bossWarningTimer <= 0) {
                bossWarningTimer = 3.0f; // Show warning for 3 seconds
                // Only initialize if not already active
                if (!slimeBoss.active) {
                    initSlimeBoss();
                }
            }
            
            // Update warning timer
            if (bossWarningTimer > 0) {
                bossWarningTimer -= 0.016f;
            }
        } else {
            // Not on level 3, don't spawn boss
            return;
        }
        
        // Update acid pools even when boss is inactive
        updateAcidPools();
        return;
    }

    // Always rotate
    slimeBoss.angle += slimeBoss.rotate;

    // Update color - fade back to normal green after damage
    if (slimeBoss.color[0] > 0) {
        slimeBoss.color[0] *= 0.95f;
    }

    // Boss state machine
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    double stateTime = timeDiff(&slimeBoss.stateTimer, &currentTime);
    double attackTime = timeDiff(&slimeBoss.attackTimer, &currentTime);

    // Define boss behavior based on state
    switch(slimeBoss.state) {
        case SLIME_BOSS_IDLE:
            // In idle state, slowly move around
            if (!gl.game_paused) {
                slimeBoss.vel[0] *= 0.98f;
                slimeBoss.vel[1] *= 0.98f;
            }

            // Change to chase after a short time
            if (stateTime > 2.0) {
                slimeBoss.state = SLIME_BOSS_CHASE;
                clock_gettime(CLOCK_REALTIME, &slimeBoss.stateTimer);
            }
            break;

        case SLIME_BOSS_CHASE:
            // Chase the player
            {
                if (!gl.game_paused) {
                    float dx = g.player.pos[0] - slimeBoss.pos[0];
                    float dy = g.player.pos[1] - slimeBoss.pos[1];
                    float dist = sqrt(dx*dx + dy*dy);

                    // steer toward player
                    if (dist > 0) {
                        slimeBoss.vel[0] = slimeBoss.vel[0] * 0.95f + (dx/dist * 1.5f) * 0.05f;
                        slimeBoss.vel[1] = slimeBoss.vel[1] * 0.95f + (dy/dist * 1.5f) * 0.05f;
                    }

                    // limit speed
                    float speed = sqrt(slimeBoss.vel[0]*slimeBoss.vel[0] + slimeBoss.vel[1]*slimeBoss.vel[1]);
                    if (speed > 2.0f) {
                        slimeBoss.vel[0] = slimeBoss.vel[0] / speed * 2.0f;
                        slimeBoss.vel[1] = slimeBoss.vel[1] / speed * 2.0f;
                    }

                    // attack if close to player or after a while
                    if (dist < 150 || stateTime > 5.0) {
                        slimeBoss.state = SLIME_BOSS_ATTACK;
                        clock_gettime(CLOCK_REALTIME, &slimeBoss.stateTimer);
                        clock_gettime(CLOCK_REALTIME, &slimeBoss.attackTimer);
                    }
                } 
            }
            break;

        case SLIME_BOSS_ATTACK:
            if (attackTime > 1.0) {
                // choose between bounce and acid attack
                if (rand() % 2 == 0) {
                    slimeBossBounceAttack();
                } else {
                    slimeBossAcidAttack();
                }

                // spawn minions sometimes
                if (g.nslimes < 10 && slimeBoss.minionsSpawned < 20) {
                    spawnSlimeBossMinions(2 + rand() % 3, 15.0f + (rand() / (float)RAND_MAX) * 10.0f);
                }

                clock_gettime(CLOCK_REALTIME, &slimeBoss.attackTimer);
            }

            // back to chase after attack sequence
            if (stateTime > 8.0) {
                slimeBoss.state = SLIME_BOSS_CHASE;
                clock_gettime(CLOCK_REALTIME, &slimeBoss.stateTimer);
            }
            break;

        case SLIME_BOSS_SPLIT:
            {
                // Special state when boss is damaged enough
                // Slow down movement
                slimeBoss.vel[0] *= 0.9f;
                slimeBoss.vel[1] *= 0.9f;

                // Increase split timer
                slimeBoss.splitTimer += 0.016f;

                // Visual pulsing effect but it doesn't work yet
                //float pulseScale = 0.2f * sin(slimeBoss.splitTimer * 5.0f) + 1.0f;

                // spawn small slimes as the boss is about to split
                if (slimeBoss.splitTimer > 3.0f && slimeBoss.splitTimer < 3.5f) {
                    // create a ring of slimes
                    spawnSlimeBossMinions(8, 20.0f);

                    // split into two medium-sized bosses (simulation by creating larger slimes) 
                    // i believe this is a bit faulty as a new boss reappears sometime after the split
                    for (int i = 0; i < 2; i++) {
                        Slime *s = new Slime;
                        s->nverts = 8;
                        s->radius = slimeBoss.radius * 0.6f;

                        Flt r2 = s->radius / 2.0;
                        Flt angle = 0.0f;
                        Flt inc = (PI * 2.0) / (Flt)s->nverts;
                        for (int j = 0; j < s->nverts; j++) {
                            s->vert[j][0] = sin(angle) * (r2 + (rand() / (float)RAND_MAX) * s->radius * 0.3);
                            s->vert[j][1] = cos(angle) * (r2 + (rand() / (float)RAND_MAX) * s->radius * 0.3);
                            angle += inc;
                        }

                        // Position to either side
                        float offset = (i == 0) ? -40.0f : 40.0f;
                        s->pos[0] = slimeBoss.pos[0] + offset;
                        s->pos[1] = slimeBoss.pos[1];
                        s->pos[2] = 0.0f;

                        // Same color as boss but slightly varied
                        s->color[0] = 0.1f;
                        s->color[1] = 0.8f + (rand() / (float)RAND_MAX) * 0.2f;
                        s->color[2] = 0.1f;

                        // Velocity in opposite directions
                        s->vel[0] = (i == 0) ? -2.0f : 2.0f;
                        s->vel[1] = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;

                        s->angle = 0.0f;
                        s->rotate = slimeBoss.rotate * (0.8f + (rand() / (float)RAND_MAX) * 0.4f);

                        // Add to slime linked list
                        s->next = g.slimeHead;
                        if (g.slimeHead != NULL)
                            g.slimeHead->prev = s;
                        g.slimeHead = s;
                        g.nslimes++;
                    }

                    // Deactivate the original boss
                    slimeBoss.active = false;
                }
            }
            break;

        case SLIME_BOSS_DEAD:
            {
                // Boss is dead, clean up
                slimeBoss.active = false;
            }
            break;
    }

    // Update position - only if not in split mode or if split timer is low
    if ((slimeBoss.state != SLIME_BOSS_SPLIT || slimeBoss.splitTimer < 3.0f)
            && (!gl.game_paused)) {
        slimeBoss.pos[0] += slimeBoss.vel[0];
        slimeBoss.pos[1] += slimeBoss.vel[1];
    }

    // Check screen boundaries
    if (slimeBoss.pos[0] - slimeBoss.radius < 0) {
        slimeBoss.pos[0] = slimeBoss.radius;
        slimeBoss.vel[0] = fabs(slimeBoss.vel[0]) * 0.8f;
    } else if (slimeBoss.pos[0] + slimeBoss.radius > gl.xres) {
        slimeBoss.pos[0] = gl.xres - slimeBoss.radius;
        slimeBoss.vel[0] = -fabs(slimeBoss.vel[0]) * 0.8f;
    }

    if (slimeBoss.pos[1] - slimeBoss.radius < 0) {
        slimeBoss.pos[1] = slimeBoss.radius;
        slimeBoss.vel[1] = fabs(slimeBoss.vel[1]) * 0.8f;
    } else if (slimeBoss.pos[1] + slimeBoss.radius > gl.yres) {
        slimeBoss.pos[1] = gl.yres - slimeBoss.radius;
        slimeBoss.vel[1] = -fabs(slimeBoss.vel[1]) * 0.8f;
    }

    // Check for collision with bullets
    for (int i = 0; i < g.nbullets; i++) {
        Bullet *b = &g.barr[i];
        float dx = b->pos[0] - slimeBoss.pos[0];
        float dy = b->pos[1] - slimeBoss.pos[1];
        float dist = sqrt(dx*dx + dy*dy);

        if (dist < slimeBoss.radius) {
            // Bullet hits boss
            slimeBossTakeDamage(5);

            // Remove bullet
            memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
            g.nbullets--;
        }
    }

    // Check for collision with player (damage player)
    float dx = g.player.pos[0] - slimeBoss.pos[0];
    float dy = g.player.pos[1] - slimeBoss.pos[1];
    float dist = sqrt(dx*dx + dy*dy);

    if (dist < (slimeBoss.radius + gl.player_size)) {
        // Damage player more than regular slimes
        if (!gl.player_invincible) {
            gl.player_health -= 5;
            gl.player_invincible = 1;
            gl.invincible_timer = 3;
        }

        // Knockback player
        if (dist > 0) {
            g.player.pos[0] += (dx/dist) * 20.0f;
            g.player.pos[1] += (dy/dist) * 20.0f;
        }
    }

    //create random acid pools after some time
    static float acidPoolTimer = 0.0f;
    acidPoolTimer += 0.016f;

    if (acidPoolTimer >= 0.5f) {
        acidPoolTimer = 0.0f;
        Slime *s = g.slimeHead;
        while (s) {
            if (s->color[0] > 0.6f && s->color[1] > 0.8f && rand() % 10 == 0) {
                createAcidPool(s->pos[0], s->pos[1]);
            }
            s = s->next;
        }
    }

    updateAcidPools();
}

void renderAcidPools() {
    for (int i = 0; i < MAX_ACID_POOLS; i++) {
        if (!acidPools[i].active)
            continue;

        float alpha = 1.0f;
        if (acidPools[i].lifetime > 8.0f) {
            // Start fading out at 8 seconds
            alpha = 1.0f - (acidPools[i].lifetime - 8.0f) / 2.0f;
        }

        // transparent acid
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.5f, 0.9f, 0.1f, alpha * 0.5f);

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(acidPools[i].x, acidPools[i].y); // Center

        // creates shape of acid pool
        for (int j = 0; j <= 36; j++) {
            float angle = 2.0f * PI * j / 36.0f;
            float wobble = 0.8f + 0.2f * sin(angle * 4.0f + acidPools[i].lifetime * 2.0f);
            float x = acidPools[i].x + cos(angle) * acidPools[i].radius * wobble;
            float y = acidPools[i].y + sin(angle) * acidPools[i].radius * wobble;
            glVertex2f(x, y);
        }
        glEnd();

        // Draw outline
        glColor4f(0.2f, 0.7f, 0.0f, alpha * 0.8f);
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j <= 36; j++) {
            float angle = 2.0f * PI * j / 36.0f;
            float wobble = 0.8f + 0.2f * sin(angle * 4.0f + acidPools[i].lifetime * 2.0f);
            float x = acidPools[i].x + cos(angle) * acidPools[i].radius * wobble;
            float y = acidPools[i].y + sin(angle) * acidPools[i].radius * wobble;
            glVertex2f(x, y);
        }
        glEnd();

        glDisable(GL_BLEND);
    }
}
void renderSlimeBoss() {
    renderAcidPools();

    // boss warning message
    if (bossWarningTimer > 0.0f) {
        Rect r;
        r.bot = gl.yres / 2;
        r.left = gl.xres / 2 - 150;
        r.center = 0;

        // pulsing text
        float pulse = 0.7f + 0.3f * sin(bossWarningTimer * 5.0f);
        int color = (int)(pulse * 255) << 16 | (int)(pulse * 100) << 8;

        ggprint16(&r, 0, color, "WARNING: SLIME KING APPROACHING!");
    }

    // If boss isn't active maybe game over??
    if (!slimeBoss.active)
        return;

    float pulseScale = 1.0f;
    if (slimeBoss.state == SLIME_BOSS_SPLIT) {
        // makes boss pulse when about to split
        pulseScale = 0.2f * sin(slimeBoss.splitTimer * 5.0f) + 1.0f;
    }

    // Draw slime body
    glColor3fv(slimeBoss.color);
    glPushMatrix();
    glTranslatef(slimeBoss.pos[0], slimeBoss.pos[1], 0.0f);
    glRotatef(slimeBoss.angle, 0.0f, 0.0f, 1.0f);

    // irregular shape
    glBegin(GL_POLYGON);
    for (int i = 0; i < 16; i++) {
        float angle = (2.0 * PI * i) / 16.0;
        float wobble = 0.8f + 0.2f * sin(angle * 3 + slimeBoss.angle/10.0f);
        float x = cos(angle) * slimeBoss.radius * wobble * pulseScale;
        float y = sin(angle) * slimeBoss.radius * wobble * pulseScale;
        glVertex2f(x, y);
    }
    glEnd();

    // Draw outline
    glColor3f(0.0f, 0.5f, 0.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 16; i++) {
        float angle = (2.0 * PI * i) / 16.0;
        float wobble = 0.8f + 0.2f * sin(angle * 3 + slimeBoss.angle/10.0f);
        float x = cos(angle) * slimeBoss.radius * wobble * pulseScale;
        float y = sin(angle) * slimeBoss.radius * wobble * pulseScale;
        glVertex2f(x, y);
    }
    glEnd();

    // eyes
    glColor3f(0.0f, 0.0f, 0.0f);
    float eyeSize = slimeBoss.radius * 0.15f;

    glBegin(GL_POLYGON);
    for (int i = 0; i < 12; i++) {
        float angle = (2.0 * PI * i) / 12.0;
        float x = slimeBoss.radius * 0.4f + cos(angle) * eyeSize;
        float y = slimeBoss.radius * 0.3f + sin(angle) * eyeSize;
        glVertex2f(x, y);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 12; i++) {
        float angle = (2.0 * PI * i) / 12.0;
        float x = -slimeBoss.radius * 0.4f + cos(angle) * eyeSize;
        float y = slimeBoss.radius * 0.3f + sin(angle) * eyeSize;
        glVertex2f(x, y);
    }
    glEnd();

    // mouth
    glBegin(GL_LINE_STRIP);
    for (float i = -0.5; i <= 0.5; i += 0.05) {
        float x = i * slimeBoss.radius;
        float y = -0.2f * slimeBoss.radius + sin(i*6 + slimeBoss.angle/20.0f) * 0.1f * slimeBoss.radius;
        glVertex2f(x, y);
    }
    glEnd();

    //tried something with the eyebrows
    if (slimeBoss.state == SLIME_BOSS_ATTACK) {
        glBegin(GL_LINES);
        glVertex2f(-slimeBoss.radius * 0.5f, slimeBoss.radius * 0.45f);
        glVertex2f(-slimeBoss.radius * 0.3f, slimeBoss.radius * 0.35f);
        glVertex2f(slimeBoss.radius * 0.3f, slimeBoss.radius * 0.35f);
        glVertex2f(slimeBoss.radius * 0.5f, slimeBoss.radius * 0.45f);
        glEnd();
    }

    glPopMatrix();

    //health bar
    const float bar_width = slimeBoss.radius * 2.0f;
    const float bar_height = 8.0f;
    const float bar_y_offset = slimeBoss.radius + 20.0f;

    float health_x = slimeBoss.pos[0] - bar_width/2;
    float health_y = slimeBoss.pos[1] + bar_y_offset;

    // Health bar background (red)
    glColor3f(0.7f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(health_x, health_y);
    glVertex2f(health_x + bar_width, health_y);
    glVertex2f(health_x + bar_width, health_y + bar_height);
    glVertex2f(health_x, health_y + bar_height);
    glEnd();

    // Health bar fill (green)
    float fill_width = (float)slimeBoss.health / slimeBoss.max_health * bar_width;
    glColor3f(0.0f, 0.7f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(health_x, health_y);
    glVertex2f(health_x + fill_width, health_y);
    glVertex2f(health_x + fill_width, health_y + bar_height);
    glVertex2f(health_x, health_y + bar_height);
    glEnd();

    // Health bar outline
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(health_x, health_y);
    glVertex2f(health_x + bar_width, health_y);
    glVertex2f(health_x + bar_width, health_y + bar_height);
    glVertex2f(health_x, health_y + bar_height);
    glEnd();

    // slime king text above healhth bar
    Rect r;
    r.bot = health_y + bar_height + 15;
    r.left = slimeBoss.pos[0] - 50;
    r.center = 0;
    ggprint16(&r, 0, 0x00FFFFFF, "SLIME KING");
}

// HAD THIS FOR A SPAWN AFTER KILLING SLIMES BUT INSTEAD FORCED IT TO SPAWN ON LEVEL 3 FOR NOW
// Check if we should spawn the boss (track slime waves)
void checkSlimeBossSpawn() {
    // This function is called from main physics loop
    // Track kills via lastSlimeCount vs g.nslimes

    // If boss is already active, just update it
    if (slimeBoss.active) {
        return;
    }

    // If we completed a wave (had slimes, now have none)
    if (lastSlimeCount > 0 && g.nslimes == 0) {
        slimeWavesDefeated++;
    }

    // Update last count
    lastSlimeCount = g.nslimes;
}

