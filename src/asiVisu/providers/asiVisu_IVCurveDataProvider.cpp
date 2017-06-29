//-----------------------------------------------------------------------------
// Created on: 11 April 2016
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

// Own include
#include <asiVisu_IVCurveDataProvider.h>

// Visualization includes
#include <asiData_IVCurveNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param N [in] source Node.
asiVisu_IVCurveDataProvider::asiVisu_IVCurveDataProvider(const Handle(ActAPI_INode)& N)
: asiVisu_CurveDataProvider(),
  m_node(N)
{
}

//-----------------------------------------------------------------------------

//! \return curve type.
Handle(Standard_Type) asiVisu_IVCurveDataProvider::GetCurveType() const
{
  double f, l;
  return this->GetCurve(f, l)->DynamicType();
}

//! Not used.
Handle(Geom2d_Curve) asiVisu_IVCurveDataProvider::GetCurve2d(double&, double&) const
{
  return NULL;
}

//! Accessor for curve.
//! \param f [out] first parameter.
//! \param l [out] last parameter.
//! \return curve.
Handle(Geom_Curve) asiVisu_IVCurveDataProvider::GetCurve(double& f, double& l) const
{
  Handle(asiData_IVCurveNode)
    curve_n = Handle(asiData_IVCurveNode)::DownCast(m_node);
  //
  if ( curve_n.IsNull() || !curve_n->IsWellFormed() )
    return NULL;

  return curve_n->GetCurve(f, l);
}

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId asiVisu_IVCurveDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

//! Creates a copy of the Data Provider.
//! \return copy.
Handle(asiVisu_IVCurveDataProvider) asiVisu_IVCurveDataProvider::Clone() const
{
  return new asiVisu_IVCurveDataProvider(m_node);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_IVCurveDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_IVCurveNode)
    curve_n = Handle(asiData_IVCurveNode)::DownCast(m_node);
  //
  if ( curve_n.IsNull() || !curve_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << curve_n->Parameter(asiData_IVCurveNode::PID_Geometry);

  return out;
}
