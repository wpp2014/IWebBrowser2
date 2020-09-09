#ifndef BASE_WIN_SCOPED_COMPTR_H_
#define BASE_WIN_SCOPED_COMPTR_H_

#include <assert.h>

namespace base {
namespace win {

template<class T>
class ScopedComPtr {
 public:
  ScopedComPtr() : ptr_(nullptr) {}
  explicit ScopedComPtr(T* ptr) : ptr_(ptr) {}
  ~ScopedComPtr() {
    if (ptr_) {
      ptr_->Release();
    }
  }

  bool Create(const CLSID clsid) {
    assert(!ptr_);
    if (ptr_) {
      return false;
    }
    HRESULT hr =
        CoCreateInstance(clsid, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&ptr_));
    return SUCCEEDED(hr);
  }

  T* Get() const { return ptr_; }

  operator T*() const { return ptr_; }

  T** operator&() { return &ptr_; }

  T* operator->() const { return ptr_; }

  T* operator=(T* new_ptr) {
    if (ptr_) {
      ptr_->Release();
    }
    return (ptr_ = new_ptr);
  }

 private:
  T* ptr_;
};  // class ScopedComPtr

}  // namespace win
}  // namespace base

#endif  // BASE_WIN_SCOPED_COMPTR_H_
