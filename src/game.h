#pragma once

#include "types.h"
#include "xdata.h"
#include "player.h"

struct MyGame {
  struct MyWindow* myWindow;
  bool isPaused;
  bool isDirty;
  struct MyPlayer myPlayer;
};

struct MyGame* MyGame_new();
void MyGame_initialize(struct MyGame* self, struct MyWindow* myWindow);
bool MyGame_handleInput(struct MyGame* self);
void _MyGame_togglePause(struct MyGame* self);
void _MyGame_notifyPauseChanged(struct MyGame* self);
void MyGame_update(struct MyGame* self);
void MyGame_draw(struct MyGame* self);
void _MyGame_drawBackground(struct MyGame* self);
void _MyGame_drawObjects(struct MyGame* self);
