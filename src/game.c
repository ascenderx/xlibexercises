#include <stdio.h> // printf
#include <stdlib.h> // malloc

#include "game.h"
#include "xdata.h"
#include "types.h"
#include "player.h"

/**********
 * 
 **********/
struct MyGame* MyGame_new(void) {
  return (struct MyGame*)malloc(sizeof(struct MyGame));
}

#define HUD_LEFT 0
#define HUD_TOP 0
#define HUD_HEIGHT 50
#define GAME_LEFT 0
#define GAME_TOP (HUD_TOP + HUD_HEIGHT + 1)
#define HUD_PADDING_LEFT 2

/**********
 * 
 **********/
void MyGame_initialize(struct MyGame* self, struct MyWindow* myWindow) {
  self->myWindow = myWindow;
  self->isPaused = false;
  self->isDirty = true;
#ifdef DEBUG
  self->numTimesRedrawn = 0;
#endif
  MyPlayer_initialize(&self->myPlayer, GAME_LEFT, GAME_TOP);
}

/**********
 * 
 **********/
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
      if (myPlayer->right < myWindow->attributes.width) {
        myPlayer->direction = PLAYER_RIGHT;
      } else {
        myPlayer->direction = PLAYER_STATIONARY;
        MyPlayer_setRight(myPlayer, myWindow->attributes.width);
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
      if (myPlayer->bottom < myWindow->attributes.height) {
        myPlayer->direction = PLAYER_DOWN;
      } else {
        myPlayer->direction = PLAYER_STATIONARY;
        MyPlayer_setBottom(myPlayer, myWindow->attributes.height);
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

/**********
 * 
 **********/
void _MyGame_togglePause(struct MyGame* self) {
  self->isPaused = !self->isPaused;
  _MyGame_notifyPauseChanged(self);
}

#define PAUSE_MESSAGE "Paused.\n"
#define UNPAUSE_MESSAGE "Unpaused.\n"

/**********
 * 
 **********/
void _MyGame_notifyPauseChanged(struct MyGame* self) {
  self->isDirty = true;
  printf(
    (self->isPaused)
    ? PAUSE_MESSAGE
    : UNPAUSE_MESSAGE
  );
}

/**********
 * 
 **********/
void MyGame_update(struct MyGame* self) {
  struct MyPlayer* myPlayer = &self->myPlayer;
  struct MyWindow* myWindow = self->myWindow;
  struct MyMouse* myMouse = &myWindow->myMouse;

  MyPlayer_update(myPlayer);

  if (myMouse->hasMoved) {
    self->isDirty = true;
  }
}

#define LINE_WIDTH 2

/**********
 * 
 **********/
void MyGame_draw(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;

  if (!self->isDirty && !myWindow->didResize) {
    return;
  }

#ifdef DEBUG
  self->numTimesRedrawn++;
  printf("Redrawing (%d)...\n", self->numTimesRedrawn);
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
  _MyGame_drawHud(self);

  XUnlockDisplay(myWindow->display);

  self->isDirty = false;
}

/**********
 * 
 **********/
void _MyGame_drawBackground(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;

  XColor* backgroundColor = &myWindow->black;
  MyWindow_setBackgroundColor(myWindow, backgroundColor);
  MyWindow_clear(myWindow);
}

/**********
 * 
 **********/
void _MyGame_drawHud(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;
  struct MyMouse* myMouse = &myWindow->myMouse;

  MyWindow_setForegroundColor(myWindow, &myWindow->white);
  MyWindow_drawRectangle(myWindow, HUD_LEFT, HUD_TOP, myWindow->attributes.width, HUD_HEIGHT);

  MyWindow_setForegroundColor(myWindow, &myWindow->white);
  ushort textRow = 1;
  ushort fontSize = myWindow->fontSize;
  MyWindow_drawText(
    myWindow,
    HUD_PADDING_LEFT,
    fontSize*(textRow++),
    "Player: (%d,%d) [%d,%d] \n",
    self->myPlayer.x - GAME_LEFT,
    self->myPlayer.y - GAME_TOP,
    self->myPlayer.x,
    self->myPlayer.y
  );
  MyWindow_drawText(
    myWindow,
    HUD_PADDING_LEFT,
    fontSize*(textRow++),
    "Mouse: (%d,%d) \n",
    myMouse->x,
    myMouse->y
  );
  if (self->isPaused) {
    MyWindow_setForegroundColor(myWindow, &myWindow->red);
    MyWindow_drawText(
      myWindow,
      HUD_PADDING_LEFT,
      fontSize*(textRow++),
      "Paused \n"
    );
  }
}

/**********
 * 
 **********/
void _MyGame_drawObjects(struct MyGame* self) {
  struct MyWindow* myWindow = self->myWindow;
  struct MyPlayer* myPlayer = &self->myPlayer;

  // Draw the player.
  myPlayer->color = (!self->isPaused)
    ? &myWindow->red
    : &myWindow->white;
  MyPlayer_draw(myPlayer, myWindow);  
}
