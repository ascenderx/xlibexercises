#include <stdio.h> // printf
#include <stdlib.h> // getenv
#include <unistd.h> // usleep

#include <X11/Xlib.h> // X*
#include <X11/XKBlib.h> // Xkb*
#include <X11/keysym.h> // XK_*

#ifndef BOOL
typedef unsigned char BOOL;
#endif // ifndef BOOL

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif // ifndef TRUE

#ifndef NULL
#define NULL (0)
#endif // ifndef NULL

// Window/app properties.
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400
#define REFRESH_INTERVAL 1000

// Foreground object properties.
#define OBJECT_SPEED 1
#define OBJECT_WIDTH 100
#define OBJECT_HEIGHT 100

int main() {
  // Initialize the display.
  Display* display = XOpenDisplay(getenv("DISPLAY"));
  const int screen = DefaultScreen(display);
  const unsigned int black = BlackPixel(display, screen);
  const unsigned int white = WhitePixel(display, screen);
  const Window window = XCreateSimpleWindow(
    display,
    DefaultRootWindow(display),
    0,
    0,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    5,
    white,
    black
  );
  const GC context = XCreateGC(
    display,
    window,
    0,
    NULL
  );
  XSetForeground(display, context, white);
  XSetBackground(display, context, black);
  
  // Register events.
  XSelectInput(
    display,
    window,
    ExposureMask
    | ButtonPressMask
    | ButtonReleaseMask
    | KeyPressMask
    | KeyReleaseMask
  );
  int dummy;
  XkbSetDetectableAutoRepeat(display, FALSE, &dummy);
  KeySym previousKeyDown = 0;
  KeySym keyDown = 0;
  KeySym previousKeyUp = 0;
  KeySym keyUp = 0;

  // Render the window.
  XMapRaised(display, window);
  XSync(display, FALSE);

  // Initialize colors.
  XColor red;
  red.red = 0xffff;
  red.green = 0x0000;
  red.blue = 0x0000;
  red.flags = DoRed | DoGreen | DoBlue;
  XAllocColor(
    display,
    DefaultColormap(display, screen),
    &red
  );

  // Foreground object data.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;

  // Begin the main loop. 
  while (TRUE) {
    // Poll events.
    XEvent event;
    XCheckWindowEvent(
      display,
      window,
      KeyPressMask | KeyReleaseMask,
      &event
    );
    XKeyEvent keyEvent = event.xkey;
    keyEvent.state &= ~ControlMask;

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
    XClearWindow(display, window);
    XSetBackground(display, context, black);
    XSetForeground(display, context, red.pixel);
    XSetLineAttributes(
      display,
      context,
      2,
      LineSolid,
      CapRound,
      JoinRound
    );
    XDrawLine(
      display,
      window,
      context,
      x,
      y,
      x + OBJECT_WIDTH,
      y + OBJECT_HEIGHT
    );
    XFlush(display);
  
    // Check for key presses and releases.
    previousKeyDown = keyDown;
    previousKeyUp = keyUp;
    unsigned int keyCode = keyEvent.keycode;
    KeySym keySym = XkbKeycodeToKeysym(
      display,
      keyCode,
      0, keyEvent.state & ShiftMask ? 1 : 0
    );

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

    usleep(REFRESH_INTERVAL);
  }

  // Close the window and clean up.
  XFreeGC(display, context);
  XDestroyWindow(display, window);
  XCloseDisplay(display);

  printf("Goodbye.\n");

  return 0;
}
