#ifndef _PLAYER_H
#define _PLAYER_H

#include <X11/Xlib.h> // X*

#include "types.h"
#include "xdata.h"

// Player directions.
enum MyPlayerDirection {
  PLAYER_STATIONARY,
  PLAYER_LEFT,
  PLAYER_RIGHT,
  PLAYER_UP,
  PLAYER_DOWN,
};

// Player geometry.
#define PLAYER_SPEED 1
#define PLAYER_WIDTH 25
#define PLAYER_HEIGHT 25
#define PLAYER_VERTEX_COUNT 5

/**********
 * 
 **********/
struct MyPlayer {
  int x;
  int y;
  int dx;
  int dy;
  int left;
  int right;
  int top;
  int bottom;
  enum MyPlayerDirection direction;
  XColor* color;
  // Defined points are relative to the player's coordinates. 
  XPoint _relativeVertices[PLAYER_VERTEX_COUNT];
  // Computed points are relative to the origin.
  XPoint vertices[PLAYER_VERTEX_COUNT];
};

struct MyPlayer* MyPlayer_new(void);
void MyPlayer_initialize(struct MyPlayer* self, int x, int y);
void _MyPlayer_updateBoundaries(struct MyPlayer* self);
void MyPlayer_setX(struct MyPlayer* self, int x);
void MyPlayer_setY(struct MyPlayer* self, int y);
void MyPlayer_setLeft(struct MyPlayer* self, int leftX);
void MyPlayer_setRight(struct MyPlayer* self, int rightX);
void MyPlayer_setTop(struct MyPlayer* self, int topY);
void MyPlayer_setBottom(struct MyPlayer* self, int bottomY);
void MyPlayer_update(struct MyPlayer* self);
void _MyPlayer_updateVertices(struct MyPlayer* self);
void MyPlayer_draw(struct MyPlayer* self, struct MyWindow* myWindow);

#endif // _PLAYER_H
