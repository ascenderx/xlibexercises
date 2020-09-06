#pragma once

#include "xdata.h"
#include "types.h"

struct MyGame {
  struct MyWindow* myWindow;
  Bool isPaused;
  Bool isDirty;
  int x;
  int y;
  int dx;
  int dy;
};

struct MyGame* MyGame_new();
void MyGame_initialize(struct MyGame* self, struct MyWindow* myWindow);
Bool MyGame_handleInput(struct MyGame* self);
void _MyGame_togglePause(struct MyGame* self);
void _MyGame_notifyPauseChanged(struct MyGame* self);
void MyGame_update(struct MyGame* self);
void MyGame_draw(struct MyGame* self);
void _MyGame_drawBackground(struct MyGame* self);
void _MyGame_drawPlayer(struct MyGame* self);
