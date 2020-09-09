#include <windows.h>

#include <MsHTML.h>
#include <Exdisp.h>
#include <ExDispid.h>

#include "base/win/current_module.h"
#include "web_demo/main_window.h"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  OleInitialize(NULL);

  MainWindow main_window;
  int res = main_window.Initialize();
  if (res != 0) {
    MessageBox(NULL, L"Initialize failed.", L"Error", MB_OK);
    OleUninitialize();
    return -1;
  }
  main_window.RunMessageLoop();

  OleUninitialize();
  return 0;
}