#include <stdlib.h> // malloc

#include <X11/Xlib.h> // X*, KeySym

#include "player.h"
#include "types.h"

struct MyPlayer* MyPlayer_new() {
  return (struct MyPlayer*)malloc(sizeof(struct MyPlayer));
}

void MyPlayer_initialize(struct MyPlayer* self) {
  self->dx = 0;
  self->dy = 0;
  MyPlayer_setX(self, 0);
  MyPlayer_setY(self, 0);
  self->color = 0;
  self->direction = PLAYER_STATIONARY;
}

void MyPlayer_setX(struct MyPlayer* self, int x) {
  MyPlayer_setLeft(self, x);
}

void MyPlayer_setY(struct MyPlayer* self, int y) {
  MyPlayer_setTop(self, y);
}

void MyPlayer_setLeft(struct MyPlayer* self, int leftX) {
  self->x = leftX;
  self->left = leftX;
  self->right = leftX + PLAYER_WIDTH;
}

void MyPlayer_setRight(struct MyPlayer* self, int rightX) {
  self->x = rightX - PLAYER_WIDTH;
  self->right = rightX;
  self->left = self->x;
}

void MyPlayer_setTop(struct MyPlayer* self, int topY) {
  self->y = topY;
  self->top = topY;
  self->bottom = topY + PLAYER_HEIGHT;
}

void MyPlayer_setBottom(struct MyPlayer* self, int bottomY) {
  self->y = bottomY - PLAYER_HEIGHT;
  self->bottom = bottomY;
  self->top = self->y;
}

void MyPlayer_update(struct MyPlayer* self) {
  switch (self->direction) {
    case PLAYER_LEFT:
      self->dx = -PLAYER_SPEED;
      self->dy = 0;
      break;
    
    case PLAYER_RIGHT:
      self->dx = +PLAYER_SPEED;
      self->dy = 0;
      break;
    
    case PLAYER_UP:
      self->dx = 0;
      self->dy = -PLAYER_SPEED;
      break;
    
    case PLAYER_DOWN:
      self->dx = 0;
      self->dy = +PLAYER_SPEED;
      break;
    
    case PLAYER_STATIONARY:
    default:
      self->dx = 0;
      self->dy = 0;
      break;
  }

  int x = self->x + self->dx;
  int y = self->y + self->dy;
  MyPlayer_setX(self, x);
  MyPlayer_setY(self, y);
}

void MyPlayer_draw(struct MyPlayer* self, struct MyWindow* myWindow) {
  XSetForeground(myWindow->display, myWindow->context, self->color);

  static const int PLAYER_VERTICES[] = {
    0, 0,
    PLAYER_WIDTH, 0,
    PLAYER_WIDTH, PLAYER_HEIGHT,
    0, PLAYER_HEIGHT,
  };

  MyWindow_drawPolygon(myWindow, PLAYER_VERTICES, self->x, self->y);
}
