#include "main_window.h"

#include <assert.h>

int WINAPI wWinMain(HINSTANCE, HINSTANCE, wchar_t*, int) {
  MainWindow main_window(960, 540);
  if (!main_window.Initialize()) {
    assert(false);
    return 0;
  }

  main_window.RunMessageLoop();
  return 0;
}
