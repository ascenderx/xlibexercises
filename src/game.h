#pragma once

#include "xdata.h"

struct MyGame {
  struct MyXData* xData;
  int x;
  int y;
  int dx;
  int dy;
};

struct MyGame* MyGame_new();
void MyGame_initialize(struct MyGame* self, struct MyXData* xData);
BOOL MyGame_update(struct MyGame* self);
void MyGame_draw(struct MyGame* self);
