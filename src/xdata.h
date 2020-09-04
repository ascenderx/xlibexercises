#pragma once

#include <X11/Xlib.h> // X*
#include <X11/XKBlib.h> // Xkb*

void initializeXColor(XColor* color, struct MyXData* xData, unsigned short red, unsigned short green, unsigned short blue);

struct MyXData {
  Display* display;
  int screen;
  Window window;
  GC context;
  XColor black;
  XColor red;
  XColor white;
  XEvent event;
  KeySym keyDown;
  KeySym keyUp;
};

struct MyXData* MyXData_new();
void MyXData_initialize(struct MyXData* self, unsigned int windowWidth, unsigned int windowHeight);
void MyXData_update(struct MyXData* self);
void MyXData_finalize(struct MyXData* self);
