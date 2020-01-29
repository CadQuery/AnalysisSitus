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

#ifndef asiAlgo_TimeStamp_HeaderFile
#define asiAlgo_TimeStamp_HeaderFile

// asiAlgo includes
#include <asiAlgo_Utils.h>

// OCCT includes
#include <Standard_Type.hxx>

// STD includes
#include <time.h>

//-----------------------------------------------------------------------------
// TimeStamp structure
//-----------------------------------------------------------------------------

//! Utility class representing general-purpose timestamp down to the
//! seconds. Actually this class extends time_t value with addendum
//! integer value required for uniqueness (this integer is meaningless,
//! it has nothing related to timing).
class asiAlgo_TimeStamp : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_TimeStamp, Standard_Transient)

public:

  //! Time.
  time_t Time;

  //! Additional integer for uniqueness.
  int Internal;

public:

  //! Default constructor.
  asiAlgo_TimeStamp() : Standard_Transient(), Time(-1), Internal(0)
  {}

  //! Complete constructor.
  //! \param time [in] time to initialize the unique timestamp with.
  //! \param internalCount [in] additional integer value to provide
  //!        uniqueness of the generated timestamp.
  asiAlgo_TimeStamp(const time_t time, const int internalCount)
  : Standard_Transient(), Time(time), Internal(internalCount)
  {}

  //! Destructor.
  ~asiAlgo_TimeStamp()
  {}

  //! Checks whether this timestamp is null (not initialized).
  //! \return true/false.
  bool IsOrigin()
  {
    return (Time == -1) && (Internal == 0);
  }

  //! Checks whether this timestamp is equal to the passed one.
  //! \param Other [in] another timestamp to compare with.
  //! \return true/false.
  bool IsEqual(const Handle(asiAlgo_TimeStamp)& Other) const
  {
    return (Time == Other->Time) && (Internal == Other->Internal);
  }

  //! Checks whether this timestamp is less than the passed one.
  //! \param Other [in] another timestamp to compare with.
  //! \return true/false.
  bool IsLess(const Handle(asiAlgo_TimeStamp)& Other) const
  {
    return (Time < Other->Time) || (Time == Other->Time) && (Internal < Other->Internal);
  }

  //! Checks whether this timestamp is less or equal to the passed one.
  //! \param Other [in] another timestamp to compare with.
  //! \return true/false.
  bool LessOrEqual(const Handle(asiAlgo_TimeStamp)& Other) const
  {
    return this->IsLess(Other) || this->IsEqual(Other);
  }

  //! Checks whether this timestamp is greater than the passed one.
  //! \param Other [in] another timestamp to compare with.
  //! \return true/false.
  bool IsGreater(const Handle(asiAlgo_TimeStamp)& Other) const
  {
    return (Time > Other->Time) || (Time == Other->Time) && (Internal > Other->Internal);
  }

  //! Checks whether this timestamp is greater or equal to the passed one.
  //! \param Other [in] timestamp to compare with.
  //! \return true/false.
  bool IsGreaterOrEqual(const Handle(asiAlgo_TimeStamp)& Other) const
  {
    return this->IsGreater(Other) || this->IsEqual(Other);
  }

  //! Copies the timestamp.
  //! \return copy of the timestamp.
  Handle(asiAlgo_TimeStamp) Clone() const
  {
    return new asiAlgo_TimeStamp(Time, Internal);
  }

  //! Dumps this timestamp to the passed output stream.
  //! \param out [in/out] stream to dump the timestamp to.
  void Dump(std::ostream* out)
  {
    *out << this->ToString().c_str();
  }

  //! Converts timestamp to string.
  //! \param useInternal [in] indicates whether the internal index must be
  //!        included into the string representation as well.
  //! \param isCompatible [in] indicates whether the requested string
  //!        representation has to contain "safe" characters only. In such
  //!        a form you can use it, for instance, as a filename.
  //! \return string representation of timestamp.
  std::string ToString(const bool useInternal = true,
                       const bool isCompatible = false) const
  {
    std::string res;

#ifdef _WIN32
    char buf[26];
    ctime_s(buf, 26, &Time);
    buf[24] = '\0'; // Replace EOL [\n\0 --> \0\0]

    res += buf;
    if ( useInternal )
      res += ":: [" + asiAlgo_Utils::Str::ToString(Internal) + "]";

    if ( isCompatible )
    {
      asiAlgo_Utils::Str::ReplaceAll(res, ":", "");
      asiAlgo_Utils::Str::ReplaceAll(res, "[", "");
      asiAlgo_Utils::Str::ReplaceAll(res, "]", "");
      asiAlgo_Utils::Str::ReplaceAll(res, " ", "_");
    }
#else
    std::stringstream ss;
    ss << Time;
    res = ss.str();
#endif

    return res;
  }
};

//-----------------------------------------------------------------------------
// TimeStampTool utility
//-----------------------------------------------------------------------------

//! Auxiliary class generating unique and monotonic timestamps.
//! This class operates with standard time_t type not attempting to cast it
//! to any primitive (like integer, long etc). As this timestamp allows
//! serialization, it needs to be represented somehow with simple integer
//! basis. Even though straightforward conversion from time_t to int does
//! the trick, it is not really a solution as it leads to injection of Y2K38
//! problem into the application domain. The natural alternative solution is
//! to store standard "tm" structure containing only integer values and
//! being easily serialized this way.
//!
//! Another peculiarity here is that time_t value does not contain milliseconds
//! and cannot provide enough timing granularity for some cases. This is
//! resolved by extending that type with additional atomically incremented
//! static integer value. This value is used to maintain uniqueness only and
//! does not have any wise meaning.
class asiAlgo_TimeStampTool
{
public:

  asiAlgo_EXPORT static Handle(asiAlgo_TimeStamp)
    Generate();

  asiAlgo_EXPORT static std::vector<int>
    AsChunked(const Handle(asiAlgo_TimeStamp)& TS);

  asiAlgo_EXPORT static Handle(asiAlgo_TimeStamp)
    FromChunked(const std::vector<int>& chunked);

};

#endif
