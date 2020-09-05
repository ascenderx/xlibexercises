#include <stdlib.h> // malloc

#include <X11/keysym.h> // XK_*

#include "types.h"
#include "xdata.h"
#include "game.h"

// Foreground object properties.
#define OBJECT_SPEED 1
#define OBJECT_WIDTH 25
#define OBJECT_HEIGHT 25
#define LINE_WIDTH 2
#define NUM_PLAYER_VERTICES 4

struct MyGame* MyGame_new() {
  return (struct MyGame *)malloc(sizeof(struct MyGame));
}

void MyGame_initialize(struct MyGame* self, struct MyXData* xData) {
  self->xData = xData;
  self->x = 0;
  self->y = 0;
  self->dx = 0;
  self->dy = 0;
}

BOOL MyGame_update(struct MyGame* self) {
  struct MyXData* xData = self->xData;

  switch (xData->keyDown) {
      case XK_Left:
        self->dx = -OBJECT_SPEED;
        self->dy = 0;
        break;
      
      case XK_Right:
        self->dx = +OBJECT_SPEED;
        self->dy = 0;
        break;
      
      case XK_Up:
        self->dx = 0;
        self->dy = -OBJECT_SPEED;
        break;
      
      case XK_Down:
        self->dx = 0;
        self->dy = +OBJECT_SPEED;
        break;
    }

    if (xData->previousKeyUp != xData->keyUp) {
      // If Q is pressed, then end the loop.
      if (xData->keyUp == XK_q || xData->keyUp == XK_Q) {
        return FALSE;
      }
    }

    switch (xData->keyUp) {
      case XK_Left:
      case XK_Right:
        self->dx = 0;
        break;
      
      case XK_Up:
      case XK_Down:
        self->dy = 0;
        break;
    }
    xData->keyUp = 0;

    // Update objects.
    if ((self->dx > 0) && (self->x + OBJECT_WIDTH < xData->windowWidth)) {
      self->x += self->dx;
    } else if ((self->dx < 0) && (self->x > 0)) {
      self->x += self->dx;
    }

    if ((self->dy > 0) && (self->y + OBJECT_HEIGHT < xData->windowHeight)) {
      self->y += self->dy;
    } else if ((self->dy < 0) && (self->y > 0)) {
      self->y += self->dy;
    }

    return TRUE;
}

void MyGame_draw(struct MyGame* self) {
  struct MyXData* xData = self->xData;

  XLockDisplay(xData->display);

  XClearWindow(xData->display, xData->window);
  XSetBackground(xData->display, xData->context, xData->black.pixel);
  XSetForeground(xData->display, xData->context, xData->red.pixel);
  XSetLineAttributes(
    xData->display,
    xData->context,
    LINE_WIDTH,
    LineSolid,
    CapRound,
    JoinRound
  );

  static const int PLAYER_VERTICES[NUM_PLAYER_VERTICES*2] = {
    0, 0,
    OBJECT_WIDTH, 0,
    OBJECT_WIDTH, OBJECT_HEIGHT,
    0, OBJECT_HEIGHT,
  };
  
  // Draw the edges, except the last.
  for (short v = 0; v < NUM_PLAYER_VERTICES; v++) {
    int indexA = v*2;
    int indexB = ((v + 1) % NUM_PLAYER_VERTICES)*2;
    int vertexAX = PLAYER_VERTICES[indexA];
    int vertexAY = PLAYER_VERTICES[indexA + 1];
    int vertexBX = PLAYER_VERTICES[indexB];
    int vertexBY = PLAYER_VERTICES[indexB + 1];
    XDrawLine(
      xData->display,
      xData->window,
      xData->context,
      self->x + vertexAX,
      self->y + vertexAY,
      self->x + vertexBX,
      self->y + vertexBY
    );
  }

  XUnlockDisplay(xData->display);
}
