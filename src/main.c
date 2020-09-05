#include <stdio.h> // printf
#include <unistd.h> // usleep
#include <pthread.h> // pthread*

#include "types.h"
#include "xdata.h"
#include "game.h"

#define REFRESH_INTERVAL 1000

void* loop(void* argument) {
  struct MyGame* game = (struct MyGame*)argument;
  struct MyXData* xData = game->xData;

  // Begin the main loop. 
  while (TRUE) {
    // Update the window.
    MyXData_update(xData);

    // Update the game data.
    BOOL status = MyGame_update(game);
    if (!status) {
      break;
    }

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

  pthread_t mainThread;
  pthread_create(&mainThread, NULL, loop, &game);
  pthread_join(mainThread, NULL);

  return 0;
}
