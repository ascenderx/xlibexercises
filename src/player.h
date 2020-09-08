#pragma once

#include "types.h"
#include "xdata.h"

// Player directions.
#define PLAYER_STATIONARY 0
#define PLAYER_LEFT 1
#define PLAYER_RIGHT 2
#define PLAYER_UP 3
#define PLAYER_DOWN 4

// Player geometry.
#define PLAYER_SPEED 1
#define PLAYER_WIDTH 25
#define PLAYER_HEIGHT 25

struct MyPlayer {
  int x;
  int y;
  int dx;
  int dy;
  int left;
  int right;
  int top;
  int bottom;
  UByte direction;
  ULong color;
};

struct MyPlayer* MyPlayer_new();
void MyPlayer_initialize(struct MyPlayer* self);
void _MyPlayer_updateBoundaries(struct MyPlayer* self);
void MyPlayer_setX(struct MyPlayer* self, int x);
void MyPlayer_setY(struct MyPlayer* self, int y);
void MyPlayer_setLeft(struct MyPlayer* self, int leftX);
void MyPlayer_setRight(struct MyPlayer* self, int rightX);
void MyPlayer_setTop(struct MyPlayer* self, int topY);
void MyPlayer_setBottom(struct MyPlayer* self, int bottomY);
void MyPlayer_update(struct MyPlayer* self);
void MyPlayer_draw(struct MyPlayer* self, struct MyWindow* myWindow);
