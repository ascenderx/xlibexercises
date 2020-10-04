#include <stdio.h> // printf
#include <unistd.h> // usleep
#include <pthread.h> // pthread*

#include "types.h"
#include "xdata.h"
#include "game.h"

#define REFRESH_INTERVAL 1000
#define START_MESSAGE \
  "Press W/A/S/D or Up/Left/Down/Right to move.\n" \
  "Press P to pause.\n" \
  "Press Q to quit.\n"

/**********
 * 
 **********/
void* loop(void* argument) {
  struct MyGame* myGame = (struct MyGame*)argument;
  struct MyWindow* myWindow = myGame->myWindow;

  // Begin the main loop. 
  while (true) {
    // Update the window.
    MyWindow_update(myWindow);

    // Update the game data.
    bool status = MyGame_handleInput(myGame);
    if (!status) {
      break;
    }
    MyGame_update(myGame);

    // Draw the game.
    MyGame_draw(myGame);

    // Sleep.
    usleep(REFRESH_INTERVAL);
  }

  // Clean up.
  MyWindow_finalize(myWindow);
  printf("Goodbye.\n");

  return NULL;
}

#define THREAD_ATTRIBUTES NULL
#define THREAD_RETURN NULL
#define EXIT_SUCCESS 0

/**********
 * 
 **********/
int main(void) {
  // Initialize data.
  XInitThreads();
  struct MyWindow myWindow;
  MyWindow_initialize(&myWindow);
  MyWindow_show(&myWindow);
  struct MyGame myGame;
  MyGame_initialize(&myGame, &myWindow);

  printf(START_MESSAGE);

  pthread_t mainThread;
  pthread_create(&mainThread, THREAD_ATTRIBUTES, loop, &myGame);
  pthread_join(mainThread, THREAD_RETURN);

  return EXIT_SUCCESS;
}
