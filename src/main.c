#include <stdio.h> // printf
#include <unistd.h> // usleep
#include <pthread.h> // pthread*

#include "types.h"
#include "xdata.h"
#include "game.h"

#define REFRESH_INTERVAL 1000
#define START_MESSAGE \
  "Press W/A/S/D or Up/Left/Down/Right to move.\n" \
  "Press P to pause.\n" \
  "Press Q to quit.\n"

void* loop(void* argument) {
  struct MyGame* game = (struct MyGame*)argument;
  struct MyXData* xData = game->xData;

  // Begin the main loop. 
  while (TRUE) {
    // Update the window.
    MyXData_update(xData);

    // Update the game data.
    BOOL status = MyGame_handleInput(game);
    if (!status) {
      break;
    }
    MyGame_update(game);

    // Draw the game.
    MyGame_draw(game);

    // Sleep.
    usleep(REFRESH_INTERVAL);
  }

  // Clean up.
  MyXData_finalize(xData);
  printf("Goodbye.\n");

  return NULL;
}

int main() {
  // Initialize data.
  XInitThreads();
  struct MyXData xData;
  MyXData_initialize(&xData);
  struct MyGame game;
  MyGame_initialize(&game, &xData);

  printf(START_MESSAGE);

  pthread_t mainThread;
  pthread_create(&mainThread, NULL, loop, &game);
  pthread_join(mainThread, NULL);

  return 0;
}
