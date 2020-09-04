#include <stdio.h> // printf
#include <stdlib.h> // getenv, malloc
#include <unistd.h> // usleep

#include <X11/Xlib.h> // X*
#include <X11/XKBlib.h> // Xkb*
#include <X11/keysym.h> // XK_*

#include "types.h"
#include "xdata.h"

// Window/app properties.
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400
#define REFRESH_INTERVAL 1000
#define LINE_WIDTH 2

// Foreground object properties.
#define OBJECT_SPEED 1
#define OBJECT_WIDTH 100
#define OBJECT_HEIGHT 100

struct MyGameData {
  unsigned int keyDown;
  unsigned int keyUp;
};

int main() {
  struct MyXData myXData;
  MyXData_initialize(&myXData, WINDOW_WIDTH, WINDOW_HEIGHT);

  // Foreground object data.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;

  // Begin the main loop. 
  while (TRUE) {
    

    switch (keyEvent.type) {
      case KeyPress:
        keyDown = keySym;
        break;
      
      case KeyRelease:
        keyUp = keySym;
        break;
    }

    if (previousKeyDown != keyDown) {
      // Check for debounceable keys.
    }

    switch (keyDown) {
      case XK_Left:
        dx = -OBJECT_SPEED;
        dy = 0;
        break;
      
      case XK_Right:
        dx = +OBJECT_SPEED;
        dy = 0;
        break;
      
      case XK_Up:
        dx = 0;
        dy = -OBJECT_SPEED;
        break;
      
      case XK_Down:
        dx = 0;
        dy = +OBJECT_SPEED;
        break;
    }

    if (previousKeyUp != keyUp) {
      // If Q is pressed, then end the loop.
      if (keyUp == XK_q || keyUp == XK_Q) {
        break;
      }
    }

    switch (keyUp) {
      case XK_Left:
      case XK_Right:
        dx = 0;
        break;
      
      case XK_Up:
      case XK_Down:
        dy = 0;
        break;
    }
    keyUp = 0;

    // Update objects.
    if ((dx > 0) && (x + OBJECT_WIDTH < WINDOW_WIDTH)) {
      x += dx;
    } else if ((dx < 0) && (x > 0)) {
      x += dx;
    }

    if ((dy > 0) && (y + OBJECT_HEIGHT < WINDOW_HEIGHT)) {
      y += dy;
    } else if ((dy < 0) && (y > 0)) {
      y += dy;
    }

    // Begin drawing.
    XClearWindow(myXData.display, myXData.window);
    XSetBackground(myXData.display, myXData.context, black);
    XSetForeground(myXData.display, myXData.context, myXData.red.pixel);
    XSetLineAttributes(
      myXData.display,
      myXData.context,
      LINE_WIDTH,
      LineSolid,
      CapRound,
      JoinRound
    );
    XDrawLine(
      myXData.display,
      myXData.window,
      myXData.context,
      x,
      y,
      x + OBJECT_WIDTH,
      y + OBJECT_HEIGHT
    );

    usleep(REFRESH_INTERVAL);
  }

  MyXData_finalize(&myXData);

  printf("Goodbye.\n");

  return 0;
}

#undef WINDOW_WIDTH
#undef WINDOW_HEIGHT
#undef REFRESH_INTERVAL
#undef LINE_WIDTH
#undef OBJECT_SPEED
#undef OBJECT_WIDTH
#undef OBJECT_HEIGHT
