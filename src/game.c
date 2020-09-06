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

void MyGame_initialize(struct MyGame* self, struct MyWindow* myWindow) {
  self->myWindow = myWindow;
  self->isPaused = FALSE;
  self->isDirty = TRUE;
  self->x = 0;
  self->y = 0;
  self->dx = 0;
  self->dy = 0;
}

Bool MyGame_handleInput(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;
  struct MyKeys* myKeys = &myWindow->keys;

  if ((myWindow->focus == FOCUS_OUT) && !self->isPaused) {
    self->isPaused = TRUE;
    _MyGame_notifyPauseChanged(self);
    myWindow->focus = FOCUS_OUT_DEBOUNCED;
  }

  if (myKeys->keyQ == KEY_PRESSED) {
    return FALSE;
  }

  if (myKeys->keyP == KEY_PRESSED) {
    _MyGame_togglePause(self);
    myKeys->keyP = KEY_DEBOUNCED;
  }

  if (!self->isPaused) {
    if (myKeys->keyA == KEY_PRESSED || myKeys->keyLeft == KEY_PRESSED) {
      self->dx = -OBJECT_SPEED;
      self->dy = 0;
    } else if (myKeys->keyD == KEY_PRESSED || myKeys->keyRight == KEY_PRESSED) {
      self->dx = +OBJECT_SPEED;
      self->dy = 0;
    } else if (myKeys->keyW == KEY_PRESSED || myKeys->keyUp == KEY_PRESSED) {
      self->dx = 0;
      self->dy = -OBJECT_SPEED;
    } else if (myKeys->keyS == KEY_PRESSED || myKeys->keyDown == KEY_PRESSED) {
      self->dx = 0;
      self->dy = +OBJECT_SPEED;
    } else {
      self->dx = 0;
      self->dy = 0;
    }
  }

  if (myWindow->mouse.hasMoved) {
    self->isDirty = TRUE;
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
  struct MyWindow* myWindow = self->myWindow;

  if (!self->isPaused && myWindow->focus) {
    // Update objects.
    if ((self->dx > 0) && (self->x + OBJECT_WIDTH < myWindow->windowWidth)) {
      self->x += self->dx;
      self->isDirty = TRUE;
    } else if ((self->dx < 0) && (self->x > 0)) {
      self->x += self->dx;
      self->isDirty = TRUE;
    }

    if ((self->dy > 0) && (self->y + OBJECT_HEIGHT < myWindow->windowHeight)) {
      self->y += self->dy;
      self->isDirty = TRUE;
    } else if ((self->dy < 0) && (self->y > 0)) {
      self->y += self->dy;
      self->isDirty = TRUE;
    }
  }
}

void MyGame_draw(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;

  if (!self->isDirty) {
    return;
  }

  XLockDisplay(myWindow->display);

  XSetLineAttributes(
    myWindow->display,
    myWindow->context,
    LINE_WIDTH,
    LineSolid,
    CapRound,
    JoinRound
  );

  _MyGame_drawBackground(self);
  _MyGame_drawPlayer(self);

  XUnlockDisplay(myWindow->display);
}

void _MyGame_drawBackground(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;

  ULong backgroundColor = myWindow->black.pixel;
  XSetBackground(myWindow->display, myWindow->context, backgroundColor);
  XClearWindow(myWindow->display, myWindow->window);
}

void _MyGame_drawPlayer(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;

  ULong playerColor = (!self->isPaused)
    ? myWindow->red.pixel
    : myWindow->white.pixel;
  XSetForeground(myWindow->display, myWindow->context, playerColor);

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
      myWindow->display,
      myWindow->window,
      myWindow->context,
      self->x + vertexAX,
      self->y + vertexAY,
      self->x + vertexBX,
      self->y + vertexBY
    );
  }
}
