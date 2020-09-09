#ifndef BASE_STRINGS_STRING_UTIL_H_
#define BASE_STRINGS_STRING_UTIL_H_

#include <stdarg.h>   // va_list

namespace base {

int vsnprintf(char* buffer, size_t size, const char* format, va_list arguments);
int vswprintf(wchar_t* buffer,
              size_t size,
              const wchar_t* format,
              va_list arguments);

bool IsWprintfFormatPortable(const wchar_t* format);

}  // namespace base

#endif  // BASE_STRINGS_STRING_UTIL_H_
