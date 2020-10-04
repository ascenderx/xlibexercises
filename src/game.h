#ifndef _GAME_H
#define _GAME_H

#include "types.h"
#include "xdata.h"
#include "player.h"

/**********
 * 
 **********/
struct MyGame {
  struct MyWindow* myWindow;
  bool isPaused;
  bool isDirty;
  struct MyPlayer myPlayer;
#ifdef DEBUG
  uint numTimesRedrawn;
#endif
};

struct MyGame* MyGame_new(void);
void MyGame_initialize(struct MyGame* self, struct MyWindow* myWindow);
bool MyGame_handleInput(struct MyGame* self);
void _MyGame_togglePause(struct MyGame* self);
void _MyGame_notifyPauseChanged(struct MyGame* self);
void MyGame_update(struct MyGame* self);
void MyGame_draw(struct MyGame* self);
void _MyGame_drawBackground(struct MyGame* self);
void _MyGame_drawHud(struct MyGame* self);
void _MyGame_drawObjects(struct MyGame* self);

#endif // _GAME_H
