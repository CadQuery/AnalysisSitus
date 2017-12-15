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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <BRepTools_ReShape.hxx>

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

  //! Constructor.
  //! \param[in] masterCAD full CAD model.
  //! \param[in] ctx       Re-Shape tool.
  //! \param[in] progress  Progress Notifier.
  //! \param[in] plotter   Imperative Plotter.
  asiAlgo_EXPORT
    asiAlgo_Euler(const TopoDS_Shape&              masterCAD,
                  const Handle(BRepTools_ReShape)& ctx,
                  ActAPI_ProgressEntry             progress,
                  ActAPI_PlotterEntry              plotter);

public:

  //! Static method to compute value of the Euler-Poincare characteristic for the given genus.
  asiAlgo_EXPORT static int
    EulerPoincareCharacteristic(const TopoDS_Shape& shape,
                                const int genus = 0);


public:

  //! Performs Euler operation.
  //! \param[in] doApply indicates whether to apply Re-Shape modification
  //!                    requests right at the end (true) or let the caller
  //!                    code manage it explicitly (false).
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT virtual bool
    Perform(const bool doApply = true);

private:

  virtual bool perform(const bool doApply) = 0;

public:

  //! \return converted model.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

  //! \return Reshape tool.
  const Handle(BRepTools_ReShape)& GetContext() const
  {
    return m_ctx;
  }

protected:

  Handle(BRepTools_ReShape) m_ctx;    //!< Reshape tool.
  TopoDS_Shape              m_master; //!< Master CAD.
  TopoDS_Shape              m_result; //!< Result CAD.

};

#endif
