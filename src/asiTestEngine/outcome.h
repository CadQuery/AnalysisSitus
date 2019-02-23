//-----------------------------------------------------------------------------
// Created on: 26 June 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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

#ifndef outcome_h
#define outcome_h

// asiAlgo includes
#include <asiAlgo_MemChecker.h>
#include <asiAlgo_Utils.h>

// Standard includes
#include <string>
#include <sstream>

// OCCT includes
#include <OSD_Timer.hxx>

//! Defines a class for holding the results of test functions. This class
//! contains a Boolean flag to indicate success or failure status of the
//! test routine, together with additional information on function execution.
//!
//! Usage of outcome objects instead of primitive Boolean types is inspired
//! by ACIS modeling kernel. See (Corney, J.R. and Lim, T. 2001. 3D Modelling with ACIS.)
//! for details.
class outcome
{
public:

  std::string name;           //!< Function name.
  bool        ok;             //!< Success/failure.
  std::string elapsedTimeSec; //!< Elapsed (wall) time in seconds.
  int         memBefore;      //!< Consumed memory [MiB] before function execution.
  int         memAfter;       //!< Consumed memory [MiB] after function execution.

public:

  //! Default ctor.
  explicit outcome()
  : ok             (false),
    elapsedTimeSec ("undefined"),
    memBefore      (-1),
    memAfter       (-1)
  {
    this->startTimer();
  }

  //! Ctor accepting function name.
  //! \param[in] _name function name.
  explicit outcome(const std::string& _name)
  : name           (_name),
    ok             (false),
    elapsedTimeSec ("undefined"),
    memBefore      (-1),
    memAfter       (-1)
  {
    this->startTimer();
  }

  //! Ctor accepting function name.
  //! \param[in] _name   function name.
  //! \param[in] _funcID function ID.
  explicit outcome(const std::string& _name,
                   const int          _funcID)
  : name           (_name + "[" + asiAlgo_Utils::Str::ToString(_funcID) + "]"),
    ok             (false),
    elapsedTimeSec ("undefined"),
    memBefore      (-1),
    memAfter       (-1)
  {
    this->startTimer();
  }

  //! Ctor accepting success/failure flag.
  //! \param[in] _ok Boolean value to set as execution status.
  explicit outcome(const bool _ok)
  : ok             (_ok),
    elapsedTimeSec ("undefined"),
    memBefore      (-1),
    memAfter       (-1)
  {
    this->startTimer();
  }

  //! Ctor accepting function name and success/failure flag.
  //! \param[in] _name function name.
  //! \param[in] _ok   Boolean value to set as execution status.
  explicit outcome(const std::string& _name,
                   const bool         _ok)
  : name           (_name),
    ok             (_ok),
    elapsedTimeSec ("undefined"),
    memBefore      (-1),
    memAfter       (-1)
  {
    this->startTimer();
  }

  //! Ctor accepting function name, success/failure flag and execution time.
  //! \param[in] _name function name.
  //! \param[in] _ok   Boolean value to set as execution status.
  //! \param[in] _time execution time in seconds.
  explicit outcome(const std::string& _name,
                   const bool         _ok,
                   const std::string& _time)
  : name           (_name),
    ok             (_ok),
    elapsedTimeSec (_time),
    memBefore      (-1),
    memAfter       (-1)
  {
    this->startTimer();
  }

public:

  //! \return failure outcome.
  const outcome& failure()
  {
    this->stopTimer();
    this->ok = false;
    return *this;
  }

  //! \return success outcome.
  const outcome& success()
  {
    this->stopTimer();
    this->ok = true;
    return *this;
  }

  //! Modifies this outcome to have the passed status.
  //! \param[in] _ok status to set.
  //! \return outcome with the given status.
  const outcome& status(const bool _ok)
  {
    this->stopTimer();
    this->ok = _ok;
    return *this;
  }

  //! Stops timer and returns the elapsed time.
  //! \return elapsed time.
  const std::string& time()
  {
    this->stopTimer();
    return this->elapsedTimeSec;
  }

public:

  //! Dumps outcome to the passed output stream.
  //! \param[in,out] out target output stream.
  void dump(std::ostream& out) const
  {
    out << "outcome for "                <<  this->name.c_str()           << "\n";
    out << "ok: "                        << (this->ok ? "true" : "false") << "\n";
    out << "elapsed time (wall clock): " <<  this->elapsedTimeSec << " s" << "\n";
  }

protected:

  //! Starts internal timer.
  void startTimer()
  {
    MEMCHECK_COUNT_MIB(this->memBefore)
    m_timer.Start();
  }

  //! Stops internal timer.
  void stopTimer()
  {
    m_timer.Stop();
    MEMCHECK_COUNT_MIB(this->memAfter)

    // Prepare string for elapsed time.
    std::ostringstream os;
    os << std::setprecision(3);
    os << m_timer.ElapsedTime();
    this->elapsedTimeSec = os.str();
  }

protected:

  OSD_Timer m_timer; //!< Automatic timer.

};

#endif
