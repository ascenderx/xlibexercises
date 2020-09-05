#include <stdio.h> // printf
#include <stdlib.h> // malloc

#include "types.h"
#include "xdata.h"
#include "game.h"

// Foreground object properties.
#define OBJECT_SPEED 1
#define OBJECT_WIDTH 25
#define OBJECT_HEIGHT 25
#define LINE_WIDTH 2
#define NUM_PLAYER_VERTICES 4
#define PAUSE_MESSAGE "Paused.\n"
#define UNPAUSE_MESSAGE "Unpaused.\n"

struct MyGame* MyGame_new() {
  return (struct MyGame *)malloc(sizeof(struct MyGame));
}

void MyGame_initialize(struct MyGame* self, struct MyXData* xData) {
  self->xData = xData;
  self->isPaused = FALSE;
  self->isDirty = TRUE;
  self->x = 0;
  self->y = 0;
  self->dx = 0;
  self->dy = 0;
}

BOOL MyGame_handleInput(struct MyGame* self) {
  struct MyXData* xData = self->xData;
  struct MyKeys* keys = &xData->keys;

  if ((xData->focus == FOCUS_OUT) && !self->isPaused) {
    self->isPaused = TRUE;
    _MyGame_notifyPauseChanged(self);
    xData->focus = FOCUS_OUT_DEBOUNCED;
  }

  if (keys->keyQ == KEY_PRESSED) {
    return FALSE;
  }

  if (keys->keyP == KEY_PRESSED) {
    _MyGame_togglePause(self);
    keys->keyP = KEY_DEBOUNCED;
  }

  if (!self->isPaused) {
    if (keys->keyA == KEY_PRESSED || keys->keyLeft == KEY_PRESSED) {
      self->dx = -OBJECT_SPEED;
      self->dy = 0;
    } else if (keys->keyD == KEY_PRESSED || keys->keyRight == KEY_PRESSED) {
      self->dx = +OBJECT_SPEED;
      self->dy = 0;
    } else if (keys->keyW == KEY_PRESSED || keys->keyUp == KEY_PRESSED) {
      self->dx = 0;
      self->dy = -OBJECT_SPEED;
    } else if (keys->keyS == KEY_PRESSED || keys->keyDown == KEY_PRESSED) {
      self->dx = 0;
      self->dy = +OBJECT_SPEED;
    } else {
      self->dx = 0;
      self->dy = 0;
    }
  }

  return TRUE;
}

void _MyGame_togglePause(struct MyGame* self) {
  self->isPaused = !self->isPaused;
  _MyGame_notifyPauseChanged(self);
}

void _MyGame_notifyPauseChanged(struct MyGame* self) {
  self->isDirty = TRUE;
  printf(
    (self->isPaused)
    ? PAUSE_MESSAGE
    : UNPAUSE_MESSAGE
  );
}

void MyGame_update(struct MyGame* self) {
  struct MyXData* xData = self->xData;

  if (!self->isPaused && xData->focus) {
    // Update objects.
    if ((self->dx > 0) && (self->x + OBJECT_WIDTH < xData->windowWidth)) {
      self->x += self->dx;
      self->isDirty = TRUE;
    } else if ((self->dx < 0) && (self->x > 0)) {
      self->x += self->dx;
      self->isDirty = TRUE;
    }

    if ((self->dy > 0) && (self->y + OBJECT_HEIGHT < xData->windowHeight)) {
      self->y += self->dy;
      self->isDirty = TRUE;
    } else if ((self->dy < 0) && (self->y > 0)) {
      self->y += self->dy;
      self->isDirty = TRUE;
    }
  }
}

void MyGame_draw(struct MyGame* self) {
  struct MyXData* xData = self->xData;

  if (!self->isDirty) {
    return;
  }

  XLockDisplay(xData->display);

  ULONG backgroundColor = xData->black.pixel;
  ULONG playerColor = (!self->isPaused)
    ? xData->red.pixel
    : xData->white.pixel;

  XClearWindow(xData->display, xData->window);
  XSetBackground(xData->display, xData->context, backgroundColor);
  XSetForeground(xData->display, xData->context, playerColor);
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
