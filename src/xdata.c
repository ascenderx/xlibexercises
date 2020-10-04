#include <stdarg.h> // va_*
#include <stdio.h> // printf
#include <stdlib.h> // getenv, malloc

#include <X11/Xlib.h> // X*, KeySym
#include <X11/XKBlib.h> // Xkb*
#include <X11/keysym.h> // XK_*
#include <X11/cursorfont.h> // XC_*

#include "xdata.h"

/**********
 * 
 **********/
struct MyWindow* MyXData_new(void) {
  return (struct MyWindow*)malloc(sizeof(struct MyWindow));
}

#define WINDOW_WIDTH_INITIAL 400
#define WINDOW_HEIGHT_INITIAL 400
#define WINDOW_BORDER_WIDTH 5
#define WINDOW_X 0
#define WINDOW_Y 0
#define GC_VALUE_MASK 0
#define GC_VALUES NULL

/**********
 * 
 **********/
void MyWindow_initialize(struct MyWindow* self) {
  // Initialize the display.
  const char* displayName = getenv("DISPLAY");
  self->display = XOpenDisplay(displayName);
  self->screen = DefaultScreen(self->display);
  
  _MyWindow_initializeColor(self, &self->black, 0x0000, 0x0000, 0x0000);
  _MyWindow_initializeColor(self, &self->red, 0xffff, 0x0000, 0x0000);
  _MyWindow_initializeColor(self, &self->white, 0xffff, 0xffff, 0xffff);

  // Initialize the window.
  const Window defaultRootWindow = DefaultRootWindow(self->display);
  self->window = XCreateSimpleWindow(
    self->display,
    defaultRootWindow,
    WINDOW_X,
    WINDOW_Y,
    WINDOW_WIDTH_INITIAL,
    WINDOW_HEIGHT_INITIAL,
    WINDOW_BORDER_WIDTH,
    self->white.pixel,
    self->black.pixel
  );
  self->context = XCreateGC(
    self->display,
    self->window,
    GC_VALUE_MASK,
    GC_VALUES
  );
  XGetWindowAttributes(self->display, self->window, &self->attributes);

  _MyWindow_initializeFonts(self);
  XSetForeground(self->display, self->context, self->white.pixel);
  XSetBackground(self->display, self->context, self->black.pixel);

  self->didResize = false;

  _MyWindow_initializeEvents(self);
}

#define FONT_NAME "-*-terminus-*-r-*-*-14-*-*-*-*-*-*-*"
#define FONT_SIZE 14
#define FONT_NAME_FALLBACK "fixed"
#define FONT_SIZE_FALLBACK 12

/**********
 * 
 **********/
void _MyWindow_initializeFonts(struct MyWindow* self) {
  self->font = XLoadQueryFont(self->display, FONT_NAME);
  self->fontSize = FONT_SIZE;
  if (!self->font) {
    printf("Unable to load font. Reverting to default.\n");
    self->font = XLoadQueryFont(self->display, FONT_NAME_FALLBACK);
    self->fontSize = FONT_SIZE_FALLBACK;
  }
  XSetFont(self->display, self->context, self->font->fid);
}

/**********
 * 
 **********/
void _MyWindow_initializeColor(struct MyWindow* self, XColor* color, ushort red, ushort green, ushort blue) {
  Colormap defaultColormap = XDefaultColormap(self->display, self->screen);

  color->red = red;
  color->green = green;
  color->blue = blue;
  color->flags = DoRed | DoGreen | DoBlue;
  XAllocColor(
    self->display,
    defaultColormap,
    color
  );
}

#define WINDOW_EVENT_MASK ExposureMask \
  | ButtonPressMask \
  | ButtonReleaseMask \
  | KeyPressMask \
  | KeyReleaseMask \
  | PointerMotionMask \
  | ButtonMotionMask \
  | Button1MotionMask \
  | Button2MotionMask \
  | Button3MotionMask \
  | EnterWindowMask \
  | LeaveWindowMask \
  | StructureNotifyMask \
  | FocusChangeMask
#define KEYBOARD_DETECTABLE true
#define KEYBOARD_SUPPORTED_RETURN NULL

/**********
 * 
 **********/
void _MyWindow_initializeEvents(struct MyWindow* self) {
  self->focus = FOCUS_IN;
  XSelectInput(
    self->display,
    self->window,
    WINDOW_EVENT_MASK
  );
  XkbSetDetectableAutoRepeat(
    self->display,
    KEYBOARD_DETECTABLE,
    KEYBOARD_SUPPORTED_RETURN
  );
  
  _MyWindow_initializeKeys(self);
  _MyWindow_initializeMouse(self);
}

#define KEYBOARD_OWNER_EVENTS true

/**********
 * 
 **********/
void _MyWindow_initializeKeys(struct MyWindow* self) {
  struct MyKeys* myKeys = &self->myKeys;

  XGrabKeyboard(
    self->display,
    self->window,
    KEYBOARD_OWNER_EVENTS,
    GrabModeAsync,
    GrabModeAsync,
    CurrentTime
  );

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

#define POINTER_OWNER_EVENTS true
#define POINTER_EVENT_MASK PointerMotionMask \
  | ButtonMotionMask \
  | Button1MotionMask \
  | Button2MotionMask \
  | Button3MotionMask \
  | EnterWindowMask \
  | LeaveWindowMask
#define CURSOR_STYLE XC_tcross

/**********
 * 
 **********/
void _MyWindow_initializeMouse(struct MyWindow* self) {
  struct MyMouse* myMouse = &self->myMouse;

  Cursor xCursor = XCreateFontCursor(self->display, CURSOR_STYLE);
  XDefineCursor(self->display, self->window, xCursor);
  XGrabPointer(
    self->display,
    self->window,
    POINTER_OWNER_EVENTS,
    POINTER_EVENT_MASK,
    GrabModeAsync,
    GrabModeAsync,
    self->window,
    xCursor,
    CurrentTime
  );

  myMouse->x = 0;
  myMouse->y = 0;
  // This motion flag will be overwritten during update.
  myMouse->hasMoved = false;
  myMouse->button1 = BUTTON_RELEASED;
  myMouse->button2 = BUTTON_RELEASED;
  myMouse->button3 = BUTTON_RELEASED;
}

#define SYNC_DISCARD false

/**********
 * 
 **********/
void MyWindow_show(struct MyWindow* self) {
  XMapRaised(self->display, self->window);
  XSync(self->display, SYNC_DISCARD);
}

#define KEYBOARD_EVENT_MASK KeyPressMask \
  | KeyReleaseMask \
  | ButtonPressMask \
  | ButtonReleaseMask \
  | PointerMotionMask \
  | ButtonMotionMask \
  | Button1MotionMask \
  | Button2MotionMask \
  | Button3MotionMask \
  | EnterWindowMask \
  | LeaveWindowMask \
  | StructureNotifyMask \
  | FocusChangeMask

/**********
 * 
 **********/
void MyWindow_update(struct MyWindow* self) {
  struct MyMouse* myMouse = &self->myMouse;
  
  XLockDisplay(self->display);

  // Get the next event.
  XCheckWindowEvent(
    self->display,
    self->window,
    KEYBOARD_EVENT_MASK,
    &self->event
  );

  self->didResize = false;

  // Assume no pointer movement until checked.
  myMouse->hasMoved = false;
  int oldMouseX = myMouse->x;
  int oldMouseY = myMouse->y;

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

  myMouse->hasMoved = (myMouse->x != oldMouseX) || (myMouse->y != oldMouseY);

  XUnlockDisplay(self->display);
}

/**********
 * 
 **********/
void MyWindow_clear(struct MyWindow* self) {
  XClearWindow(self->display, self->window);
}

/**********
 * 
 **********/
void MyWindow_setBackgroundColor(struct MyWindow* self, XColor* color) {
  XSetBackground(self->display, self->context, color->pixel);
}

/**********
 * 
 **********/
void MyWindow_setForegroundColor(struct MyWindow* self, XColor* color) {
  XSetForeground(self->display, self->context, color->pixel);
}

/**********
 * 
 **********/
void MyWindow_drawText(struct MyWindow* self, int x, int y, char* text, ...) {
  va_list arguments;

  // Format the text.
  char output[MAX_STRING_LENGTH];
  va_start(arguments, text);
  vsnprintf(output, MAX_STRING_LENGTH, text, arguments);
  va_end(arguments);

  // Get the text length.
  uint textLength;
  char* t;
  for (textLength = 0, t = output; *t; textLength++, t++) {
    /* Do nothing, just calculate. */
  }

  // Dummy variables needed for XTextExtents.
  int direction;
  int ascent;
  int descent;
  XCharStruct overall;

  XTextExtents(self->font, output, textLength, &direction, &ascent, &descent, &overall);
  XDrawString(self->display, self->window, self->context, x, y, output, textLength);
}

/**********
 * 
 **********/
void MyWindow_drawPolygon(struct MyWindow* self, XPoint* vertices, uint vertexCount) {
  XDrawLines(
    self->display,
    self->window,
    self->context,
    vertices,
    vertexCount,
    CoordModeOrigin
  );
}

/**********
 * 
 **********/
void MyWindow_drawRectangle(struct MyWindow* self, int x, int y, uint width, uint height) {
  XDrawRectangle(self->display, self->window, self->context, x, y, width, height);
}

/**********
 * 
 **********/
void _MyWindow_onKey(struct MyWindow* self) {
  struct MyKeys* myKeys = &self->myKeys;

  // Get most recent key press or release.
  XKeyEvent* keyEvent = &self->event.xkey;
  keyEvent->state &= ~ControlMask;
  unsigned int keyCode = keyEvent->keycode;
  unsigned int keyGroup = 0;
  unsigned int shiftLevel = (keyEvent->state & ShiftMask) ? 1 : 0;
  KeySym keySym = XkbKeycodeToKeysym(
    self->display,
    keyCode,
    keyGroup,
    shiftLevel
  );

  enum MyKeyStatus* key = NULL;
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

/**********
 * 
 **********/
void _MyWindow_onMotion(struct MyWindow* self) {
  XMotionEvent* motionEvent = &self->event.xmotion;
  struct MyMouse* myMouse = &self->myMouse;

  myMouse->x = motionEvent->x;
  myMouse->y = motionEvent->y;
}

/**********
 * 
 **********/
void _MyWindow_onLeave(struct MyWindow* self) {
  struct MyMouse* myMouse = &self->myMouse;

  myMouse->x = -1;
  myMouse->y = -1;
}

/**********
 * 
 **********/
void _MyWindow_onEnter(struct MyWindow* self) {
  XEnterWindowEvent* enterEvent = &self->event.xcrossing;
  struct MyMouse* myMouse = &self->myMouse;

  myMouse->x = enterEvent->x;
  myMouse->y = enterEvent->y; 
}

/**********
 * 
 **********/
void _MyWindow_onConfigure(struct MyWindow* self) {
  XWindowAttributes* attributes = &self->attributes;

  self->didResize = true;

  if (self->event.xconfigure.width != attributes->width) {
    attributes->width = self->event.xconfigure.width;
  }
  if (self->event.xconfigure.height != attributes->height) {
    attributes->height = self->event.xconfigure.height;
  }
}

/**********
 * 
 **********/
void _MyWindow_onFocusIn(struct MyWindow* self) {
  if (self->focus != FOCUS_IN_DEBOUNCED) {
    self->focus = FOCUS_IN;
  }
}

/**********
 * 
 **********/
void _MyWindow_onFocusOut(struct MyWindow* self) {
  if (self->focus != FOCUS_OUT_DEBOUNCED) {
    self->focus = FOCUS_OUT;
  }
}

/**********
 * 
 **********/
void MyWindow_finalize(struct MyWindow* self) {
  XLockDisplay(self->display);

  // Close the window and clean up.
  XFreeGC(self->display, self->context);
  XDestroyWindow(self->display, self->window);
  XCloseDisplay(self->display);
}
