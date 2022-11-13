#include "gameControl.h"
#include "missile.h"
#include "touchscreen.h"
#include "config.h"

#define DELTA_X ((missiles[i].x_current - missiles[j].x_current) * (missiles[i].x_current - missiles[j].x_current))
#define DELTA_Y ((missiles[i].y_current - missiles[j].y_current) * (missiles[i].y_current - missiles[j].y_current))
#define RADIUS (missiles[j].radius * missiles[j].radius)

display_point_t location;

missile_t missiles[CONFIG_MAX_TOTAL_MISSILES];
missile_t *enemy_missiles = &(missiles[0]);
missile_t *player_missiles = &(missiles[CONFIG_MAX_ENEMY_MISSILES]);
// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void gameControl_init()
{
    display_fillScreen(DISPLAY_BLACK);
    for (uint16_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++)
        missile_init_dead(&missiles[i]);
}

// Tick the game control logic
//
// This function should tick the missiles, handle screen touches, collisions,
// and updating statistics.
void gameControl_tick()
{   
    for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++){
        if (missile_is_dead(&enemy_missiles[i])) 
            missile_init_enemy(&enemy_missiles[i]);
        
    }

  // Check for dead player missiles and re-initialize
    location = touchscreen_get_location();

    if(touchscreen_get_status() == TOUCHSCREEN_RELEASED)
    {
        touchscreen_ack_touch();
        
        for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++)
        {
            if (missile_is_dead(&player_missiles[i])) {
                missile_init_player(&player_missiles[i], location.x, location.y);
                touchscreen_ack_touch();
                break;
                
            }
        }
    }

    for (uint16_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) {
        missile_tick(&missiles[i]);
    }


    for (int16_t i = 0;i < CONFIG_MAX_ENEMY_MISSILES; i++){
        for (int16_t j = 0; j < CONFIG_MAX_TOTAL_MISSILES; j++){

            if (missile_is_flying(&missiles[i]) && missile_is_exploding(&missiles[j])){
                if((DELTA_Y + DELTA_X) < RADIUS)
                    missile_trigger_explosion(&missiles[i]);
            }
        }
    }

}
