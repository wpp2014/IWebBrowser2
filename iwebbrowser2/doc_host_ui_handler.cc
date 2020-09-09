#include "iwebbrowser2/doc_host_ui_handler.h"

#include <shlwapi.h>

namespace ie {

DocHostUIHandler::DocHostUIHandler()
    : ref_count_(1) {
}

ULONG STDMETHODCALLTYPE DocHostUIHandler::AddRef() {
  ++ref_count_;
  return ref_count_;
}

ULONG STDMETHODCALLTYPE DocHostUIHandler::Release() {
  --ref_count_;
  if (ref_count_ == 0) {
    delete this;
  }
  return ref_count_;
}

HRESULT STDMETHODCALLTYPE DocHostUIHandler::QueryInterface(REFIID riid,
                                                           PVOID* ppvObj) {
  *ppvObj = NULL;

  if (riid == IID_IDocHostUIHandler) {
    *ppvObj = dynamic_cast<IDocHostUIHandler*>(this);
  } else if (riid == IID_IOleCommandTarget) {
    *ppvObj = dynamic_cast<IOleCommandTarget*>(this);
  }

  if (*ppvObj) {
    AddRef();
    return S_OK;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP DocHostUIHandler::QueryStatus(const GUID* pguidCmdGroup,
                                           ULONG cCmds,
                                           OLECMD prgCmds[],
                                           OLECMDTEXT* pCmdText) {
  HRESULT hres = OLECMDERR_E_UNKNOWNGROUP;
  if (IsEqualGUID(CGID_DocHostCommandHandler, *pguidCmdGroup)) {
    ULONG i;
    if (prgCmds == NULL) {
      return E_INVALIDARG;
    }

    for (i = 0; i < cCmds; i++) {
      switch (prgCmds[i].cmdID) {
        case OLECMDID_SHOWSCRIPTERROR:
        case OLECMDID_SHOWMESSAGE:
        case OLECMDID_SHOWFIND:
        case OLECMDID_SHOWPAGESETUP:
        case OLECMDID_SHOWPRINT:
        case OLECMDID_PROPERTIES:
          prgCmds[i].cmdf = OLECMDF_ENABLED;
          break;

        default:
          prgCmds[i].cmdf = 0;
          break;
      }
    }

    hres = S_OK;
  }

  return hres;
}

STDMETHODIMP DocHostUIHandler::Exec(const GUID* pguidCmdGroup,
                                    DWORD nCmdID,
                                    DWORD nCmdexecopt,
                                    VARIANTARG* pvarargIn,
                                    VARIANTARG* pvarargOut) {
  if (IsEqualGUID(CGID_DocHostCommandHandler, *pguidCmdGroup)) {
    switch (nCmdID) {
      case OLECMDID_SHOWSCRIPTERROR:
      case OLECMDID_SHOWMESSAGE:
        (*pvarargOut).vt = VT_BOOL;
        (*pvarargOut).boolVal = VARIANT_TRUE;
        return S_OK;
      default:
        return OLECMDERR_E_NOTSUPPORTED;
    }
  }
  return OLECMDERR_E_UNKNOWNGROUP;
}

}  // namespace iwebbrowser2
