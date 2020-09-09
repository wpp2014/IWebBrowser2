﻿#include "iwebbrowser2/browser_control.h"

#include <atlbase.h>
#include <exdispid.h>
#include <shlwapi.h>

#include <assert.h>

#include <string>
#include <vector>

#include "base/win/scoped_comptr.h"
#include "base/win/scoped_variant.h"
#include "iwebbrowser2/browser_event_handler.h"

namespace ie {

BrowserControl::BrowserControl(HWND parent)
    : browser_wnd_(NULL),
      parent_(parent),
      ole_object_(NULL),
      ole_in_place_object_(NULL),
      web_browser2_(NULL),
      browser_event_handler_(nullptr),
      doc_host_ui_handler_(nullptr),
      rect_({0, 0, 0, 0}),
      cookie_(0),
      ref_count_(1) {
  assert(parent_);

  bool res = CreateBrowser();
  assert(res);

  base::win::ScopedVariant about_blank(L"about:blank");
  web_browser2_->Navigate2(about_blank.AsInput(), NULL, NULL, NULL, NULL);
}

BrowserControl::~BrowserControl() {
  RegisterEventHandler(false);
}

void BrowserControl::SetRect(const RECT& rect) {
  rect_ = rect;

  SIZEL size;
  size.cx = rect_.right - rect_.left;
  size.cy = rect_.bottom - rect_.top;
  ole_object_->SetExtent(DVASPECT_CONTENT, &size);

  if (ole_in_place_object_) {
    ole_in_place_object_->SetObjectRects(&rect_, &rect_);
  }
}

bool BrowserControl::CreateBrowser() {
  HRESULT hr = ::OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0,
                           this, this, (void**)&ole_object_);
  if (FAILED(hr)) {
    OutputDebugString(L"BrowserControl: Create OleObject Failed.");
    return false;
  }

  hr = ole_object_->SetClientSite(this);
  if (FAILED(hr)) {
    OutputDebugString(L"BrowserControl: OleObject SetClientSite Failed.");
    return false;
  }

  hr= OleSetContainedObject(ole_object_, TRUE);
  if (FAILED(hr)) {
    OutputDebugString(L"BrowserControl: OleObject OleSetContainedObject Failed.");
    return false;
  }

  hr = ole_object_->DoVerb(
      OLEIVERB_INPLACEACTIVATE, NULL, this, -1, parent_, &rect_);
  if (FAILED(hr)) {
    OutputDebugString(L"WebDemo: OleObject DoVerb Failed.");
    return false;
  }

  hr = ole_object_->QueryInterface(&web_browser2_);
  if (FAILED(hr)) {
    OutputDebugString(L"BrowserControl: OleObject QueryInterface Failed.");
    return false;
  }

  RegisterEventHandler(true);

  if (ole_in_place_object_) {
    hr = ole_in_place_object_->GetWindow(&browser_wnd_);
    if (FAILED(hr)) {
      OutputDebugString(L"BrowserControl: Get Browser Window Failed.");
      return false;
    }
  }

  return true;
}

bool BrowserControl::RegisterEventHandler(bool is_advise) {
  if (!ole_object_) {
    return false;
  }

  base::win::ScopedComPtr<IWebBrowser2> web_browser2;
  base::win::ScopedComPtr<IConnectionPointContainer> cpc;
  base::win::ScopedComPtr<IConnectionPoint> cp;

  HRESULT hr = ole_object_->QueryInterface(&web_browser2);
  if (FAILED(hr)) {
    return false;
  }

  hr = web_browser2->QueryInterface(&cpc);
  if (FAILED(hr)) {
    return false;
  }

  hr = cpc->FindConnectionPoint(DIID_DWebBrowserEvents2, &cp);
  if (FAILED(hr)) {
    return false;
  }

  if (is_advise) {
    hr = cp->Advise(static_cast<IDispatch*>(this), &cookie_);
  } else {
    hr = cp->Unadvise(cookie_);
  }

  return SUCCEEDED(hr);
}

HRESULT STDMETHODCALLTYPE BrowserControl::SaveObject() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::GetMoniker(DWORD dwAssign,
                           DWORD dwWhichMoniker,
                           __RPC__deref_out_opt IMoniker** ppmk) {
  if ((dwAssign == OLEGETMONIKER_ONLYIFTHERE) &&
      (dwWhichMoniker == OLEWHICHMK_CONTAINER)) {
    return E_FAIL;
  }
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::GetContainer(__RPC__deref_out_opt IOleContainer** ppContainer) {
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE BrowserControl::ShowObject() {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::OnShowWindow(BOOL fShow) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::RequestNewObjectLayout() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::CanInPlaceActivate() {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::OnInPlaceActivate() {
  OleLockRunning(ole_object_, TRUE, FALSE);
  ole_object_->QueryInterface(&ole_in_place_object_);
  ole_in_place_object_->SetObjectRects(&rect_, &rect_);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::OnUIActivate() {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::GetWindowContext(
    __RPC__deref_out_opt IOleInPlaceFrame** ppFrame,
    __RPC__deref_out_opt IOleInPlaceUIWindow** ppDoc,
    __RPC__out LPRECT lprcPosRect,
    __RPC__out LPRECT lprcClipRect,
    __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo) {
  *ppFrame = dynamic_cast<IOleInPlaceFrame*>(this);
  *ppDoc = NULL;

  *lprcPosRect = rect_;
  *lprcClipRect = *lprcPosRect;

  lpFrameInfo->fMDIApp = false;
  lpFrameInfo->hwndFrame = parent_;
  lpFrameInfo->haccel = NULL;
  lpFrameInfo->cAccelEntries = 0;

  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::Scroll(SIZE scrollExtant) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::OnUIDeactivate(BOOL fUndoable) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::OnInPlaceDeactivate() {
  browser_wnd_ = 0;
  ole_in_place_object_ = 0;

  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::DiscardUndoState() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::DeactivateAndUndo() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::OnPosRectChange(__RPC__in LPCRECT lprcPosRect) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::GetWindow(__RPC__deref_out_opt HWND* phwnd) {
  *phwnd = parent_;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::ContextSensitiveHelp(BOOL fEnterMode) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::InsertMenus(HMENU hmenuShared,
                            LPOLEMENUGROUPWIDTHS lpMenuWidths) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::SetMenu(HMENU hmenuShared,
                                                  HOLEMENU holemenu,
                                                  HWND hwndActiveObject) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::RemoveMenus(HMENU hmenuShared) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::SetStatusText(LPCOLESTR pszStatusText) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::EnableModeless(BOOL fEnable) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::TranslateAccelerator(LPMSG lpmsg,
                                                               WORD wID) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::GetBorder(LPRECT lprectBorder) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::SetBorderSpace(LPCBORDERWIDTHS pborderwidths) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::SetActiveObject(IOleInPlaceActiveObject* pActiveObject,
                                LPCOLESTR pszObjName) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::CreateStream(__RPC__in_string const OLECHAR* pwcsName,
                             DWORD grfMode,
                             DWORD reserved1,
                             DWORD reserved2,
                             __RPC__deref_out_opt IStream** ppstm) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::OpenStream(const OLECHAR* pwcsName,
                                                     void* reserved1,
                                                     DWORD grfMode,
                                                     DWORD reserved2,
                                                     IStream** ppstm) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::CreateStorage(const OLECHAR* pwcsName,
                                                        DWORD grfMode,
                                                        DWORD reserved1,
                                                        DWORD reserved2,
                                                        IStorage** ppstg) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::OpenStorage(const OLECHAR* pwcsName,
                                                      IStorage* pstgPriority,
                                                      DWORD grfMode,
                                                      SNB snbExclude,
                                                      DWORD reserved,
                                                      IStorage** ppstg) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::CopyTo(DWORD ciidExclude,
                                                 const IID* rgiidExclude,
                                                 SNB snbExclude,
                                                 IStorage* pstgDest) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::MoveElementTo(const OLECHAR* pwcsName,
                              IStorage* pstgDest,
                              const OLECHAR* pwcsNewName,
                              DWORD grfFlags) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::Commit(DWORD grfCommitFlags) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::Revert(void) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::EnumElements(DWORD reserved1,
                                                       void* reserved2,
                                                       DWORD reserved3,
                                                       IEnumSTATSTG** ppenum) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::DestroyElement(const OLECHAR* pwcsName) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::RenameElement(const OLECHAR* pwcsOldName,
                              const OLECHAR* pwcsNewName) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::SetElementTimes(const OLECHAR* pwcsName,
                                const FILETIME* pctime,
                                const FILETIME* patime,
                                const FILETIME* pmtime) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::SetClass(REFCLSID clsid) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE BrowserControl::SetStateBits(DWORD grfStateBits,
                                                       DWORD grfMask) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::Stat(STATSTG* pstatstg,
                                               DWORD grfStatFlag) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::GetTypeInfoCount(__RPC__out UINT* pctinfo) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::GetTypeInfo(UINT iTInfo,
                            LCID lcid,
                            __RPC__deref_out_opt ITypeInfo** ppTInfo) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE BrowserControl::GetIDsOfNames(
    __RPC__in REFIID riid,
    __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
    __RPC__in_range(0, 16384) UINT cNames,
    LCID lcid,
    __RPC__out_ecount_full(cNames) DISPID* rgDispId) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
BrowserControl::Invoke(_In_ DISPID dispIdMember,
                       _In_ REFIID riid,
                       _In_ LCID lcid,
                       _In_ WORD wFlags,
                       _In_ DISPPARAMS* pDispParams,
                       _Out_opt_ VARIANT* pVarResult,
                       _Out_opt_ EXCEPINFO* pExcepInfo,
                       _Out_opt_ UINT* puArgErr) {
  if (dispIdMember == DISPID_VALUE &&
      (wFlags & DISPATCH_PROPERTYGET) != 0 &&
      pVarResult) {
    ::VariantInit(pVarResult);
    pVarResult->vt = VT_BSTR;
    pVarResult->bstrVal = ::SysAllocString(L"[WebBrowser Object]");
    return S_OK;
  } else if (dispIdMember != DISPID_UNKNOWN) {
    OnInvoke(dispIdMember, pDispParams);
    return S_OK;
  }

  if (pVarResult) {
    pVarResult->vt = VT_EMPTY;
  }
  return S_OK;
}

ULONG STDMETHODCALLTYPE BrowserControl::AddRef() {
  ++ref_count_;
  return ref_count_;
}

ULONG STDMETHODCALLTYPE BrowserControl::Release() {
  --ref_count_;
  if (ref_count_ == 0) {
    delete this;
  }
  return ref_count_;
}

HRESULT STDMETHODCALLTYPE BrowserControl::QueryInterface(REFIID riid,
                                                         LPVOID* ppvObject) {
  *ppvObject = NULL;

  if (riid == IID_IDocHostUIHandler && doc_host_ui_handler_) {
    *ppvObject = doc_host_ui_handler_;
    return S_OK;
  } else if (riid == IID_IOleClientSite) {
    *ppvObject = dynamic_cast<IOleClientSite*>(this);
  } else if (riid == IID_IOleInPlaceSite) {
    *ppvObject = dynamic_cast<IOleInPlaceSite*>(this);
  } else if (riid == IID_IOleInPlaceFrame) {
    *ppvObject = dynamic_cast<IOleInPlaceFrame*>(this);
  } else if (riid == IID_IStorage) {
    *ppvObject = dynamic_cast<IStorage*>(this);
  } else if (riid == IID_IDispatch) {
    *ppvObject = dynamic_cast<IDispatch*>(this);
  }

  if (*ppvObject) {
    AddRef();
    return S_OK;
  }
  return E_NOINTERFACE;
}

void BrowserControl::OnInvoke(DISPID dispIdMember, DISPPARAMS* pDispParams) {
  switch (dispIdMember) {
    case DISPID_BEFORENAVIGATE2:
      BeforeNavigate2(pDispParams);
      break;
    case DISPID_NAVIGATECOMPLETE2:
      NavigateComplete2(pDispParams);
      break;
    case DISPID_NAVIGATEERROR:
      NavigateError(pDispParams);
      break;
    case DISPID_DOCUMENTCOMPLETE:
      DocumentComplete(pDispParams);
      break;
    case DISPID_COMMANDSTATECHANGE:
      break;
    case DISPID_STATUSTEXTCHANGE:
      break;
    case DISPID_NEWWINDOW2:
      break;
    case DISPID_NEWWINDOW3:
      NewWindow3(pDispParams);
      break;
    default:
      break;
  }
}

void BrowserControl::BeforeNavigate2(DISPPARAMS* pDispParams) {
  if (!browser_event_handler_) {
    return;
  }
  browser_event_handler_->BeforeNavigate2(
      pDispParams->rgvarg[6].pdispVal, pDispParams->rgvarg[5].pvarVal,
      pDispParams->rgvarg[4].pvarVal, pDispParams->rgvarg[3].pvarVal,
      pDispParams->rgvarg[2].pvarVal, pDispParams->rgvarg[1].pvarVal,
      pDispParams->rgvarg[0].pboolVal);
}

void BrowserControl::NavigateComplete2(DISPPARAMS* pDispParams) {
  if (!browser_event_handler_) {
    return;
  }
  browser_event_handler_->NavigateComplete2(pDispParams->rgvarg[1].pdispVal,
                                            pDispParams->rgvarg[0].pvarVal);
}

void BrowserControl::NavigateError(DISPPARAMS* pDispParams) {
  if (!browser_event_handler_) {
    return;
  }
  browser_event_handler_->NavigateError(
      pDispParams->rgvarg[4].pdispVal, pDispParams->rgvarg[3].pvarVal,
      pDispParams->rgvarg[2].pvarVal, pDispParams->rgvarg[1].pvarVal,
      pDispParams->rgvarg[0].pboolVal);
}

void BrowserControl::DocumentComplete(DISPPARAMS* pDispParams) {
  if (!browser_event_handler_) {
    return;
  }
  browser_event_handler_->DocumentComplete(pDispParams->rgvarg[1].pdispVal,
                                           pDispParams->rgvarg[0].pvarVal);
}

void BrowserControl::CommandStateChange(DISPPARAMS* pDispParams) {
  if (!browser_event_handler_) {
    return;
  }
  browser_event_handler_->CommandStateChange(
      pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0].boolVal);
}

void BrowserControl::NewWindow3(DISPPARAMS* pDispParams) {
  if (!browser_event_handler_) {
    return;
  }
  browser_event_handler_->NewWindow3(
      pDispParams->rgvarg[4].ppdispVal, pDispParams->rgvarg[3].pboolVal,
      pDispParams->rgvarg[2].uintVal, pDispParams->rgvarg[1].bstrVal,
      pDispParams->rgvarg[0].bstrVal);
}

}  // namespace iwebbrowser2
