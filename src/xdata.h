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

struct MyXData {
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

struct MyXData* MyXData_new();
void MyXData_initialize(struct MyXData* self);
void MyXData_update(struct MyXData* self);
void _MyXData_onKey(struct MyXData* self);
void _MyXData_onConfigure(struct MyXData* self);
void MyXData_finalize(struct MyXData* self);

void initializeXColor(XColor* color, struct MyXData* xData, USHORT red, USHORT green, USHORT blue);
