#include "missile.h"
#include "config.h"
#include "gameControl.h"
#include "stdio.h"
#include "display.h"
#include "math.h"

#define DROP_ZONE (DISPLAY_HEIGHT * 3 / 4)
#define GROUND DISPLAY_HEIGHT
#define LAUNCH_SITE_BOUNDARY_1 (3 * DISPLAY_WIDTH / 8)
#define LAUNCH_SITE_BOUNDARY_2 (5 * DISPLAY_WIDTH / 8)

#define PLAYER_LAUNCH_SITE_1 (DISPLAY_WIDTH / 4)
#define PLAYER_LAUNCH_SITE_2 (DISPLAY_WIDTH / 2)
#define PLAYER_LAUNCH_SITE_3 (3 * DISPLAY_WIDTH / 4)

enum missile_st {
  initialSt,
  moveSt,
  explodeSt,
  shrinkSt,
  deadSt,
};




void missile_init_dead(missile_t *missile)
{
    missile->currentState = deadSt;
}

// Initialize the missile as an enemy missile.  This will randomly choose the
// origin and destination of the missile.  The origin should be somewhere near
// the top of the screen, and the destination should be the very bottom of the
// screen.
void missile_init_enemy(missile_t *missile)
{
    missile->type = MISSILE_TYPE_ENEMY; 
    missile->x_origin = rand() % DISPLAY_WIDTH;
    missile->y_origin = rand() % DROP_ZONE;
    missile->x_dest = rand() % DISPLAY_WIDTH;
    missile->y_dest = GROUND;
    missile->currentState = initialSt;
}

// Initialize the missile as a player missile.  This function takes an (x, y)
// destination of the missile (where the user touched on the touchscreen).  The
// origin should be the closest "firing location" to the destination (there are
// three firing locations evenly spaced along the bottom of the screen).
void missile_init_player(missile_t *missile, uint16_t x_dest, uint16_t y_dest)
{
    missile->type = MISSILE_TYPE_PLAYER;
    if(missile->x_dest < LAUNCH_SITE_BOUNDARY_1) 
        missile->x_origin = PLAYER_LAUNCH_SITE_1;
    else if(missile->x_dest > LAUNCH_SITE_BOUNDARY_2)
        missile->x_origin = PLAYER_LAUNCH_SITE_2;
    else
        missile->x_origin = PLAYER_LAUNCH_SITE_3;
    missile->y_origin = GROUND;
    missile->x_dest = x_dest;
    missile->y_dest = y_dest;
    missile->currentState = initialSt;
}

// Initialize the missile as a plane missile.  This function takes an (x, y)
// location of the plane which will be used as the origin.  The destination can
// be randomly chosed along the bottom of the screen.
/*void missile_init_plane(missile_t *missile, int16_t plane_x, int16_t plane_y)
{
    missile->type = MISSILE_TYPE_PLANE;
    missile->x_origin = plane_x;
    missile->y_origin = plane_y;
    missile->x_dest = rand() % DISPLAY_WIDTH;
    missile->y_dest = GROUND;
    missile->currentState = initialSt;
}*/

////////// State Machine TICK Function //////////
void missile_tick(missile_t *missile)
{
    switch(missile->currentState)
    {
        case initialSt:
            missile->length = 0;
            missile->explode_me = false;
            missile->total_length = sqrt(((missile->y_dest - missile->y_origin)*(missile->y_dest - missile->y_origin)) + ((missile->x_dest - missile->x_origin)*(missile->x_dest - missile->x_origin)));
            missile->x_current = missile->x_origin; 
            missile->y_current = missile->y_origin; 
            missile->impacted = false;
            missile->currentState = moveSt;
            break;

        case moveSt:
            
            if(missile->length >= missile->total_length)
            {
                display_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, DISPLAY_BLACK);
                missile->currentState = deadSt;
            }
        break;

        case explodeSt:
        //if(missile->radius == CONFIG_EXPLOSION_MAX_RADIUS)
            //missile->currentState = shrinkSt;
        break;

        case shrinkSt:
        //if(missile_is_dead(missile))
            //missile->currentState = deadSt;
        break;

        case deadSt:
        break;

        default:
        break;
    }


    switch(missile->currentState)
    {
        case moveSt:
            
            display_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, DISPLAY_BLACK);
            if(missile->type == MISSILE_TYPE_PLAYER)
            {
                missile->length += CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK;
                missile->x_current = missile->x_origin + (missile->length / missile->total_length) * (missile->x_dest - missile->x_origin);
                missile->y_current = missile->y_origin + (missile->length / missile->total_length) * (missile->y_dest - missile->y_origin);
                display_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, CONFIG_COLOR_PLAYER);
                printf("%d\n", missile->currentState);
                break;
            }
            else if(missile->type == MISSILE_TYPE_ENEMY)
            {
                missile->length += CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
                missile->x_current = missile->x_origin + (missile->length / missile->total_length) * (missile->x_dest - missile->x_origin);
                missile->y_current = missile->y_origin + (missile->length / missile->total_length) * (missile->y_dest - missile->y_origin);
                display_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, CONFIG_COLOR_ENEMY);
                printf("%d\n", missile->currentState);
                break;
            }
            else
            {
                missile->length += CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
                missile->x_current = missile->x_origin + (missile->length / missile->total_length) * (missile->x_dest - missile->x_origin);
                missile->y_current = missile->y_origin + (missile->length / missile->total_length) * (missile->y_dest - missile->y_origin);
                display_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, CONFIG_COLOR_PLANE);
                break;
            }
            
            break;

        case explodeSt:
        //missile->radius++;
        break;

        case shrinkSt:
        //missile->radius--;
        break;

        default:
        break;
    }
}

// Return whether the given missile is dead.
bool missile_is_dead(missile_t *missile)
{
    if(missile->currentState == deadSt)
        return true;
}

// Return whether the given missile is exploding.  This is needed when detecting
// whether a missile hits another exploding missile.
bool missile_is_exploding(missile_t *missile)
{

}

// Return whether the given missile is flying.
bool missile_is_flying(missile_t *missile)
{

}

// Used to indicate that a flying missile should be detonated.  This occurs when
// an enemy or plane missile is located within an explosion zone.
void missile_trigger_explosion(missile_t *missile)
{

}
