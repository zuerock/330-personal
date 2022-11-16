#include "missile.h"
#include "config.h"
#include "display.h"
#include "gameControl.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "plane.h"

#define PLANE_START_HEIGHT (DISPLAY_HEIGHT / 4)
#define PLANE_HEIGHT 5
#define PLANE_WIDTH 15
#define X_0 planeLocation.x
#define X_1 (planeLocation.x + PLANE_WIDTH)
#define X_2 (planeLocation.x + PLANE_WIDTH)
#define Y_0 planeLocation.y
#define Y_1 (planeLocation.y + PLANE_HEIGHT)
#define Y_2 (planeLocation.y - PLANE_HEIGHT)


enum plane_st_t{
  moveSt,
  explodeSt,
  deadSt,
};

static enum plane_st_t planeSt;

display_point_t planeLocation;


// Initialize the plane state machine
// Pass in a pointer to the missile struct (the plane will only have one
// missile)
void plane_init(missile_t *plane_missile)
{
    planeSt = moveSt;
    planeLocation.x = DISPLAY_WIDTH;
    planeLocation.y = DISPLAY_HEIGHT / 4;
}

// State machine tick function
void plane_tick()
{
    switch (planeSt)
    {
    case moveSt:
        if(planeLocation.x == 0)
        {
            display_fillTriangle(X_0,Y_0,X_1,Y_1,X_2,Y_2,DISPLAY_BLACK);
            planeSt = deadSt;
        }
        planeLocation.x--;
        break;
    
    default:
        break;
    }

    switch (planeSt)
    {
    case moveSt:
        display_fillTriangle(X_0,Y_0,X_1,Y_1,X_2,Y_2,DISPLAY_WHITE);
        break;
    
    default:
        break;
    }
}

// Trigger the plane to explode
void plane_explode()
{

}

// Get the XY location of the plane
display_point_t plane_getXY()
{
    return planeLocation;
}
