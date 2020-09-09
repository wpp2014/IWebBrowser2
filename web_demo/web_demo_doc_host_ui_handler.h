#ifndef WEB_DEMO_WEB_DEMO_DOC_HOST_UI_HANDLER_H_
#define WEB_DEMO_WEB_DEMO_DOC_HOST_UI_HANDLER_H_

#include "iwebbrowser2/doc_host_ui_handler.h"

class WebDemoDocHostUIHandler : public ie::DocHostUIHandler {
 public:
  explicit WebDemoDocHostUIHandler(HWND hwnd);
  ~WebDemoDocHostUIHandler() override;

  // IDocHostUIHandler
  HRESULT STDMETHODCALLTYPE GetExternal(IDispatch** ppDispatch) override;

 private:
  class ClientCall;

  ClientCall* client_call_;

  HWND hwnd_;
};  // class WebDemoDocHostUIHandler

#endif  // WEB_DEMO_WEB_DEMO_DOC_HOST_UI_HANDLER_H_
