//-----------------------------------------------------------------------------
// Created on: 30 March 2017
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
#include <asiVisu_PartDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! ctor.
asiVisu_PartDataProvider::asiVisu_PartDataProvider()
: asiVisu_DataProvider()
{}

//-----------------------------------------------------------------------------

//! Constructor accepting the data source Node.
//! \param partNode [in] Part Node.
asiVisu_PartDataProvider::asiVisu_PartDataProvider(const Handle(asiData_PartNode)& partNode)
: asiVisu_DataProvider(), m_node(partNode)
{}

//-----------------------------------------------------------------------------

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId asiVisu_PartDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

//! \return Part Node.
Handle(asiData_PartNode) asiVisu_PartDataProvider::GetPartNode() const
{
  return m_node;
}

//-----------------------------------------------------------------------------

//! Returns the OCCT model to be visualized.
//! \return OCCT B-Rep model.
TopoDS_Shape asiVisu_PartDataProvider::GetShape() const
{
  return m_node->GetShape();
}

//-----------------------------------------------------------------------------

//! \return AAG.
Handle(asiAlgo_AAG) asiVisu_PartDataProvider::GetAAG() const
{
  return m_node->GetAAG();
}

//-----------------------------------------------------------------------------

double asiVisu_PartDataProvider::GetLinearDeflection() const
{
  return m_node->GetLinearDeflection();
}

//-----------------------------------------------------------------------------

double asiVisu_PartDataProvider::GetAngularDeflection() const
{
  return m_node->GetAngularDeflection();
}

//-----------------------------------------------------------------------------

//! \return copy.
Handle(asiVisu_PartDataProvider) asiVisu_PartDataProvider::Clone() const
{
  return new asiVisu_PartDataProvider(m_node);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_PartDataProvider::translationSources() const
{
  ActParamStream params;

  params << m_node->Parameter(asiData_PartNode::PID_Geometry)
         << m_node->Parameter(asiData_PartNode::PID_AAG)
         << m_node->Parameter(asiData_PartNode::PID_TessLinDefl)
         << m_node->Parameter(asiData_PartNode::PID_TessAngDefl);

  return params.List;
}
