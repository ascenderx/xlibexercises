#ifndef _XDATA_H
#define _XDATA_H

#include <X11/Xlib.h> // X*, KeySym

#include "types.h"

/**********
 * 
 **********/
enum MyKeyStatus {
  KEY_RELEASED,
  KEY_PRESSED,
  KEY_DEBOUNCED,
};

/**********
 * 
 **********/
struct MyKeys {
  enum MyKeyStatus keyA;
  enum MyKeyStatus keyD;
  enum MyKeyStatus keyP;
  enum MyKeyStatus keyQ;
  enum MyKeyStatus keyS;
  enum MyKeyStatus keyW;
  enum MyKeyStatus keyLeft;
  enum MyKeyStatus keyRight;
  enum MyKeyStatus keyUp;
  enum MyKeyStatus keyDown;
};

/**********
 * 
 **********/
enum MyButtonStatus {
  BUTTON_RELEASED,
  BUTTON_PRESSED,
  BUTTON_DEBOUNCED,
};

/**********
 * 
 **********/
struct MyMouse {
  int x;
  int y;
  bool hasMoved;
  enum MyButtonStatus button1;
  enum MyButtonStatus button2;
  enum MyButtonStatus button3;
};

/**********
 * 
 **********/
enum MyFocusStatus {
  FOCUS_OUT,
  FOCUS_IN,
  FOCUS_OUT_DEBOUNCED,
  FOCUS_IN_DEBOUNCED,
};

/**********
 * 
 **********/
struct MyWindow {
  Display* display;
  int screen;
  Window window;
  XWindowAttributes attributes;
  GC context;
  XColor black;
  XColor red;
  XColor white;
  XEvent event;
  struct MyKeys myKeys;
  struct MyMouse myMouse;
  enum MyFocusStatus focus;
  XFontStruct* font;
  ushort fontSize;
  bool didResize;
};

struct MyWindow* MyXData_new(void);
void MyWindow_initialize(struct MyWindow* self);
void _MyWindow_initializeFonts(struct MyWindow* self);
void _MyWindow_initializeColor(struct MyWindow* self, XColor* color, ushort red, ushort green, ushort blue);
void _MyWindow_initializeEvents(struct MyWindow* self);
void _MyWindow_initializeKeys(struct MyWindow* self);
void _MyWindow_initializeMouse(struct MyWindow* self);
void MyWindow_show(struct MyWindow* self);
void MyWindow_update(struct MyWindow* self);
void MyWindow_clear(struct MyWindow* self);
void MyWindow_setBackgroundColor(struct MyWindow* self, XColor* color);
void MyWindow_setForegroundColor(struct MyWindow* self, XColor* color);
void MyWindow_drawText(struct MyWindow* self, int x, int y, char* text, ...);
void MyWindow_drawPolygon(struct MyWindow* self, XPoint* vertices, uint vertexCount);
void MyWindow_drawRectangle(struct MyWindow* self, int x, int y, uint width, uint height);
void _MyWindow_onKey(struct MyWindow* self);
void _MyWindow_onMotion(struct MyWindow* self);
void _MyWindow_onLeave(struct MyWindow* self);
void _MyWindow_onEnter(struct MyWindow* self);
void _MyWindow_onConfigure(struct MyWindow* self);
void _MyWindow_onFocusIn(struct MyWindow* self);
void _MyWindow_onFocusOut(struct MyWindow* self);
void MyWindow_finalize(struct MyWindow* self);

#endif // _XDATA_H
