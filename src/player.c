#include <stdlib.h> // malloc

#include <X11/Xlib.h> // X*, KeySym

#include "player.h"
#include "types.h"

/**********
 * 
 **********/
struct MyPlayer* MyPlayer_new(void) {
  return (struct MyPlayer*)malloc(sizeof(struct MyPlayer));
}

/**********
 * 
 **********/
void MyPlayer_initialize(struct MyPlayer* self, int x, int y) {
  self->dx = 0;
  self->dy = 0;
  MyPlayer_setX(self, x);
  MyPlayer_setY(self, y);
  self->color = NULL;
  self->direction = PLAYER_STATIONARY;

  static const int PLAYER_VERTICES[PLAYER_VERTEX_COUNT * 2] = {
    0, 0,
    PLAYER_WIDTH, 0,
    PLAYER_WIDTH, PLAYER_HEIGHT,
    0, PLAYER_HEIGHT,
    0, 0,
  };
  for (ushort p = 0; p < PLAYER_VERTEX_COUNT; p++) {
    self->_relativeVertices[p].x = PLAYER_VERTICES[p*2];
    self->_relativeVertices[p].y = PLAYER_VERTICES[p*2 + 1];
  }
}

/**********
 * 
 **********/
void MyPlayer_setX(struct MyPlayer* self, int x) {
  MyPlayer_setLeft(self, x);
}

/**********
 * 
 **********/
void MyPlayer_setY(struct MyPlayer* self, int y) {
  MyPlayer_setTop(self, y);
}

/**********
 * 
 **********/
void MyPlayer_setLeft(struct MyPlayer* self, int leftX) {
  self->x = leftX;
  self->left = leftX;
  self->right = leftX + PLAYER_WIDTH;
}

/**********
 * 
 **********/
void MyPlayer_setRight(struct MyPlayer* self, int rightX) {
  self->x = rightX - PLAYER_WIDTH;
  self->right = rightX;
  self->left = self->x;
}

/**********
 * 
 **********/
void MyPlayer_setTop(struct MyPlayer* self, int topY) {
  self->y = topY;
  self->top = topY;
  self->bottom = topY + PLAYER_HEIGHT;
}

/**********
 * 
 **********/
void MyPlayer_setBottom(struct MyPlayer* self, int bottomY) {
  self->y = bottomY - PLAYER_HEIGHT;
  self->bottom = bottomY;
  self->top = self->y;
}

/**********
 * 
 **********/
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

/**********
 * 
 **********/
void _MyPlayer_updateVertices(struct MyPlayer* self) {
  for (ushort v = 0; v < PLAYER_VERTEX_COUNT; v++) {
    self->vertices[v].x = self->_relativeVertices[v].x + self->x;
    self->vertices[v].y = self->_relativeVertices[v].y + self->y;
  }
}

/**********
 * 
 **********/
void MyPlayer_draw(struct MyPlayer* self, struct MyWindow* myWindow) {
  if (self->color == NULL) {
    return;
  }

  _MyPlayer_updateVertices(self);
  MyWindow_setForegroundColor(myWindow, self->color);
  MyWindow_drawPolygon(myWindow, self->vertices, PLAYER_VERTEX_COUNT);
}
