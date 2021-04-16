#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <windows.h>
#include <exdisp.h>

#include <stdint.h>

#include <memory>

#include "iwebbrowser2/browser_event_handler.h"

namespace ie {
class BrowserControl;
}

class MainWindow : public ie::BrowserEventHandler {
 public:
  explicit MainWindow(int width, int height);
  ~MainWindow();

  bool Initialize();

  void RunMessageLoop();

  void CenterWindow();

 private:
  // iwebbrowser2::BrowserEventHandler
  void BeforeNavigate2(IDispatch* i_dispatch,
                       VARIANT*& url,
                       VARIANT*& flags,
                       VARIANT*& target_frame_name,
                       VARIANT*& post_data,
                       VARIANT*& headers,
                       VARIANT_BOOL*& cancel) override;
  void DocumentComplete(IDispatch* i_dispatch, VARIANT*& url) override;

  MainWindow() = delete;
  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;

  static LRESULT CALLBACK WndProc(
      HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param);

  void OnDestroy();
  void OnClose();
  void OnSize(uint32_t width, uint32_t height);

  // 创建浏览器控件
  bool CreateBrowserControl();
  // 设置ie控件在客户区的尺寸
  void SetBrowserControlRect();

  void GetWindowSize(int* width, int* height);
  void GetClientSize(int* width, int* height);

  HWND hwnd_;

  int width_;
  int height_;

  std::unique_ptr<ie::BrowserControl> browser_control_;
  IWebBrowser2* browser_object_;
};  // class MainWindow

#endif  // MAIN_WINDOW_H_
