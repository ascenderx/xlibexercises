#include <stdio.h> // printf
#include <stdlib.h> // getenv, malloc

#include <X11/Xlib.h> // X*, KeySym
#include <X11/XKBlib.h> // Xkb*
#include <X11/keysym.h> // XK_*
#include <X11/cursorfont.h> // XC_*

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

  _MyWindow_initializeEvents(self);
}

void _MyWindow_initializeEvents(struct MyWindow* self) {
  self->focus = FOCUS_IN;
  XSelectInput(
    self->display,
    self->window,
    ExposureMask
    | ButtonPressMask
    | ButtonReleaseMask
    | KeyPressMask
    | KeyReleaseMask
    | PointerMotionMask
    | ButtonMotionMask
    | Button1MotionMask
    | Button2MotionMask
    | Button3MotionMask
    | EnterWindowMask
    | LeaveWindowMask
    | StructureNotifyMask
    | FocusChangeMask
  );
  XkbSetDetectableAutoRepeat(self->display, FALSE, NULL);
  
  _MyWindow_initializeKeys(self);
  _MyWindow_initializeMouse(self);
}

void _MyWindow_initializeKeys(struct MyWindow* self) {
  struct MyKeys* myKeys = &self->myKeys;

  myKeys->keyA = KEY_RELEASED;
  myKeys->keyD = KEY_RELEASED;
  myKeys->keyP = KEY_RELEASED;
  myKeys->keyQ = KEY_RELEASED;
  myKeys->keyS = KEY_RELEASED;
  myKeys->keyW = KEY_RELEASED;
  myKeys->keyLeft = KEY_RELEASED;
  myKeys->keyRight = KEY_RELEASED;
  myKeys->keyUp = KEY_RELEASED;
  myKeys->keyDown = KEY_RELEASED;
}

void _MyWindow_initializeMouse(struct MyWindow* self) {
  struct MyMouse* myMouse = &self->myMouse;

  Cursor xCursor = XCreateFontCursor(self->display, XC_tcross);
  XDefineCursor(self->display, self->window, xCursor);

  myMouse->x = 0;
  myMouse->y = 0;
  // This motion flag will be overwritten during update.
  myMouse->hasMoved = FALSE;
  myMouse->button1 = BUTTON_RELEASED;
  myMouse->button2 = BUTTON_RELEASED;
  myMouse->button3 = BUTTON_RELEASED;
}

void MyWindow_show(struct MyWindow* self) {
  XMapRaised(self->display, self->window);
  XSync(self->display, FALSE);
}

void MyWindow_update(struct MyWindow* self) {
  struct MyMouse* myMouse = &self->myMouse;
  
  XLockDisplay(self->display);

  // Get the next event.
  XCheckWindowEvent(
    self->display,
    self->window,
    KeyPressMask
    | KeyReleaseMask
    | ButtonPressMask
    | ButtonReleaseMask
    | PointerMotionMask
    | ButtonMotionMask
    | Button1MotionMask
    | Button2MotionMask
    | Button3MotionMask
    | EnterWindowMask
    | LeaveWindowMask
    | StructureNotifyMask
    | FocusChangeMask,
    &self->event
  );

  // Assume no pointer movement until checked.
  myMouse->hasMoved = FALSE;

  switch (self->event.type) {
    case KeyPress:
    case KeyRelease:
      _MyWindow_onKey(self);
      break;
    
    case ConfigureNotify:
      _MyWindow_onConfigure(self);
      break;
    
    case FocusIn:
      _MyWindow_onFocusIn(self);
      break;

    case FocusOut:
      _MyWindow_onFocusOut(self);
      break;
    
    case MotionNotify:
      _MyWindow_onMotion(self);
      break;
    
    case LeaveNotify:
      _MyWindow_onLeave(self);
      break;
    
    case EnterNotify:
      _MyWindow_onEnter(self);
      break;
  }  

  XUnlockDisplay(self->display);
}

void _MyWindow_onKey(struct MyWindow* self) {
  struct MyKeys* myKeys = &self->myKeys;

  // Get most recent key press or release.
  XKeyEvent* keyEvent = &self->event.xkey;
  keyEvent->state &= ~ControlMask;
  unsigned int keyCode = keyEvent->keycode;
  KeySym keySym = XkbKeycodeToKeysym(
    self->display,
    keyCode,
    0,
    (keyEvent->state & ShiftMask) ? 1 : 0
  );

  UByte* key = NULL;
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
    switch (keyEvent->type) {
      case KeyPress:
        if (*key != KEY_DEBOUNCED) {
          *key = KEY_PRESSED;
        }
        break;
      
      case KeyRelease:
        *key = KEY_RELEASED;
        break;
    }
  }
}

void _MyWindow_onMotion(struct MyWindow* self) {
  XMotionEvent* motionEvent = &self->event.xmotion;
  struct MyMouse* myMouse = &self->myMouse;

  myMouse->hasMoved = TRUE;
  myMouse->x = motionEvent->x;
  myMouse->y = motionEvent->y;
}

void _MyWindow_onLeave(struct MyWindow* self) {
  struct MyMouse* myMouse = &self->myMouse;

  myMouse->hasMoved = TRUE;
  myMouse->x = -1;
  myMouse->y = -1;
}

void _MyWindow_onEnter(struct MyWindow* self) {
  XEnterWindowEvent* enterEvent = &self->event.xcrossing;
  struct MyMouse* myMouse = &self->myMouse;

  myMouse->hasMoved = TRUE;
  myMouse->x = enterEvent->x;
  myMouse->y = enterEvent->y; 
}

void _MyWindow_onConfigure(struct MyWindow* self) {
  if (self->event.xconfigure.width != self->windowWidth) {
    self->windowWidth = self->event.xconfigure.width;
  }
  if (self->event.xconfigure.height != self->windowHeight) {
    self->windowHeight = self->event.xconfigure.height;
  }
}

void _MyWindow_onFocusIn(struct MyWindow* self) {
  if (self->focus != FOCUS_IN_DEBOUNCED) {
    self->focus = FOCUS_IN;
  }
}

void _MyWindow_onFocusOut(struct MyWindow* self) {
  if (self->focus != FOCUS_OUT_DEBOUNCED) {
    self->focus = FOCUS_OUT;
  }
}

void MyWindow_finalize(struct MyWindow* self) {
  XLockDisplay(self->display);

  // Close the window and clean up.
  XFreeGC(self->display, self->context);
  XDestroyWindow(self->display, self->window);
  XCloseDisplay(self->display);
}

void initializeXColor(XColor* xColor, struct MyWindow* xData, UShort red, UShort green, UShort blue) {
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
