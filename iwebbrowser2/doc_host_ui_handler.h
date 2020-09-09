// 网页事件/界面处理接口

#ifndef IWEBBROWSER2_DOC_HOST_UI_HANDLER_H_
#define IWEBBROWSER2_DOC_HOST_UI_HANDLER_H_

#include <windows.h>
#include <mshtmhst.h>

#include <atomic>

namespace ie {

class DocHostUIHandler : public IDocHostUIHandler
                       , public IOleCommandTarget {
 public:
  DocHostUIHandler();
  virtual ~DocHostUIHandler() {}

  // IUnknown
  virtual ULONG STDMETHODCALLTYPE AddRef();
  virtual ULONG STDMETHODCALLTYPE Release();
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, PVOID *ppvObj);

  // IDocHostUIHandler
  virtual STDMETHODIMP ShowContextMenu(DWORD dwID,
                                       POINT* ppt,
                                       IUnknown* pcmdtReserved,
                                       IDispatch* pdispReserved) {
    // E_NOTIMPL: 正常弹出右键菜单，返回S_OK屏蔽右键菜单
    return S_OK;
  }
  virtual STDMETHODIMP GetHostInfo(DOCHOSTUIINFO* pInfo) {
    pInfo->cbSize = sizeof(DOCHOSTUIINFO);
    pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
    pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME |
                      DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE;
    return S_OK;
  }
  virtual STDMETHODIMP ShowUI(DWORD dwID,
                              IOleInPlaceActiveObject* pActiveObject,
                              IOleCommandTarget* pCommandTarget,
                              IOleInPlaceFrame* pFrame,
                              IOleInPlaceUIWindow* pDoc) {
    // return S_FALSE;
    return S_OK;
  }
  virtual STDMETHODIMP HideUI(void) {
    return S_OK;
  }
  virtual STDMETHODIMP UpdateUI(void) {
    return S_OK;
  }
  virtual STDMETHODIMP EnableModeless(BOOL fEnable) {
    // eturn S_OK;
    return E_NOTIMPL;
  }
  virtual STDMETHODIMP OnDocWindowActivate(BOOL fActivate) {
    // return S_OK;
    return E_NOTIMPL;
  }
  virtual STDMETHODIMP OnFrameWindowActivate(BOOL fActivate) {
    // return S_OK;
    return E_NOTIMPL;
  }
  virtual STDMETHODIMP ResizeBorder(LPCRECT prcBorder,
                                    IOleInPlaceUIWindow* pUIWindow,
                                    BOOL fRameWindow) {
    return S_OK;
  }
  virtual STDMETHODIMP TranslateAccelerator(LPMSG lpMsg,
                                            const GUID* pguidCmdGroup,
                                            DWORD nCmdID) {
    return S_FALSE;
  }
  virtual STDMETHODIMP GetOptionKeyPath(LPOLESTR* pchKey, DWORD dw) {
    return S_OK;
  }
  virtual STDMETHODIMP GetDropTarget(IDropTarget* pDropTarget,
                                     IDropTarget** ppDropTarget) {
    return E_NOTIMPL;
  }
  virtual STDMETHODIMP GetExternal(IDispatch** ppDispatch) {
    return E_NOTIMPL;
  }
  virtual STDMETHODIMP TranslateUrl(DWORD dwTranslate,
                                    OLECHAR* pchURLIn,
                                    OLECHAR** ppchURLOut) {
    return S_OK;
  }
  virtual STDMETHODIMP FilterDataObject(IDataObject* pDO,
                                        IDataObject** ppDORet) {
    return S_OK;
  }

  // IOleCommandTarget（主要用来处理JS脚本错误弹窗）
  virtual STDMETHODIMP QueryStatus(const GUID* pguidCmdGroup,
                                   ULONG cCmds,
                                   OLECMD prgCmds[],
                                   OLECMDTEXT* pCmdText);
  virtual STDMETHODIMP Exec(const GUID* pguidCmdGroup,
                            DWORD nCmdID,
                            DWORD nCmdexecopt,
                            VARIANTARG* pvarargIn,
                            VARIANTARG* pvarargOut);

 private:
  std::atomic_ulong ref_count_;
};  // class DocHostUIHandler

}  // namespace iwebbrowser2

#endif  // IWEBBROWSER2_DOC_HOST_UI_HANDLER_H_
