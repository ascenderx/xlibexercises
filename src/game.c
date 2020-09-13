#include <stdio.h> // printf
#include <stdlib.h> // malloc

#include "game.h"
#include "xdata.h"
#include "types.h"
#include "player.h"

// Foreground object properties.
#define LINE_WIDTH 2
#define PAUSE_MESSAGE "Paused.\n"
#define UNPAUSE_MESSAGE "Unpaused.\n"
#define GAME_TOP 50
#define GAME_LEFT 0

struct MyGame* MyGame_new(void) {
  return (struct MyGame *)malloc(sizeof(struct MyGame));
}

void MyGame_initialize(struct MyGame* self, struct MyWindow* myWindow) {
  self->myWindow = myWindow;
  self->isPaused = false;
  self->isDirty = true;
  MyPlayer_initialize(&self->myPlayer, GAME_LEFT, GAME_TOP);
}

bool MyGame_handleInput(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;
  struct MyKeys* myKeys = &myWindow->myKeys;
  struct MyPlayer* myPlayer = &self->myPlayer;

  if ((myWindow->focus == FOCUS_OUT) && !self->isPaused) {
    self->isPaused = true;
    _MyGame_notifyPauseChanged(self);
    myWindow->focus = FOCUS_OUT_DEBOUNCED;
  }

  if (myKeys->keyQ == KEY_PRESSED) {
    return false;
  }

  if (myKeys->keyP == KEY_PRESSED) {
    _MyGame_togglePause(self);
    myKeys->keyP = KEY_DEBOUNCED;
  }

  if (!self->isPaused) {
    if (myKeys->keyA == KEY_PRESSED || myKeys->keyLeft == KEY_PRESSED) {
      if (myPlayer->left > GAME_LEFT) {
        myPlayer->direction = PLAYER_LEFT;
      } else {
        myPlayer->direction = PLAYER_STATIONARY;
        MyPlayer_setLeft(myPlayer, GAME_LEFT);
      }
      self->isDirty = True;
    } else if (myKeys->keyD == KEY_PRESSED || myKeys->keyRight == KEY_PRESSED) {
      if (myPlayer->right < myWindow->windowWidth) {
        myPlayer->direction = PLAYER_RIGHT;
      } else {
        myPlayer->direction = PLAYER_STATIONARY;
        MyPlayer_setRight(myPlayer, myWindow->windowWidth);
      }
      self->isDirty = True;
    } else if (myKeys->keyW == KEY_PRESSED || myKeys->keyUp == KEY_PRESSED) {
      if (myPlayer->top > GAME_TOP) {
        myPlayer->direction = PLAYER_UP;
      } else {
        myPlayer->direction = PLAYER_STATIONARY;
        MyPlayer_setTop(myPlayer, GAME_TOP);
      }
      self->isDirty = True;
    } else if (myKeys->keyS == KEY_PRESSED || myKeys->keyDown == KEY_PRESSED) {
      if (myPlayer->bottom < myWindow->windowHeight) {
        myPlayer->direction = PLAYER_DOWN;
      } else {
        myPlayer->direction = PLAYER_STATIONARY;
        MyPlayer_setBottom(myPlayer, myWindow->windowHeight);
      }
      self->isDirty = True;
    } else {
      if (myPlayer->direction != PLAYER_STATIONARY) {
        self->isDirty = True;
      }
      myPlayer->direction = PLAYER_STATIONARY;
    }
  }

  return true;
}

void _MyGame_togglePause(struct MyGame* self) {
  self->isPaused = !self->isPaused;
  _MyGame_notifyPauseChanged(self);
}

void _MyGame_notifyPauseChanged(struct MyGame* self) {
  self->isDirty = true;
  printf(
    (self->isPaused)
    ? PAUSE_MESSAGE
    : UNPAUSE_MESSAGE
  );
}

void MyGame_update(struct MyGame* self) {
  struct MyPlayer* myPlayer = &self->myPlayer;

  MyPlayer_update(myPlayer);
}

void MyGame_draw(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;

  if (!self->isDirty) {
    return;
  }
#ifdef DEBUG
  printf("Redrawing...\n");
#endif // DEBUG

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
  _MyGame_drawObjects(self);

  XUnlockDisplay(myWindow->display);

  self->isDirty = False;
}

void _MyGame_drawBackground(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;

  ulong backgroundColor = myWindow->black.pixel;
  XSetBackground(myWindow->display, myWindow->context, backgroundColor);
  XClearWindow(myWindow->display, myWindow->window);
}

void _MyGame_drawObjects(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;
  struct MyPlayer* myPlayer = &self->myPlayer;

  // Draw the player.
  myPlayer->color = (!self->isPaused)
    ? &myWindow->red
    : &myWindow->white;
  MyPlayer_draw(myPlayer, myWindow);  
}
