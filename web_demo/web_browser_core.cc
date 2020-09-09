#include "web_demo/web_browser_core.h"

#include <shlwapi.h>

#include <assert.h>

WebBrowserCore::WebBrowserCore(HWND parent)
    : parent_(parent)
    , browser_control_(NULL)
    , web_browser2_(NULL)
    , ole_object_(NULL)
    , ole_in_place_object_(NULL)
    , ref_count_(1) {
}

WebBrowserCore::~WebBrowserCore() {
  SafeRelease(&ole_object_);
}

int WebBrowserCore::CreateWebBrowser() {
  assert(::IsWindow(parent_));

  HRESULT hr;
  hr = ::OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0, this, this, (void**)&ole_object_);
  if (FAILED(hr)) {
    OutputDebugString(L"WebBrowser: Cannot create oleObject CLSID_WebBrowser");
    return 1;
  }

  ole_object_->SetClientSite(this);
  OleSetContainedObject(ole_object_, TRUE);

  RECT parent_rect;
  ::GetClientRect(parent_, &parent_rect);
  hr = ole_object_->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, -1, parent_, &parent_rect);
  if (FAILED(hr)) {
    OutputDebugString(L"WebDemo: oleObject->DoVerb() failed");
    return 2;
  }

  hr = ole_object_->QueryInterface(&web_browser2_);
  if (FAILED(hr)) {
    OutputDebugString(L"WebDemo: ole_object_->QueryInterface() failed");
    return 3;
  }

  return 0;
}

void WebBrowserCore::DestroyWebBrowser() {
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::SaveObject() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetMoniker(DWORD dwAssign,
                                         DWORD dwWhichMoniker,
                                         IMoniker** ppmk) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetContainer(IOleContainer** ppContainer) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::ShowObject() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OnShowWindow(BOOL fShow) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::RequestNewObjectLayout(void) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::CanInPlaceActivate() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OnInPlaceActivate() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OnUIActivate() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetWindowContext(
    IOleInPlaceFrame** ppFrame,
    IOleInPlaceUIWindow** ppDoc,
    LPRECT lprcPosRect,
    LPRECT lprcClipRect,
    LPOLEINPLACEFRAMEINFO lpFrameInfo) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::Scroll(SIZE scrollExtant) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OnUIDeactivate(BOOL fUndoable) {
  return E_NOTIMPL;
}

HWND WebBrowserCore::GetControlWindow() {
  return HWND();
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OnInPlaceDeactivate() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::DiscardUndoState() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::DeactivateAndUndo() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OnPosRectChange(LPCRECT lprcPosRect) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::CreateStream(const OLECHAR* pwcsName,
                                           DWORD grfMode,
                                           DWORD reserved1,
                                           DWORD reserved2,
                                           IStream** ppstm) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OpenStream(const OLECHAR* pwcsName,
                                         void* reserved1,
                                         DWORD grfMode,
                                         DWORD reserved2,
                                         IStream** ppstm) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::CreateStorage(const OLECHAR* pwcsName,
                                            DWORD grfMode,
                                            DWORD reserved1,
                                            DWORD reserved2,
                                            IStorage** ppstg) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OpenStorage(const OLECHAR* pwcsName,
                                          IStorage* pstgPriority,
                                          DWORD grfMode,
                                          SNB snbExclude,
                                          DWORD reserved,
                                          IStorage** ppstg) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::CopyTo(DWORD ciidExclude,
                                     const IID* rgiidExclude,
                                     SNB snbExclude,
                                     IStorage* pstgDest) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::MoveElementTo(const OLECHAR* pwcsName,
                                            IStorage* pstgDest,
                                            const OLECHAR* pwcsNewName,
                                            DWORD grfFlags) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::Commit(DWORD grfCommitFlags) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::Revert() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::EnumElements(DWORD reserved1,
                                           void* reserved2,
                                           DWORD reserved3,
                                           IEnumSTATSTG** ppenum) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::DestroyElement(const OLECHAR* pwcsName) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::RenameElement(const OLECHAR* pwcsOldName,
                                            const OLECHAR* pwcsNewName) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::SetElementTimes(const OLECHAR* pwcsName,
                                              const FILETIME* pctime,
                                              const FILETIME* patime,
                                              const FILETIME* pmtime) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::SetClass(REFCLSID clsid) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::SetStateBits(DWORD grfStateBits, DWORD grfMask) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::Stat(STATSTG* pstatstg, DWORD grfStatFlag) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetTypeInfoCount(UINT* pctinfo) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetIDsOfNames(REFIID riid,
                                            LPOLESTR* rgszNames,
                                            UINT cNames,
                                            LCID lcid,
                                            DISPID* rgDispId) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::Invoke(DISPID dispIdMember,
                                     REFIID riid,
                                     LCID lcid,
                                     WORD wFlags,
                                     DISPPARAMS* Params,
                                     VARIANT* pVarResult,
                                     EXCEPINFO* pExcepInfo,
                                     UINT* puArgErr) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::ShowContextMenu(DWORD dwID,
                                              POINT* ppt,
                                              IUnknown* pcmdtReserved,
                                              IDispatch* pdispReserved) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetHostInfo(DOCHOSTUIINFO* pInfo) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::ShowUI(DWORD dwID,
                                     IOleInPlaceActiveObject* pActiveObject,
                                     IOleCommandTarget* pCommandTarget,
                                     IOleInPlaceFrame* pFrame,
                                     IOleInPlaceUIWindow* pDoc) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::HideUI() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::UpdateUI() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::EnableModeless(BOOL fEnable) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OnDocWindowActivate(BOOL fActivate) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::OnFrameWindowActivate(BOOL fActivate) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::ResizeBorder(LPCRECT prcBorder,
                                           IOleInPlaceUIWindow* pUIWindow,
                                           BOOL fRameWindow) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::TranslateAccelerator(LPMSG lpMsg,
                                                   const GUID* pguidCmdGroup,
                                                   DWORD nCmdID) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetOptionKeyPath(LPOLESTR* pchKey, DWORD dw) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetDropTarget(IDropTarget* pDropTarget,
                                            IDropTarget** ppDropTarget) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::GetExternal(IDispatch** ppDispatch) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::TranslateUrl(DWORD dwTranslate,
                                           OLECHAR* pchURLIn,
                                           OLECHAR** ppchURLOut) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::FilterDataObject(IDataObject* pDO,
                                               IDataObject** ppDORet) {
  return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE WebBrowserCore::AddRef() {
  ++ref_count_;
  return ref_count_;
}

ULONG STDMETHODCALLTYPE WebBrowserCore::Release() {
  --ref_count_;
  if (ref_count_ == 0) {
    delete this;
  }
  return ref_count_;
}

HRESULT STDMETHODCALLTYPE WebBrowserCore::QueryInterface(REFIID riid,
                                                         LPVOID* ppvObject) {
  static const QITAB qitable[] = {
    QITABENT(WebBrowserCore, IOleClientSite),
    QITABENT(WebBrowserCore, IOleInPlaceSite),
    QITABENT(WebBrowserCore, IStorage),
    QITABENT(WebBrowserCore, IDispatch),
    QITABENT(WebBrowserCore, IDocHostUIHandler),
    { 0 },
  };
  return QISearch(this, qitable, riid, ppvObject);
}

HWND WebBrowserCore::GetBrowserWindow() {
  if (browser_control_) {
    return browser_control_;
  }

  if (!ole_in_place_object_) {
    OutputDebugString(L"WebDemo: ole_in_place_object_ is NULL");
    return NULL;
  }

  HRESULT hr = ole_in_place_object_->GetWindow(&browser_control_);
  if (FAILED(hr)) {
    OutputDebugString(L"WebDemo: Get IWebBrowser HWND failed");
    return NULL;
  }
  return browser_control_;
}
