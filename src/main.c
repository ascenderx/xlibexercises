#include <stdio.h> // printf
#include <unistd.h> // usleep

#include <X11/Xlib.h> // X*
#include <X11/XKBlib.h> // Xkb*

#include "types.h"
#include "xdata.h"
#include "game.h"

#define REFRESH_INTERVAL 1000

int main() {
  // Initialize data.
  struct MyXData xData;
  MyXData_initialize(&xData);
  struct MyGame game;
  MyGame_initialize(&game, &xData);

  // Begin the main loop. 
  while (TRUE) {
    // Update the window.
    MyXData_update(&xData);

    // Update the game data.
    BOOL status = MyGame_update(&game);
    if (!status) {
      break;
    }

    // Draw the game.
    MyGame_draw(&game);

    // Sleep.
    usleep(REFRESH_INTERVAL);
  }

  // Clean up.
  MyXData_finalize(&xData);
  printf("Goodbye.\n");
  return 0;
}

#undef REFRESH_INTERVAL
