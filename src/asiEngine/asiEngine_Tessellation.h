//-----------------------------------------------------------------------------
// Created on: 05 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiEngine_Tessellation_h
#define asiEngine_Tessellation_h

// asiEngine includes
#include <asiEngine_Model.h>

// asiData includes
#include <asiData_TessNode.h>
#include <asiData_TessNormsNode.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

//! Data Model API for tessellations.
class asiEngine_Tessellation
{
public:

  asiEngine_Tessellation(const Handle(asiEngine_Model)&             model,
                         const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                         ActAPI_ProgressEntry                       progress = NULL,
                         ActAPI_PlotterEntry                        plotter  = NULL)
  : m_model    (model),
    m_prsMgr   (prsMgr),
    m_progress (progress),
    m_plotter  (plotter)
  {}

public:

  //! Creates new Tessellation Node in the Data Model.
  //! \return newly created Tessellation Node.
  asiEngine_EXPORT Handle(asiData_TessNode)
    CreateTessellation();

  //! Computes normal field for the given Tessellation Node. This method
  //! creates a child Node under the passed one to store the computed
  //! normal vectors.
  //! \param[in] tessNode Tessellation Node to compute the normal field for.
  //! \return child Data Node representing the computed normal field.
  asiEngine_EXPORT Handle(asiData_TessNormsNode)
    ComputeNorms(const Handle(asiData_TessNode)& tessNode);

protected:

  Handle(asiEngine_Model)             m_model;    //!< Data Model instance.
  vtkSmartPointer<asiVisu_PrsManager> m_prsMgr;   //!< Presentation manager.
  ActAPI_ProgressEntry                m_progress; //!< Progress notifier.
  ActAPI_PlotterEntry                 m_plotter;  //!< Plotter.

};

#endif
