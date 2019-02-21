//-----------------------------------------------------------------------------
// Created on: 07 December 2013
//-----------------------------------------------------------------------------
// Copyright (c) 2013-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_TimeStamp.h>

// Windows includes
#include <windows.h>

//! Generates timestamp structure for the current time.
//! \return timestamp structure.
Handle(asiAlgo_TimeStamp) asiAlgo_TimeStampTool::Generate()
{
  time_t t = -1;
  time(&t);

  int internalCount = 0;

  // TODO: Windows ONLY (!!!)
  static LONG INTERNAL = 0;
  internalCount = (int) InterlockedIncrement(&INTERNAL);

  return new asiAlgo_TimeStamp(t, internalCount);
}

//! Converts the passed timestamp structure to the corresponding array of
//! integer values.
//! \param TS [in] timestamp structure to convert.
//! \return correspondent array of integer data chunks.
std::vector<int>
  asiAlgo_TimeStampTool::AsChunked(const Handle(asiAlgo_TimeStamp)& TS)
{
  std::vector<int> res;
  if ( TS->Time != -1 )
  {
    tm timeInfo;
    localtime_s(&timeInfo, &TS->Time);

    res.push_back(timeInfo.tm_sec);
    res.push_back(timeInfo.tm_min);
    res.push_back(timeInfo.tm_hour);
    res.push_back(timeInfo.tm_mday);
    res.push_back(timeInfo.tm_mon);
    res.push_back(timeInfo.tm_year);
    res.push_back(timeInfo.tm_wday);
    res.push_back(timeInfo.tm_yday);
    res.push_back(timeInfo.tm_isdst);
    res.push_back(TS->Internal);
  }
  else
  {
    for ( int i = 0; i < 10; ++i )
      res.push_back(-1);
  }

  return res;
}

//! Converts the passed array of integer data chunks to TimeStamp structure.
//! \param chunked [in] input array.
//! \return correspondent timestamp structure.
Handle(asiAlgo_TimeStamp)
  asiAlgo_TimeStampTool::FromChunked(const std::vector<int>& chunked)
{
  if ( chunked.size() != 10 )
    return new asiAlgo_TimeStamp();

  tm timeInfo;
  timeInfo.tm_sec   = chunked.at(0);
  timeInfo.tm_min   = chunked.at(1);
  timeInfo.tm_hour  = chunked.at(2);
  timeInfo.tm_mday  = chunked.at(3);
  timeInfo.tm_mon   = chunked.at(4);
  timeInfo.tm_year  = chunked.at(5);
  timeInfo.tm_wday  = chunked.at(6);
  timeInfo.tm_yday  = chunked.at(7);
  timeInfo.tm_isdst = chunked.at(8);
  time_t t = mktime(&timeInfo);

  return new asiAlgo_TimeStamp( t, chunked.at(9) );
}
