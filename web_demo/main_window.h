#ifndef WEB_DEMO_MAIN_WINDOW_H_
#define WEB_DEMO_MAIN_WINDOW_H_

#include <windows.h>
#include <exdisp.h>

// uint32_t
#include <stdint.h>

#include "base/macros.h"
#include "iwebbrowser2/browser_event_handler.h"

class WebDemoDocHostUIHandler;

namespace ie {
class BrowserControl;
}

class MainWindow : public ie::BrowserEventHandler {
 public:
  MainWindow();
  ~MainWindow() override;

  int Initialize();

  void RunMessageLoop();

 private:
  // iwebbrowser2::BrowserEventHandler
  void BeforeNavigate2(IDispatch* pDisp,
                       VARIANT*& url,
                       VARIANT*& Flags,
                       VARIANT*& TargetFrameName,
                       VARIANT*& PostData,
                       VARIANT*& Headers,
                       VARIANT_BOOL*& Cancel) override;
  void DocumentComplete(IDispatch* pDisp, VARIANT*& url) override;

  void OnDestroy(HWND hwnd);
  void OnSize(uint32_t type, const SIZE& size);
  void OnChangeSize(int width, int height);

  static LRESULT CALLBACK WndProc(
      HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param);

  void CreateBrowser();

  HWND hwnd_;

  ie::BrowserControl* browser_control_;

  WebDemoDocHostUIHandler* doc_host_ui_handler_;

  IWebBrowser2* browser_object_;

  DWORD main_thread_id_;

  DISALLOW_COPY_AND_ASSIGN(MainWindow);
};  // class MainWindow

#endif  // WEB_DEMO_MAIN_WINDOW_H_
