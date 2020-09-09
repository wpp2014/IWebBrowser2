// IWebBrowser2控件

#ifndef IWEBBROWSER2_BROWSER_CONTROL_H_
#define IWEBBROWSER2_BROWSER_CONTROL_H_

#include <windows.h>
#include <exdisp.h>
#include <mshtmhst.h>

#include <atomic>

#include "base/macros.h"

namespace ie {

class BrowserEventHandler;

class BrowserControl : public IOleClientSite
                     , public IOleInPlaceSite
                     , public IOleInPlaceFrame
                     , public IStorage
                     , public IDispatch {
 public:
  explicit BrowserControl(HWND parent);
  ~BrowserControl();

  // 设置尺寸
  void SetRect(const RECT& rect);

  // 设置消息处理对象
  void SetEventHandler(BrowserEventHandler* event_handler) {
    browser_event_handler_ = event_handler;
  }

  // 设置页面事件处理
  void SetExternalUIHandler(IDocHostUIHandler* handler) {
    doc_host_ui_handler_ = handler;
  }

  // 获取IWebBrowser2窗口句柄
  HWND GetBrowserWnd() const { return browser_wnd_; }
  // 获取IwebBrowser2对象
  IWebBrowser2* GetBrowserObject() const { return web_browser2_; }

 protected:
  bool CreateBrowser();

  bool RegisterEventHandler(bool is_advise);

  // IOleClientSite
  virtual HRESULT STDMETHODCALLTYPE SaveObject();
  virtual HRESULT STDMETHODCALLTYPE
  GetMoniker(DWORD dwAssign,
             DWORD dwWhichMoniker,
             __RPC__deref_out_opt IMoniker** ppmk);
  virtual HRESULT STDMETHODCALLTYPE
  GetContainer(__RPC__deref_out_opt IOleContainer** ppContainer);
  virtual HRESULT STDMETHODCALLTYPE ShowObject();
  virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
  virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();

  // IOleInPlaceSite
  virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate();
  virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate();
  virtual HRESULT STDMETHODCALLTYPE OnUIActivate();
  virtual HRESULT STDMETHODCALLTYPE
  GetWindowContext(__RPC__deref_out_opt IOleInPlaceFrame** ppFrame,
                   __RPC__deref_out_opt IOleInPlaceUIWindow** ppDoc,
                   __RPC__out LPRECT lprcPosRect,
                   __RPC__out LPRECT lprcClipRect,
                   __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo);
  virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant);
  virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
  virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate();
  virtual HRESULT STDMETHODCALLTYPE DiscardUndoState();
  virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo();
  virtual HRESULT STDMETHODCALLTYPE
  OnPosRectChange(__RPC__in LPCRECT lprcPosRect);

  // IOleWindow
  HRESULT STDMETHODCALLTYPE GetWindow(__RPC__deref_out_opt HWND* phwnd) override;
  HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode) override;

  // IOleInPlaceFrame
  virtual HRESULT STDMETHODCALLTYPE
  InsertMenus(__RPC__in HMENU hmenuShared,
              __RPC__inout LPOLEMENUGROUPWIDTHS lpMenuWidths);
  virtual HRESULT STDMETHODCALLTYPE SetMenu(__RPC__in HMENU hmenuShared,
                                            __RPC__in HOLEMENU holemenu,
                                            __RPC__in HWND hwndActiveObject);
  virtual HRESULT STDMETHODCALLTYPE RemoveMenus(__RPC__in HMENU hmenuShared);
  virtual HRESULT STDMETHODCALLTYPE
  SetStatusText(__RPC__in_opt LPCOLESTR pszStatusText);
  virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
  virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(__RPC__in LPMSG lpmsg,
                                                         WORD wID);

  // IOleInPlaceUIWindow
  virtual HRESULT STDMETHODCALLTYPE GetBorder(__RPC__out LPRECT lprectBorder);
  virtual HRESULT STDMETHODCALLTYPE
  RequestBorderSpace(__RPC__in_opt LPCBORDERWIDTHS pborderwidths);
  virtual HRESULT STDMETHODCALLTYPE
  SetBorderSpace(__RPC__in_opt LPCBORDERWIDTHS pborderwidths);
  virtual HRESULT STDMETHODCALLTYPE
  SetActiveObject(__RPC__in_opt IOleInPlaceActiveObject* pActiveObject,
                  __RPC__in_opt_string LPCOLESTR pszObjName);

  // IStorage
  virtual HRESULT STDMETHODCALLTYPE
  CreateStream(__RPC__in_string const OLECHAR* pwcsName,
               DWORD grfMode,
               DWORD reserved1,
               DWORD reserved2,
               __RPC__deref_out_opt IStream** ppstm);
  virtual HRESULT STDMETHODCALLTYPE OpenStream(const OLECHAR* pwcsName,
                                               void* reserved1,
                                               DWORD grfMode,
                                               DWORD reserved2,
                                               IStream** ppstm);
  virtual HRESULT STDMETHODCALLTYPE
  CreateStorage(__RPC__in_string const OLECHAR* pwcsName,
                DWORD grfMode,
                DWORD reserved1,
                DWORD reserved2,
                __RPC__deref_out_opt IStorage** ppstg);
  virtual HRESULT STDMETHODCALLTYPE
  OpenStorage(__RPC__in_opt_string const OLECHAR* pwcsName,
              __RPC__in_opt IStorage* pstgPriority,
              DWORD grfMode,
              __RPC__deref_opt_in_opt SNB snbExclude,
              DWORD reserved,
              __RPC__deref_out_opt IStorage** ppstg);
  virtual HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciidExclude,
                                           const IID* rgiidExclude,
                                           __RPC__in_opt SNB snbExclude,
                                           IStorage* pstgDest);
  virtual HRESULT STDMETHODCALLTYPE
  MoveElementTo(__RPC__in_string const OLECHAR* pwcsName,
                __RPC__in_opt IStorage* pstgDest,
                __RPC__in_string const OLECHAR* pwcsNewName,
                DWORD grfFlags);
  virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
  virtual HRESULT STDMETHODCALLTYPE Revert(void);
  virtual HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1,
                                                 void* reserved2,
                                                 DWORD reserved3,
                                                 IEnumSTATSTG** ppenum);
  virtual HRESULT STDMETHODCALLTYPE
  DestroyElement(__RPC__in_string const OLECHAR* pwcsName);
  virtual HRESULT STDMETHODCALLTYPE
  RenameElement(__RPC__in_string const OLECHAR* pwcsOldName,
                __RPC__in_string const OLECHAR* pwcsNewName);
  virtual HRESULT STDMETHODCALLTYPE
  SetElementTimes(__RPC__in_opt_string const OLECHAR* pwcsName,
                  __RPC__in_opt const FILETIME* pctime,
                  __RPC__in_opt const FILETIME* patime,
                  __RPC__in_opt const FILETIME* pmtime);
  virtual HRESULT STDMETHODCALLTYPE SetClass(__RPC__in REFCLSID clsid);
  virtual HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits,
                                                 DWORD grfMask);
  virtual HRESULT STDMETHODCALLTYPE Stat(__RPC__out STATSTG* pstatstg,
                                         DWORD grfStatFlag);

  // IDispatch
  HRESULT STDMETHODCALLTYPE GetTypeInfoCount(__RPC__out UINT* pctinfo) override;
  HRESULT STDMETHODCALLTYPE
  GetTypeInfo(UINT iTInfo,
              LCID lcid,
              __RPC__deref_out_opt ITypeInfo** ppTInfo) override;
  HRESULT STDMETHODCALLTYPE GetIDsOfNames(__RPC__in REFIID riid,
                                          __RPC__in_ecount_full(cNames)
                                              LPOLESTR* rgszNames,
                                          __RPC__in_range(0, 16384) UINT cNames,
                                          LCID lcid,
                                          __RPC__out_ecount_full(cNames)
                                              DISPID* rgDispId) override;
  HRESULT STDMETHODCALLTYPE Invoke(_In_ DISPID dispIdMember,
                                   _In_ REFIID riid,
                                   _In_ LCID lcid,
                                   _In_ WORD wFlags,
                                   _In_ DISPPARAMS* pDispParams,
                                   _Out_opt_ VARIANT* pVarResult,
                                   _Out_opt_ EXCEPINFO* pExcepInfo,
                                   _Out_opt_ UINT* puArgErr) override;

  // IUnknown
  ULONG STDMETHODCALLTYPE AddRef();
  ULONG STDMETHODCALLTYPE Release();
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppvObject);

 private:
  void OnInvoke(DISPID dispIdMember, DISPPARAMS* pDispParams);

  void BeforeNavigate2(DISPPARAMS* pDispParams);
  void NavigateComplete2(DISPPARAMS* pDispParams);
  void NavigateError(DISPPARAMS* pDispParams);
  void DocumentComplete(DISPPARAMS* pDispParams);
  void CommandStateChange(DISPPARAMS* pDispParams);
  void NewWindow3(DISPPARAMS* pDispParams);

  HWND browser_wnd_;
  HWND parent_;

  IOleObject* ole_object_;
  IOleInPlaceObject* ole_in_place_object_;

  IWebBrowser2* web_browser2_;

  // 事件处理对象
  BrowserEventHandler* browser_event_handler_;

  // doc host ui handler
  IDocHostUIHandler* doc_host_ui_handler_;

  RECT rect_;

  DWORD cookie_;

  std::atomic_ulong ref_count_;

  DISALLOW_COPY_AND_ASSIGN(BrowserControl);
};  // class BrowserControl

}  // namespace iwebbrowser2

#endif  // IWEBBROWSER2_BROWSER_CONTROL_H_
