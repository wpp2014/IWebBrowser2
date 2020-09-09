#include "web_demo/main_window.h"

#include <shlwapi.h>

#include <assert.h>

#include <string>

#include "base/strings/stringprintf.h"
#include "base/win/current_module.h"
#include "base/win/scoped_variant.h"
#include "iwebbrowser2/browser_control.h"
#include "iwebbrowser2/browser_feature.h"
#include "web_demo/constants.h"
#include "web_demo/web_demo_doc_host_ui_handler.h"
#include "web_demo/web_demo_resource.h"

using namespace ie;

namespace {

const int kDefaultWidth = 900;
const int kDefaultHeight = 550;

const wchar_t kAboutBlankURL[] = L"about:blank";

SIZE GetMonitorSize(HWND hwnd) {
  MONITORINFO monitor_info;
  monitor_info.cbSize = sizeof(monitor_info);
  GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                 &monitor_info);
  RECT desktop_rect = monitor_info.rcMonitor;

  SIZE monitor_size;
  monitor_size.cx = desktop_rect.right - desktop_rect.left;
  monitor_size.cy = desktop_rect.bottom - desktop_rect.top;
  return monitor_size;
}

bool IsAboutBlankURL(VARIANT*& url) {
  if (url && url->vt == VT_BSTR && url->bstrVal) {
    std::wstring url_w(url->bstrVal, ::SysStringLen(url->bstrVal));
    if (url_w == kAboutBlankURL) {
      return true;
    }
  }
  return false;
}

std::wstring GenerateDefaultURL() {
  wchar_t exe_path[MAX_PATH] = {L'\0'};
  if (GetModuleFileName(NULL, exe_path, MAX_PATH - 1) <= 0) {
    return std::wstring();
  }

  wchar_t* exe_name = PathFindFileName(exe_path);
  if (!exe_name) {
    return std::wstring();
  }

  std::wstring url = base::StringPrintf(L"res://%ls/web_demo.html", exe_name);
  return url;
}

}  // namespace

MainWindow::MainWindow()
    : hwnd_(NULL),
      browser_control_(NULL),
      doc_host_ui_handler_(NULL),
      browser_object_(NULL),
      main_thread_id_(0) {
  main_thread_id_ = ::GetCurrentThreadId();

  // 使用最新的IE内核
  ie::SetIEFeature();
}

MainWindow::~MainWindow() {
  if (browser_control_) {
    delete browser_control_;
  }

  ie::RemoveIEFeature();
}

int MainWindow::Initialize() {
  WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = MainWindow::WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = sizeof(LONG_PTR);
  wcex.hInstance = CURRENT_MODULE();
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszMenuName = NULL;
  wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
  wcex.lpszClassName = constant::kWindowClass;
  RegisterClassEx(&wcex);

  DWORD style = WS_POPUP | WS_SYSMENU | WS_MINIMIZE;
  hwnd_ = CreateWindowEx(0,
                         constant::kWindowClass,
                         constant::kWindowTitle,
                         style,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         0, 0,
                         NULL,
                         NULL,
                         CURRENT_MODULE(),
                         this);
  if (!hwnd_) {
    return 1;
  }
  ShowWindow(hwnd_, SW_HIDE);
  UpdateWindow(hwnd_);

  // 创建浏览器控件
  CreateBrowser();

  return 0;
}

void MainWindow::RunMessageLoop() {
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void MainWindow::BeforeNavigate2(IDispatch* pDisp,
                                 VARIANT*& url,
                                 VARIANT*& Flags,
                                 VARIANT*& TargetFrameName,
                                 VARIANT*& PostData,
                                 VARIANT*& Headers,
                                 VARIANT_BOOL*& Cancel) {
  OutputDebugString(L"WebDemo: BeforeNavigate2");
}

void MainWindow::DocumentComplete(IDispatch* pDisp, VARIANT*& url) {
}

void MainWindow::OnDestroy(HWND hwnd) {
  PostQuitMessage(0);
}

void MainWindow::OnSize(uint32_t type, const SIZE& size) {
  if (browser_control_) {
    RECT rect;
    GetClientRect(hwnd_, &rect);
    browser_control_->SetRect(rect);
  }
}

void MainWindow::OnChangeSize(int width, int height) {
  assert(width > 0 && height > 0);

  SIZE monitor_size = GetMonitorSize(hwnd_);
  const int x = (monitor_size.cx - width) / 2;
  const int y = (monitor_size.cy - height) / 2;

  ShowWindow(hwnd_, SW_SHOWNORMAL);
  SetWindowPos(hwnd_, HWND_TOP, x, y, width, height,
               SWP_NOZORDER | SWP_SHOWWINDOW);
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
      case WM_LBUTTONDOWN:
        SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, l_param);
        break;

      case WM_SIZE: {
        SIZE size;
        size.cx = LOWORD(l_param);
        size.cy = HIWORD(l_param);
        main_window->OnSize((uint32_t)w_param, size);
        break;
      }

      case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

      case WM_DESTROY:
        main_window->OnDestroy(hwnd);
        return 0;

      case IDM_CHANGE_SIZE: {
        int width = static_cast<int>(w_param);
        int height = static_cast<int>(l_param);
        main_window->OnChangeSize(width, height);
        return 0;
      }

      default:
        break;
    }
    return DefWindowProc(hwnd, msg, w_param, l_param);
  }
}

void MainWindow::CreateBrowser() {
  browser_control_ = new BrowserControl(hwnd_);
  browser_control_->SetEventHandler(this);

  doc_host_ui_handler_  = new WebDemoDocHostUIHandler(hwnd_);
  browser_control_->SetExternalUIHandler(doc_host_ui_handler_);

  RECT rect;
  GetClientRect(hwnd_, &rect);
  browser_control_->SetRect(rect);

  browser_object_ = browser_control_->GetBrowserObject();
  browser_object_->put_Silent(TRUE);

  std::wstring default_url = GenerateDefaultURL();
  if (default_url.empty()) {
    default_url = kAboutBlankURL;
  }
  base::win::ScopedVariant url(default_url.c_str());
  browser_object_->Navigate2(url.AsInput(), NULL, NULL, NULL, NULL);
}
