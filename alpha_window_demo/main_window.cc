#include "alpha_window_demo/main_window.h"

#include <string>

#include <assert.h>

#include "base/win/current_module.h"
#include "base/win/scoped_variant.h"
#include "iwebbrowser2/browser_control.h"
#include "iwebbrowser2/browser_feature.h"

namespace {

const wchar_t kAboutBlankURL[] = L"about:blank";
const wchar_t kWindowTitle[] = L"Alpha Window Demo";
const wchar_t kWindowClass[] = L"Alpha Window Demo Class";

void GetMonitorSize(HWND hwnd, int* width, int* height) {
  assert(width && height);

  MONITORINFO monitor_info;
  monitor_info.cbSize = sizeof(monitor_info);
  GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                 &monitor_info);
  RECT desktop_rect = monitor_info.rcMonitor;

  *width = desktop_rect.right - desktop_rect.left;
  *height = desktop_rect.bottom - desktop_rect.top;
}

}  // namespace

MainWindow::MainWindow(int width, int height)
    : hwnd_(NULL),
      width_(width),
      height_(height),
      browser_object_(nullptr) {
  // 使用最新的IE内核
  ie::SetIEFeature();
}

MainWindow::~MainWindow() {
  browser_control_.reset();

  ie::RemoveIEFeature();
}

bool MainWindow::Initialize() {
  WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = MainWindow::WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = sizeof(LONG_PTR);
  wcex.hInstance = CURRENT_MODULE();
  // wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.hbrBackground = CreateSolidBrush(RGB(0xA1, 0xA1, 0xA1));
  wcex.lpszMenuName = NULL;
  wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
  wcex.lpszClassName = kWindowClass;
  RegisterClassEx(&wcex);

  DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  hwnd_ = CreateWindowEx(0,
                         kWindowClass,
                         kWindowTitle,
                         style,
                         0, 0,
                         width_, height_,
                         NULL,
                         NULL,
                         CURRENT_MODULE(),
                         this);
  if (!hwnd_) {
    return false;
  }

  // 将客户区的尺寸设置为width_ x height_
  {
    int window_width = 0, window_height = 0;
    int client_width = 0, client_height = 0;
    GetWindowSize(&window_width, &window_height);
    GetClientSize(&client_width, &client_height);

    int new_width = window_width - client_width + width_;
    int new_height = window_height - client_height + height_;
    SetWindowPos(hwnd_, HWND_TOP, 0, 0, new_width, new_height, SWP_NOMOVE | SWP_NOZORDER);
  }

  // 创建ie控件
  if (!CreateBrowserControl()) {
    assert(false);
    return false;
  }

  // 居中显示
  CenterWindow();

  ShowWindow(hwnd_, SW_SHOWNORMAL);
  UpdateWindow(hwnd_);

  return true;
}

void MainWindow::RunMessageLoop() {
  MSG msg = { 0 };
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void MainWindow::CenterWindow() {
  int monitor_width = 0, monitor_height = 0;
  int window_width = 0, window_height = 0;
  GetMonitorSize(hwnd_, &monitor_width, &monitor_height);
  GetWindowSize(&window_width, &window_height);

  int x = (monitor_width - window_width) / 2;
  int y = (monitor_height - window_height) / 2;
  SetWindowPos(hwnd_, HWND_TOP, x, y, window_width, window_height, SWP_NOSIZE | SWP_NOZORDER);
}

void MainWindow::BeforeNavigate2(IDispatch* i_dispatch,
                                 VARIANT*& url,
                                 VARIANT*& flags,
                                 VARIANT*& target_frame_name,
                                 VARIANT*& post_data,
                                 VARIANT*& headers,
                                 VARIANT_BOOL*& cancel) {
}

void MainWindow::DocumentComplete(IDispatch* i_dispatch, VARIANT*& url) {
  if (i_dispatch != browser_object_) {
    return;
  }

  if (url && url->vt == VT_BSTR && url->bstrVal) {
    std::wstring url_str(url->bstrVal, SysStringLen(url->bstrVal));
    if (url_str == kAboutBlankURL) {
      base::win::ScopedVariant new_url(L"http://172.18.15.219/html/iwebbrowser-demo/1.html");
      browser_object_->Navigate2(new_url.AsInput(), NULL, NULL, NULL, NULL);
    }
  }
}

// static
LRESULT CALLBACK MainWindow::WndProc(
    HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param) {
  if (msg == WM_CREATE) {
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)l_param;
    MainWindow* main_window = (MainWindow*)pcs->lpCreateParams;
    ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(main_window));
    return 1;
  } else {
    MainWindow* main_window = reinterpret_cast<MainWindow*>(
        static_cast<LONG_PTR>(::GetWindowLongPtr(hwnd, GWLP_USERDATA)));
    if (!main_window) {
      return DefWindowProc(hwnd, msg, w_param, l_param);
    }

    switch (msg) {
      case WM_DESTROY:
        main_window->OnDestroy();
        return 0;

      case WM_CLOSE:
        main_window->OnClose();
        return 0;

      case WM_SIZE:
        main_window->OnSize((uint32_t)LOWORD(l_param), (uint32_t)HIWORD(l_param));
        return 0;

      default:
        break;
    }
  }
  return DefWindowProc(hwnd, msg, w_param, l_param);
}

void MainWindow::OnDestroy() {
  PostQuitMessage(0);
}

void MainWindow::OnClose() {
  DestroyWindow(hwnd_);
}

void MainWindow::OnSize(uint32_t width, uint32_t height) {
  SetBrowserControlRect();
}

bool MainWindow::CreateBrowserControl() {
  browser_control_.reset(new ie::BrowserControl(hwnd_));
  browser_control_->SetEventHandler(this);

  browser_object_ = browser_control_->GetBrowserObject();
  browser_object_->put_Silent(TRUE);

  base::win::ScopedVariant url(kAboutBlankURL);
  browser_object_->Navigate2(url.AsInput(), NULL, NULL, NULL, NULL);

  return true;
}

void MainWindow::SetBrowserControlRect() {
  if (!browser_control_.get()) {
    return;
  }

  int client_width = 0, client_height = 0;
  GetClientSize(&client_width, &client_height);
  if (client_width <= 0 || client_height <= 0) {
    return;
  }

  const int ie_control_width = (int)(client_width * 0.75);
  const int ie_control_height = (int)(client_height * 0.75);
  const int x = (client_width - ie_control_width) / 2;
  const int y = (client_height - ie_control_height) / 2;

  RECT rect;
  rect.left = x;
  rect.top = y;
  rect.right = x + ie_control_width;
  rect.bottom = y + ie_control_height;
  browser_control_->SetRect(rect);
}

void MainWindow::GetWindowSize(int* width, int* height) {
  assert(IsWindow(hwnd_));
  assert(width && height);

  RECT rect;
  GetWindowRect(hwnd_, &rect);
  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

void MainWindow::GetClientSize(int* width, int* height) {
  assert(IsWindow(hwnd_));
  assert(width && height);

  RECT rect;
  GetClientRect(hwnd_, &rect);
  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}
