#ifndef BASE_STRINGS_STRINGPRINTF_H_
#define BASE_STRINGS_STRINGPRINTF_H_

#include <stdarg.h>   // va_list

#include <string>

namespace base {

std::string StringPrintf(_Printf_format_string_ const char* format, ...);
std::wstring StringPrintf(_Printf_format_string_ const wchar_t* format, ...);

void StringAppendV(std::string* dst, const char* format, va_list ap);
void StringAppendV(std::wstring* dst, const wchar_t* format, va_list ap);

}  // namespace

#endif  // BASE_STRINGS_STRINGPRINTF_H_
