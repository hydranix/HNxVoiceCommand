#pragma once
#include <windows.h>

//   There can only be one instance
//  of this program running otherwise
//   there are conflicts using the
//  input device (Windows Only)

namespace HNx
{

constexpr auto SINGLE_INSTANCE_GUID {L"ccc2e2f1-0338-4006-8cbc-2c338c321856"};
inline
bool
isSingleInstance()
{
  static bool exists = true;
  if (!exists)
    return false;

  if (!CreateMutexW(nullptr, TRUE, SINGLE_INSTANCE_GUID) ||
      GetLastError() == ERROR_ACCESS_DENIED)
  {
    // cache result
    exists = false;
  }
  return false;
};
}