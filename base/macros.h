#ifndef BASE_MACROS_H_
#define BASE_MACROS_H_

// size_t
#include <stddef.h>

// 禁用拷贝构造函数
#define DISALLOW_COPY(TypeName) \
  TypeName(const TypeName&) = delete

// 禁用拷贝赋值运算符
#define DISALLOW_ASSIGN(TypeName) \
  void operator=(const TypeName&) = delete

// 禁止拷贝构造函数和拷贝移动构造函数
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// 禁用默认构造函数、拷贝构造函数和移动赋值算符
// 如果一个类里面只有静态成员，使用这个可以避免构造对象
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                           \
  DISALLOW_COPY_AND_ASSIGN(TypeName)

// 计算数组元素个数
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

// 释放IUnKnow接口
template <class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
  if (*ppInterfaceToRelease != NULL) {
    (*ppInterfaceToRelease)->Release();

    (*ppInterfaceToRelease) = NULL;
  }
}

#endif  // BASE_MACROS_H_
