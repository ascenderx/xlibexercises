#pragma once

#include <X11/Xlib.h> // X*
#include <X11/XKBlib.h> // Xkb*

#include "types.h"

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
  KeySym keyDown;
  KeySym keyUp;
  KeySym previousKeyDown;
  KeySym previousKeyUp;
};

struct MyXData* MyXData_new();
void MyXData_initialize(struct MyXData* self);
void MyXData_update(struct MyXData* self);
void _MyXData_updatePreviousKeys(struct MyXData* self);
void _MyXData_suppressKeys(struct MyXData* self);
void _MyXData_onKey(struct MyXData* self);
void _MyXData_onConfigure(struct MyXData* self);
void _MyXData_onFocus(struct MyXData* self);
void MyXData_finalize(struct MyXData* self);

void initializeXColor(XColor* color, struct MyXData* xData, USHORT red, USHORT green, USHORT blue);
