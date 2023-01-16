#pragma once

#include <string>
#include <string_view>
namespace HNx
{
// trims leading and trailing
//  whitespace from a wstring
std::wstring
trim_whitespace(std::wstring_view str)
{
  size_t first = str.find_first_not_of(L' ');
  if (first == std::wstring::npos)
    return {};
  size_t last = str.find_last_not_of(L' ');
  return std::wstring(str.substr(first, (last - first + 1)));
}

// for comparing wstrings case insensatively
bool icase_cmp_wchar(wchar_t a, wchar_t b)
{
  return std::tolower(a) == std::tolower(b);
}

}