#include <stdio.h> // printf
#include <unistd.h> // usleep

#include <X11/Xlib.h> // X*
#include <X11/XKBlib.h> // Xkb*

#include "types.h"
#include "xdata.h"
#include "game.h"

#define REFRESH_INTERVAL 1000

int main() {
  struct MyXData xData;
  MyXData_initialize(&xData);
  
  struct MyGame game;
  MyGame_initialize(&game, &xData);

  // Begin the main loop. 
  while (TRUE) {
    MyXData_update(&xData);
    BOOL status = MyGame_update(&game);
    if (!status) {
      break;
    }
    MyGame_draw(&game);
    usleep(REFRESH_INTERVAL);
  }

  MyXData_finalize(&xData);
  printf("Goodbye.\n");
  return 0;
}

#undef REFRESH_INTERVAL
