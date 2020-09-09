#ifndef WEB_DEMO_WEB_BROWSER_CORE_H_
#define WEB_DEMO_WEB_BROWSER_CORE_H_

#include <windows.h>
#include <exdisp.h>
#include <mshtmhst.h>
#include <oleidl.h>

#include <atomic>

#include "base/macros.h"

class WebBrowserCore : public IOleClientSite
                     , public IOleInPlaceSite
                     , public IStorage
                     , public IDispatch
                     , public IDocHostUIHandler {
 public:
  explicit WebBrowserCore(HWND parent);
  ~WebBrowserCore();

  int CreateWebBrowser();
  void DestroyWebBrowser();

  // IOleClientSite
  HRESULT STDMETHODCALLTYPE SaveObject() override;
  HRESULT STDMETHODCALLTYPE
  GetMoniker(DWORD dwAssign,
             DWORD dwWhichMoniker,
             __RPC__deref_out_opt IMoniker** ppmk) override;
  HRESULT STDMETHODCALLTYPE
  GetContainer(__RPC__deref_out_opt IOleContainer** ppContainer) override;
  HRESULT STDMETHODCALLTYPE ShowObject() override;
  HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow) override;
  HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void) override;

  // IOleInPlaceSite
  HRESULT STDMETHODCALLTYPE CanInPlaceActivate() override;
  HRESULT STDMETHODCALLTYPE OnInPlaceActivate() override;
  HRESULT STDMETHODCALLTYPE OnUIActivate() override;
  HRESULT STDMETHODCALLTYPE
  GetWindowContext(__RPC__deref_out_opt IOleInPlaceFrame** ppFrame,
                   __RPC__deref_out_opt IOleInPlaceUIWindow** ppDoc,
                   __RPC__out LPRECT lprcPosRect,
                   __RPC__out LPRECT lprcClipRect,
                   __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo) override;
  HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant) override;
  HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable) override;
  HWND GetControlWindow();
  HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() override;
  HRESULT STDMETHODCALLTYPE DiscardUndoState() override;
  HRESULT STDMETHODCALLTYPE DeactivateAndUndo() override;
  HRESULT STDMETHODCALLTYPE
  OnPosRectChange(__RPC__in LPCRECT lprcPosRect) override;

  // IStorage
  HRESULT STDMETHODCALLTYPE
  CreateStream(__RPC__in_string const OLECHAR* pwcsName,
               DWORD grfMode,
               DWORD reserved1,
               DWORD reserved2,
               __RPC__deref_out_opt IStream** ppstm) override;
  HRESULT STDMETHODCALLTYPE OpenStream(const OLECHAR* pwcsName,
                                       void* reserved1,
                                       DWORD grfMode,
                                       DWORD reserved2,
                                       IStream** ppstm) override;
  HRESULT STDMETHODCALLTYPE
  CreateStorage(__RPC__in_string const OLECHAR* pwcsName,
                DWORD grfMode,
                DWORD reserved1,
                DWORD reserved2,
                __RPC__deref_out_opt IStorage** ppstg) override;
  HRESULT STDMETHODCALLTYPE
  OpenStorage(__RPC__in_opt_string const OLECHAR* pwcsName,
              __RPC__in_opt IStorage* pstgPriority,
              DWORD grfMode,
              __RPC__deref_opt_in_opt SNB snbExclude,
              DWORD reserved,
              __RPC__deref_out_opt IStorage** ppstg) override;
  HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciidExclude,
                                   const IID* rgiidExclude,
                                   __RPC__in_opt SNB snbExclude,
                                   IStorage* pstgDest) override;
  HRESULT STDMETHODCALLTYPE
  MoveElementTo(__RPC__in_string const OLECHAR* pwcsName,
                __RPC__in_opt IStorage* pstgDest,
                __RPC__in_string const OLECHAR* pwcsNewName,
                DWORD grfFlags) override;
  HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) override;
  HRESULT STDMETHODCALLTYPE Revert() override;
  HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1,
                                         void* reserved2,
                                         DWORD reserved3,
                                         IEnumSTATSTG** ppenum) override;
  HRESULT STDMETHODCALLTYPE
  DestroyElement(__RPC__in_string const OLECHAR* pwcsName) override;
  HRESULT STDMETHODCALLTYPE
  RenameElement(__RPC__in_string const OLECHAR* pwcsOldName,
                __RPC__in_string const OLECHAR* pwcsNewName) override;
  HRESULT STDMETHODCALLTYPE
  SetElementTimes(__RPC__in_opt_string const OLECHAR* pwcsName,
                  __RPC__in_opt const FILETIME* pctime,
                  __RPC__in_opt const FILETIME* patime,
                  __RPC__in_opt const FILETIME* pmtime) override;
  HRESULT STDMETHODCALLTYPE SetClass(__RPC__in REFCLSID clsid) override;
  HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits,
                                         DWORD grfMask) override;
  HRESULT STDMETHODCALLTYPE Stat(__RPC__out STATSTG* pstatstg,
                                 DWORD grfStatFlag) override;

  // IDispatch
  HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override;
  HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo,
                                        LCID lcid,
                                        ITypeInfo** ppTInfo) override;
  HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid,
                                          LPOLESTR* rgszNames,
                                          UINT cNames,
                                          LCID lcid,
                                          DISPID* rgDispId) override;
  HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember,
                                   REFIID riid,
                                   LCID lcid,
                                   WORD wFlags,
                                   DISPPARAMS* Params,
                                   VARIANT* pVarResult,
                                   EXCEPINFO* pExcepInfo,
                                   UINT* puArgErr) override;

  // IDocHostUIHandler
  HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID,
                                            POINT* ppt,
                                            IUnknown* pcmdtReserved,
                                            IDispatch* pdispReserved) override;
  HRESULT STDMETHODCALLTYPE GetHostInfo(DOCHOSTUIINFO* pInfo) override;
  HRESULT STDMETHODCALLTYPE ShowUI(DWORD dwID,
                                   IOleInPlaceActiveObject* pActiveObject,
                                   IOleCommandTarget* pCommandTarget,
                                   IOleInPlaceFrame* pFrame,
                                   IOleInPlaceUIWindow* pDoc) override;
  HRESULT STDMETHODCALLTYPE HideUI() override;
  HRESULT STDMETHODCALLTYPE UpdateUI() override;
  HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable) override;
  HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL fActivate) override;
  HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL fActivate) override;
  HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT prcBorder,
                                         IOleInPlaceUIWindow* pUIWindow,
                                         BOOL fRameWindow) override;
  HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpMsg,
                                                 const GUID* pguidCmdGroup,
                                                 DWORD nCmdID) override;
  HRESULT STDMETHODCALLTYPE GetOptionKeyPath(LPOLESTR* pchKey,
                                             DWORD dw) override;
  HRESULT STDMETHODCALLTYPE GetDropTarget(IDropTarget* pDropTarget,
                                          IDropTarget** ppDropTarget) override;
  HRESULT STDMETHODCALLTYPE GetExternal(IDispatch** ppDispatch) override;
  HRESULT STDMETHODCALLTYPE TranslateUrl(DWORD dwTranslate,
                                         OLECHAR* pchURLIn,
                                         OLECHAR** ppchURLOut) override;
  HRESULT STDMETHODCALLTYPE FilterDataObject(IDataObject* pDO,
                                             IDataObject** ppDORet) override;

  // IUnknown
  ULONG STDMETHODCALLTYPE AddRef();
  ULONG STDMETHODCALLTYPE Release();
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppvObject);

 private:
  HWND GetBrowserWindow();

  HWND parent_;
  HWND browser_control_;

  IWebBrowser* web_browser2_;

  IOleObject* ole_object_;
  IOleInPlaceObject* ole_in_place_object_;

  std::atomic_ulong ref_count_;

  DISALLOW_COPY_AND_ASSIGN(WebBrowserCore);
};  // class WebBrowserCore

#endif  // WEB_DEMO_WEB_BROWSER_CORE_H_
