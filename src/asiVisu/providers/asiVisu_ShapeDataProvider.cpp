//-----------------------------------------------------------------------------
// Created on: 28 November 2015
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

// Own include
#include <asiVisu_ShapeDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! ctor.
asiVisu_ShapeDataProvider::asiVisu_ShapeDataProvider()
: asiVisu_DataProvider()
{}

//-----------------------------------------------------------------------------

//! Constructor accepting the set of source data structures.
//! \param[in] nodeId    ID of the target Data Node.
//! \param[in] paramList source Parameters.
asiVisu_ShapeDataProvider::asiVisu_ShapeDataProvider(const ActAPI_DataObjectId&           nodeId,
                                                     const Handle(ActAPI_HParameterList)& paramList)
: asiVisu_DataProvider()
{
  m_nodeID = nodeId;
  m_params = paramList;
}

//-----------------------------------------------------------------------------

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId asiVisu_ShapeDataProvider::GetNodeID() const
{
  return m_nodeID;
}

//-----------------------------------------------------------------------------

//! Returns the OCCT topological shape to be visualized.
//! \return OCCT topological shape.
TopoDS_Shape asiVisu_ShapeDataProvider::GetShape() const
{
  return ActParamTool::AsShape( m_params->Value(1) )->GetShape();
}

//-----------------------------------------------------------------------------

//! \return linear deflection.
double asiVisu_ShapeDataProvider::GetLinearDeflection() const
{
  if ( m_params->Length() != 3 )
    return 0.;

  return ActParamTool::AsReal( m_params->Value(2) )->GetValue();
}

//-----------------------------------------------------------------------------

//! \return angular deflection.
double asiVisu_ShapeDataProvider::GetAngularDeflection() const
{
  if ( m_params->Length() != 3 )
    return 0.;

  return ActParamTool::AsReal( m_params->Value(3) )->GetValue();
}

//-----------------------------------------------------------------------------

//! Creates a copy of the Data Provider.
//! \return copy.
Handle(asiVisu_ShapeDataProvider) asiVisu_ShapeDataProvider::Clone() const
{
  return new asiVisu_ShapeDataProvider(m_nodeID, m_params);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_ShapeDataProvider::translationSources() const
{
  return m_params;
}
