#include <stdlib.h> // getenv, malloc

#include <X11/Xlib.h> // X*
#include <X11/XKBlib.h> // Xkb*

#include "types.h"
#include "xdata.h"

// Window/app properties.
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

struct MyXData* MyXData_new() {
  return (struct MyXData*)malloc(sizeof(struct MyXData));
}

void MyXData_initialize(struct MyXData* self) {
  // Initialize the display.
  const char* displayName = getenv("DISPLAY");
  self->display = XOpenDisplay(displayName);
  self->screen = DefaultScreen(self->display);
  
  // Initialize colors.
  initializeXColor(&self->black, self, 0x0000, 0x0000, 0x0000);
  initializeXColor(&self->red, self, 0xffff, 0x0000, 0x0000);
  initializeXColor(&self->white, self, 0xffff, 0xffff, 0xffff);

  // Initialize the window.
  self->windowWidth = WINDOW_WIDTH;
  self->windowHeight = WINDOW_HEIGHT;
  const Window defaultRootWindow = DefaultRootWindow(self->display);
  self->window = XCreateSimpleWindow(
    self->display,
    defaultRootWindow,
    0,
    0,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    5,
    self->white.pixel,
    self->black.pixel
  );
  self->context = XCreateGC(
    self->display,
    self->window,
    0,
    NULL
  );
  XSetForeground(self->display, self->context, self->white.pixel);
  XSetBackground(self->display, self->context, self->black.pixel);
  
  // Register events.
  XSelectInput(
    self->display,
    self->window,
    ExposureMask
    | ButtonPressMask
    | ButtonReleaseMask
    | KeyPressMask
    | KeyReleaseMask
    | ResizeRedirectMask
  );
  int dummy;
  XkbSetDetectableAutoRepeat(self->display, FALSE, &dummy);
  self->keyDown = 0;
  self->keyUp = 0;
  self->previousKeyDown = 0;
  self->previousKeyUp = 0;

  // Render the window.
  XMapRaised(self->display, self->window);
  XSync(self->display, FALSE);
}

void MyXData_update(struct MyXData* self) {
  // Poll events.
  XCheckWindowEvent(
    self->display,
    self->window,
    KeyPressMask
    | KeyReleaseMask
    | ButtonPressMask
    | ButtonReleaseMask,
    &self->event
  );  

  // Get most recent key press or release.
  self->event.xkey.state &= ~ControlMask;
  unsigned int keyCode = self->event.xkey.keycode;
  KeySym keySym = XkbKeycodeToKeysym(
    self->display,
    keyCode,
    0, self->event.xkey.state & ShiftMask ? 1 : 0
  );
  self->previousKeyDown = self->keyDown;
  self->previousKeyUp = self->keyUp;
  // self->keyDown = 0;
  // self->keyUp = 0;
  switch (self->event.xkey.type) {
    case KeyPress:
      self->keyDown = keySym;
      break;
    
    case KeyRelease:
      self->keyUp = keySym;
      break;
  }
}

void MyXData_finalize(struct MyXData* self) {
  // Close the window and clean up.
  XFreeGC(self->display, self->context);
  XDestroyWindow(self->display, self->window);
  XCloseDisplay(self->display);
}

void initializeXColor(XColor* xColor, struct MyXData* xData, USHORT red, USHORT green, USHORT blue) {
  Colormap defaultColormap = XDefaultColormap(xData->display, xData->screen);

  xColor->red = red;
  xColor->green = green;
  xColor->blue = blue;
  xColor->flags = DoRed | DoGreen | DoBlue;
  XAllocColor(
    xData->display,
    defaultColormap,
    xColor
  );
}

#undef WINDOW_WIDTH
#undef WINDOW_HEIGHT
