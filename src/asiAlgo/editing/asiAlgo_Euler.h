//-----------------------------------------------------------------------------
// Created on: 15 December 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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

#ifndef asiAlgo_Euler_h
#define asiAlgo_Euler_h

// asiAlgo includes
#include <asiAlgo_TopoKill.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Base class for Euler operators.
class asiAlgo_Euler : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_Euler, ActAPI_IAlgorithm)

public:

  //! Constructor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] progress  Progress Notifier.
  //! \param[in] plotter   Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_Euler(const TopoDS_Shape&  masterCAD,
                  ActAPI_ProgressEntry progress,
                  ActAPI_PlotterEntry  plotter);

public:

  //! Performs Euler operation.
  //! \param[in] doApply indicates whether to apply Re-Shape modification
  //!                    requests right at the end (true) or let the caller
  //!                    code manage it explicitly (false).
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT virtual bool
    Perform(const bool doApply = true);

public:

  //! \return modified model.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

  //! \return topology killer tool used in the "engine room".
  const Handle(asiAlgo_TopoKill)& GetTopoKiller() const
  {
    return m_killer;
  }

  //! Sets topology killer to use. This method is useful when you want to pass
  //! one instance of topology killer to several Euler operators.
  //! \param[in] killer instance of topology killer to use for modification.
  void SetTopoKiller(const Handle(asiAlgo_TopoKill)& killer)
  {
    m_killer = killer;
  }

  //! \return modification history.
  const Handle(asiAlgo_History)& GetHistory() const
  {
    return m_history;
  }

  //! \brief Sets externally defined history.
  //!
  //! The passed history will not be cleared, so that a pipeline of
  //! modeling tools will grow its history graph as long as the history
  //! object is continuously passed from one tool to another.
  //!
  //! \param[in] history history to set.
  void SetHistory(const Handle(asiAlgo_History)& history)
  {
    m_history = history;
  }

private:

  virtual bool perform(const bool doApply) = 0;

protected:

  Handle(asiAlgo_TopoKill) m_killer;  //!< Topology killer.
  Handle(asiAlgo_History)  m_history; //!< Modification history.
  TopoDS_Shape             m_master;  //!< Master CAD model.
  TopoDS_Shape             m_result;  //!< Result CAD model.

};

#endif
