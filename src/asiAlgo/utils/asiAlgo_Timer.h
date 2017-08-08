//-----------------------------------------------------------------------------
// Created on: 23 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiAlgo_Timer_h
#define asiAlgo_Timer_h

// asiAlgo includes
#include <asiAlgo_MemChecker.h>

// OCCT includes
#pragma warning(push, 0)
#include <OSD_Timer.hxx>
#include <TCollection_AsciiString.hxx>
#pragma warning(pop)

/************************************************************************
                           MEASURING PERFORMANCE
 ************************************************************************/

//! Example:
//!
//! #ifdef TIMER_NEW
//!   TIMER_NEW
//!   TIMER_RESET
//!   TIMER_GO
//! #endif
//!
//! ... YOUR AD_ALGO GOES HERE ...
//!
//! #ifdef TIMER_NEW
//!   TIMER_FINISH
//!   TIMER_COUT_RESULT
//! #endif

#define TIMER_NEW \
  OSD_Timer __aux_debug_Timer; \
  double __aux_debug_Seconds, __aux_debug_CPUTime; \
  int __aux_debug_Minutes, __aux_debug_Hours; \
  int __aux_debug_memcheck_before, __aux_debug_memcheck_after;

#define TIMER_RESET \
  __aux_debug_Seconds = __aux_debug_CPUTime = 0.0; \
  __aux_debug_Minutes = __aux_debug_Hours = 0; \
  __aux_debug_Timer.Reset();

#define TIMER_GO \
  MEMCHECK_COUNT_MIB(__aux_debug_memcheck_before) \
  __aux_debug_Timer.Start();

#define TIMER_FINISH \
  MEMCHECK_COUNT_MIB(__aux_debug_memcheck_after) \
  __aux_debug_Timer.Stop(); \
  __aux_debug_Timer.Show(__aux_debug_Seconds, __aux_debug_Minutes, __aux_debug_Hours, __aux_debug_CPUTime);

#define TIMER_COUT_RESULT \
  { \
    TIMER_COUT_RESULT_MSG(""); \
  }

#define TIMER_COUT_RESULT_MSG(Msg) \
  { \
    std::cout << "\n=============================================" << std::endl; \
    TCollection_AsciiString ascii_msg(Msg); \
    if ( !ascii_msg.IsEmpty() ) \
    { \
      std::cout << Msg                                             << std::endl; \
      std::cout << "---------------------------------------------" << std::endl; \
    } \
    std::cout << "Seconds:  " << __aux_debug_Seconds               << std::endl; \
    std::cout << "Minutes:  " << __aux_debug_Minutes               << std::endl; \
    std::cout << "Hours:    " << __aux_debug_Hours                 << std::endl; \
    std::cout << "CPU time: " << __aux_debug_CPUTime               << std::endl; \
    std::cout << "=============================================\n" << std::endl; \
  }

#define TIMER_COUT_RESULT_NOTIFIER(Notifier, Msg) \
  { \
    Notifier->SendLogMessage(LogInfo(Normal) << "============================================="); \
    Notifier->SendLogMessage(LogInfo(Normal) << "%1" << Msg); \
    Notifier->SendLogMessage(LogInfo(Normal) << "---------------------------------------------"); \
    Notifier->SendLogMessage(LogInfo(Normal) << "\tElapsed time (seconds):  %1"     << __aux_debug_Seconds); \
    Notifier->SendLogMessage(LogInfo(Normal) << "\tElapsed time (minutes):  %1"     << __aux_debug_Minutes); \
    Notifier->SendLogMessage(LogInfo(Normal) << "\tElapsed time (hours):    %1"     << __aux_debug_Hours); \
    Notifier->SendLogMessage(LogInfo(Normal) << "\tElapsed time (CPU time): %1"     << __aux_debug_CPUTime); \
    Notifier->SendLogMessage(LogInfo(Normal) << "\tMemory before:           %1 MiB" << __aux_debug_memcheck_before); \
    Notifier->SendLogMessage(LogInfo(Normal) << "\tMemory after [delta]:    %1 MiB [%2]" \
                                             << __aux_debug_memcheck_after \
                                             << (__aux_debug_memcheck_after - __aux_debug_memcheck_before) ); \
    Notifier->SendLogMessage(LogInfo(Normal) << "============================================="); \
  }

#endif
