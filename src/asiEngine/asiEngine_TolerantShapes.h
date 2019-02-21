//-----------------------------------------------------------------------------
// Created on: 19 April 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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

#ifndef asiEngine_TolerantShapes_h
#define asiEngine_TolerantShapes_h

// asiEngine includes
#include <asiEngine_Base.h>

// asiData includes
#include <asiData_TolerantRangeNode.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

//! API for operating with Tolerant Shape data objects.
class asiEngine_TolerantShapes : public asiEngine_Base
{
public:

  //! Ctor.
  //! \param[in] model    Data Model instance.
  //! \param[in] prsMgr   Presentation Manager.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiEngine_TolerantShapes(const Handle(asiEngine_Model)&             model,
                           const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                           ActAPI_ProgressEntry                       progress = NULL,
                           ActAPI_PlotterEntry                        plotter  = NULL)
  //
  : asiEngine_Base (model, progress, plotter),
    m_prsMgr       (prsMgr)
  {
    m_tolShapes = m_model->GetPartNode()->GetTolerantShapes();
  }

public:

  //! Populates Tolerant Shapes Node with the results of tolerance analysis.
  //! \param[in] shape     shape in question.
  //! \param[in] numRanges number of tolerance ranges to use.
  asiEngine_EXPORT void
    Populate(const TopoDS_Shape& shape,
             const int           numRanges);

  //! Cleans all nested ranges.
  asiEngine_EXPORT void
    Clean_All();

  //! Creates Data Node to store shapes within the specified tolerance
  //! range.
  //! \param[in] shape    shape to store.
  //! \param[in] colorNum color to use (integer mask for RGB bytes).
  //! \param[in] tolerMin min tolerance.
  //! \param[in] tolerMax max tolerance.
  //! \return newly created Range Node.
  asiEngine_EXPORT Handle(asiData_TolerantRangeNode)
    Create_Range(const TopoDS_Shape& shape,
                 const int           colorNum,
                 const double        tolerMin,
                 const double        tolerMax);

protected:

  Handle(asiData_TolerantShapesNode)  m_tolShapes; //!< Tolerant shapes Node.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr;    //!< Presentation Manager.

};

#endif
