// 浏览器事件处理接口

#ifndef IWEBBROWSER2_BROWSER_EVENT_HANDLER_H_
#define IWEBBROWSER2_BROWSER_EVENT_HANDLER_H_

#include <windows.h>

namespace ie {

class BrowserEventHandler {
 public:
  virtual ~BrowserEventHandler() {}

  // 请求URL之前触发
  virtual void BeforeNavigate2(IDispatch* pDisp,
                               VARIANT*& url,
                               VARIANT*& Flags,
                               VARIANT*& TargetFrameName,
                               VARIANT*& PostData,
                               VARIANT*& Headers,
                               VARIANT_BOOL*& Cancel) {}
  // 请求URL发生错误时触发
  virtual void NavigateError(IDispatch* pDisp,
                             VARIANT*& url,
                             VARIANT*& TargetFrameName,
                             VARIANT*& StatusCode,
                             VARIANT_BOOL*& Cancel) {}
  // 其中一个链接请求完成时触发
  virtual void NavigateComplete2(IDispatch* pDisp, VARIANT*& url) {}
  // 网页上下载进度发生变化时触发
  virtual void ProgressChange(LONG nProgress, LONG nProgressMax) {}
  // 网页上启用的命令发生变化时触发
  virtual void CommandStateChange(long Command, VARIANT_BOOL Enable) {}
  // 网页文档请求完成时触发
  virtual void DocumentComplete(IDispatch* pDisp, VARIANT*& url) {}
  // 网页请求打开一个新窗口/标签页时触发（例如<a href="https://www.baidu.com/" target="_blank"></a>）
  virtual void NewWindow3(IDispatch** ppDisp,
                          VARIANT_BOOL* Cancel,
                          DWORD dwFlags,
                          BSTR bstrUrlContext,
                          BSTR bstrUrl) {};
};  // class BrowserEventHandler

}  // namespace iwebbrowser2

#endif  // IWEBBROWSER2_BROWSER_EVENT_HANDLER_H_
