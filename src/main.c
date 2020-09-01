#include <stdio.h> // 
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

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

#define MAX_MESSAGE_LENGTH 255

int main(/* int argc, char** argv */) {
  // Initialize the display.
  const unsigned int width = 400;
  const unsigned int height = 400;
  Display* display = XOpenDisplay(getenv("DISPLAY"));
  const int screen = DefaultScreen(display);
  const unsigned int black = BlackPixel(display, screen);
  const unsigned int white = WhitePixel(display, screen);
  const Window window = XCreateSimpleWindow(
    display,
    DefaultRootWindow(display),
    0,
    0,
    width,
    height,
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
  
  // Render the window.
  XMapRaised(display, window);

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

  char keyBuffer[1];

  // Begin the main loop. 
  while (TRUE) {
    // Poll events.
    XEvent keyEvent;
    XCheckWindowEvent(
      display,
      window,
      KeyPressMask | KeyReleaseMask,
      &keyEvent
    );

    // Begin drawing.
    XClearWindow(display, window);
    XSetBackground(display, context, black);
    XSetForeground(display, context, red.pixel);
    XDrawLine(
      display,
      window,
      context,
      0,
      0,
      100,
      100
    );
    XFlush(display);
  
    // Check for exit conditions.
    unsigned char keyDown = 0;
    unsigned char keyUp = 0;
    KeySym key;
    BOOL keyStatus = XLookupString(
      &keyEvent.xkey,
      keyBuffer,
      MAX_MESSAGE_LENGTH,
      &key,
      NULL
    );

    switch (keyEvent.type) {
      case KeyPress:
        if (keyStatus) {
          keyDown = keyBuffer[0]; 
        }
        break;

      case KeyRelease:
        if (keyStatus) {
          keyUp = keyBuffer[0];
        }
        break;
    }

    // Print the currently pressed key.
    if (keyDown > 0) {
      printf("You pressed the %c key.\n", keyDown);
    }

    // If "q" is pressed, then end the loop.
    if (keyUp == 'q') {
      break;
    }

    usleep(50000);
  }

  // Close the window and clean up.
  XFreeGC(display, context);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
}
