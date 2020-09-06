#pragma once

#include <X11/Xlib.h> // X*, KeySym

#include "types.h"

#define KEY_RELEASED 0
#define KEY_PRESSED 1
#define KEY_DEBOUNCED 2

struct MyKeys {
  UBYTE keyA;
  UBYTE keyD;
  UBYTE keyP;
  UBYTE keyQ;
  UBYTE keyS;
  UBYTE keyW;
  UBYTE keyLeft;
  UBYTE keyRight;
  UBYTE keyUp;
  UBYTE keyDown;
};

#define FOCUS_OUT 0
#define FOCUS_IN 1
#define FOCUS_OUT_DEBOUNCED 2
#define FOCUS_IN_DEBOUNCED 3

struct MyWindow {
  Display* display;
  int screen;
  Window window;
  GC context;
  int windowWidth;
  int windowHeight;
  XColor black;
  XColor red;
  XColor white;
  XEvent event;
  struct MyKeys keys;
  UBYTE focus;
};

struct MyWindow* MyXData_new();
void MyWindow_initialize(struct MyWindow* self);
void MyWindow_update(struct MyWindow* self);
void _MyWindow_onKey(struct MyWindow* self);
void _MyWindow_onConfigure(struct MyWindow* self);
void MyWindow_finalize(struct MyWindow* self);

void initializeXColor(XColor* color, struct MyWindow* xData, USHORT red, USHORT green, USHORT blue);
