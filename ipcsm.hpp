#pragma once
#ifndef IPCSM_HPP
// Inter-Process Communication with Shared Memory
struct IPCSM
{
  // this flag should be held false
  //  and set true as a signal
  // very crude, i know
  bool flag;
  //...
};

#define IPCSM_HPP
#endif // IPCSM_HPP
