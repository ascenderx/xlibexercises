#include <stdio.h> // printf
#include <stdlib.h> // getenv, malloc

#include <X11/Xlib.h> // X*, KeySym
#include <X11/XKBlib.h> // Xkb*
#include <X11/keysym.h> // XK_*

#include "types.h"
#include "xdata.h"

// Window/app properties.
#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

struct MyWindow* MyXData_new() {
  return (struct MyWindow*)malloc(sizeof(struct MyWindow));
}

void MyWindow_initialize(struct MyWindow* self) {
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
  self->focus = FOCUS_IN;
  
  // Register events.
  XSelectInput(
    self->display,
    self->window,
    ExposureMask
    | ButtonPressMask
    | ButtonReleaseMask
    | KeyPressMask
    | KeyReleaseMask
    | StructureNotifyMask
    | FocusChangeMask
  );
  int dummy;
  XkbSetDetectableAutoRepeat(self->display, FALSE, &dummy);
  
  // Register keys.
  self->keys.keyA = KEY_RELEASED;
  self->keys.keyD = KEY_RELEASED;
  self->keys.keyP = KEY_RELEASED;
  self->keys.keyQ = KEY_RELEASED;
  self->keys.keyS = KEY_RELEASED;
  self->keys.keyW = KEY_RELEASED;
  self->keys.keyLeft = KEY_RELEASED;
  self->keys.keyRight = KEY_RELEASED;
  self->keys.keyUp = KEY_RELEASED;
  self->keys.keyDown = KEY_RELEASED;

  // Render the window.
  XMapRaised(self->display, self->window);
  XSync(self->display, FALSE);
}

void MyWindow_update(struct MyWindow* self) {
  XLockDisplay(self->display);

  // Poll events.
  XCheckWindowEvent(
    self->display,
    self->window,
    KeyPressMask
    | KeyReleaseMask
    | ButtonPressMask
    | ButtonReleaseMask
    | StructureNotifyMask
    | FocusChangeMask,
    &self->event
  );

  switch (self->event.type) {
    case KeyPress:
    case KeyRelease:
      _MyWindow_onKey(self);
      break;
    
    case ConfigureNotify:
      _MyWindow_onConfigure(self);
      break;
    
    case FocusIn:
      self->focus = (self->focus != FOCUS_IN_DEBOUNCED)
        ? FOCUS_IN
        : FOCUS_IN_DEBOUNCED;
      break;

    case FocusOut:
      self->focus = (self->focus != FOCUS_OUT_DEBOUNCED)
        ? FOCUS_OUT
        : FOCUS_OUT_DEBOUNCED;
      break;
  }  

  XUnlockDisplay(self->display);
}

void _MyWindow_onKey(struct MyWindow* self) {
  // Get most recent key press or release.
  self->event.xkey.state &= ~ControlMask;
  unsigned int keyCode = self->event.xkey.keycode;
  KeySym keySym = XkbKeycodeToKeysym(
    self->display,
    keyCode,
    0,
    self->event.xkey.state & ShiftMask ? 1 : 0
  );

  UBYTE* key = NULL;
  struct MyKeys* myKeys = &self->keys;
  switch (keySym) {
    case XK_a:
    case XK_A:
      key = &myKeys->keyA;
      break;
    
    case XK_d:
    case XK_D:
      key = &myKeys->keyD;
      break;
    
    case XK_p:
    case XK_P:
      key = &myKeys->keyP;
      break;
    
    case XK_q:
    case XK_Q:
      key = &myKeys->keyQ;
      break;
    
    case XK_s:
    case XK_S:
      key = &myKeys->keyS;
      break;
    
    case XK_w:
    case XK_W:
      key = &myKeys->keyW;
      break;
    
    case XK_Left:
      key = &myKeys->keyLeft;
      break;
    
    case XK_Right:
      key = &myKeys->keyRight;
      break;
    
    case XK_Up:
      key = &myKeys->keyUp;
      break;
    
    case XK_Down:
      key = &myKeys->keyDown;
      break;
  }
  
  if (key != NULL) {
    switch (self->event.type) {
      case KeyPress:
        *key = (*key != KEY_DEBOUNCED)
          ? KEY_PRESSED
          : KEY_DEBOUNCED;
        break;
      
      case KeyRelease:
        *key = KEY_RELEASED;
        break;
    }
  }
}

void _MyWindow_onConfigure(struct MyWindow* self) {
  if (self->event.xconfigure.width != self->windowWidth) {
    self->windowWidth = self->event.xconfigure.width;
  }
  if (self->event.xconfigure.height != self->windowHeight) {
    self->windowHeight = self->event.xconfigure.height;
  }
}

void MyWindow_finalize(struct MyWindow* self) {
  XLockDisplay(self->display);

  // Close the window and clean up.
  XFreeGC(self->display, self->context);
  XDestroyWindow(self->display, self->window);
  XCloseDisplay(self->display);
}

void initializeXColor(XColor* xColor, struct MyWindow* xData, USHORT red, USHORT green, USHORT blue) {
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
