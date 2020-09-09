#include "web_demo/web_demo_doc_host_ui_handler.h"

#include <assert.h>

#include <atomic>
#include <string>

#include "base/macros.h"
#include "web_demo/web_demo_resource.h"

namespace {

enum Action {
  ACTION_CLOSE = 1000,
  ACTION_MOVE = 1001,
  ACTION_CHANGE_SIZE = 1002,
};

struct ActionInfo {
  Action action_id;
  const wchar_t* action_name;
} const kActionArray[] = {
  { ACTION_CLOSE, L"Close" },
  { ACTION_MOVE, L"Move" },
  { ACTION_CHANGE_SIZE, L"ChangeSize" },
};

void GenerateWidthAndHeight(DISPPARAMS* param, int* width, int* height) {
  assert(param);
  assert(width && height);

  if (param->cArgs != 2) {
    assert(0);
    return;
  }

  VARIANTARG width_arg = param->rgvarg[1];
  VARIANTARG height_arg = param->rgvarg[0];
  if ((width_arg.vt != VT_INT && width_arg.vt != VT_I4) ||
      (height_arg.vt != VT_INT && height_arg.vt != VT_I4)) {
    return;
  }

  *width = width_arg.intVal;
  *height = height_arg.intVal;
}

}  // namespace

// WebDemoDocHostUIHandler::ClientCall -----------------------------------------

class WebDemoDocHostUIHandler::ClientCall : public IDispatch {
 public:
  ClientCall();
  ~ClientCall();

  void SetHWND(HWND hwnd);

  // IUnknown
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
  ULONG STDMETHODCALLTYPE AddRef() override;
  ULONG STDMETHODCALLTYPE Release() override;

  // IDispatch
  HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override;
  HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo,
                                        LCID lcid,
                                        ITypeInfo** ppTInfo) override;
  HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid,
                                          OLECHAR FAR* FAR* rgszNames,
                                          UINT cNames,
                                          LCID lcid,
                                          DISPID FAR* rgDispId) override;
  HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember,
                                   REFIID riid,
                                   LCID lcid,
                                   WORD wFlags,
                                   DISPPARAMS* pDispParams,
                                   VARIANT* pVarResult,
                                   EXCEPINFO* pExcepInfo,
                                   UINT* puArgErr) override;

 private:
  HWND hwnd_;

  std::atomic_ulong ref_count_;
};

WebDemoDocHostUIHandler::ClientCall::ClientCall()
    : hwnd_(NULL),
      ref_count_(1) {
  OutputDebugString(L"WebDemo: ClientCall\n");
}

WebDemoDocHostUIHandler::ClientCall::~ClientCall() {
  OutputDebugString(L"WebDemo: ~ClientCall\n");
}

void WebDemoDocHostUIHandler::ClientCall::SetHWND(HWND hwnd) {
  hwnd_ = hwnd;
}

HRESULT STDMETHODCALLTYPE
WebDemoDocHostUIHandler::ClientCall::QueryInterface(REFIID riid,
                                                    void** ppvObject) {
  if (riid == IID_IDispatch) {
    *ppvObject = dynamic_cast<IDispatch*>(this);

    AddRef();
    return S_OK;
  }

  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE WebDemoDocHostUIHandler::ClientCall::AddRef() {
  ++ref_count_;
  return ref_count_;
}

ULONG STDMETHODCALLTYPE WebDemoDocHostUIHandler::ClientCall::Release() {
  --ref_count_;
  if (ref_count_ == 0) {
    delete this;
  }
  return ref_count_;
}

HRESULT STDMETHODCALLTYPE
WebDemoDocHostUIHandler::ClientCall::GetTypeInfoCount(UINT* pctinfo) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebDemoDocHostUIHandler::ClientCall::GetTypeInfo(UINT iTInfo,
                                                 LCID lcid,
                                                 ITypeInfo** ppTInfo) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebDemoDocHostUIHandler::ClientCall::GetIDsOfNames(REFIID riid,
                                                   OLECHAR** rgszNames,
                                                   UINT cNames,
                                                   LCID lcid,
                                                   DISPID* rgDispId) {
  for (uint32_t i = 0; i < cNames; ++i) {
    for (size_t j = 0; j < arraysize(kActionArray); ++j) {
      if (wcscmp(rgszNames[i], kActionArray[j].action_name) == 0) {
        rgDispId[i] = static_cast<int>(kActionArray[j].action_id);
        break;
      }
    }
  }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE
WebDemoDocHostUIHandler::ClientCall::Invoke(DISPID dispIdMember,
                                            REFIID riid,
                                            LCID lcid,
                                            WORD wFlags,
                                            DISPPARAMS* pDispParams,
                                            VARIANT* pVarResult,
                                            EXCEPINFO* pExcepInfo,
                                            UINT* puArgErr) {
  switch (dispIdMember) {
    case static_cast<int>(ACTION_CLOSE):
      if (::IsWindow(hwnd_)) {
        ::PostMessage(hwnd_, WM_CLOSE, 0, 0);
      }
      break;

    case static_cast<int>(ACTION_MOVE):
      if (::IsWindow(hwnd_)) {
        ::SendMessage(hwnd_, WM_SYSCOMMAND, 0xF012, 0);
      }
      break;

    case static_cast<int>(ACTION_CHANGE_SIZE): {
      if (!::IsWindow(hwnd_)) {
        break;
      }

      int width = 0;
      int height = 0;
      GenerateWidthAndHeight(pDispParams, &width, &height);

      assert(width > 0 && height > 0);
      ::PostMessage(hwnd_, IDM_CHANGE_SIZE,
                    static_cast<WPARAM>(width),
                    static_cast<WPARAM>(height));

      break;
    }

    default:
      break;
  }

  return S_OK;
}

// WebDemoDocHostUIHandler -----------------------------------------------------

WebDemoDocHostUIHandler::WebDemoDocHostUIHandler(HWND hwnd)
    : hwnd_(hwnd),
      client_call_(nullptr) {
}

WebDemoDocHostUIHandler::~WebDemoDocHostUIHandler() {
}

HRESULT STDMETHODCALLTYPE
WebDemoDocHostUIHandler::GetExternal(IDispatch** ppDispatch) {
  client_call_ = new ClientCall();
  client_call_->SetHWND(hwnd_);
  *ppDispatch = client_call_;

  return S_OK;
}
