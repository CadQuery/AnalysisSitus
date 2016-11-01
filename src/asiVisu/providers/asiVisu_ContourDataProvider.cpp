//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ContourDataProvider.h>

// Common includes
#include <common_facilities.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! ctor.
asiVisu_ContourDataProvider::asiVisu_ContourDataProvider()
: asiVisu_ShapeDataProvider()
{
  Handle(asiData_ContourNode)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();

  // Set Node ID
  m_nodeID = contour_n->GetId();

  Handle(ActAPI_HParameterList) params = new ActAPI_HParameterList;
  params->Append( contour_n->Parameter(asiData_ContourNode::PID_Coords) );
  params->Append( contour_n->Parameter(asiData_ContourNode::PID_IsClosed) );
  params->Append( contour_n->Parameter(asiData_ContourNode::PID_Geometry) );

  // Set tracked Parameters
  m_params = params;
}

//-----------------------------------------------------------------------------

//! Returns the OCCT topological shape to be visualized.
//! \return OCCT topological shape.
TopoDS_Shape asiVisu_ContourDataProvider::GetShape() const
{
  Handle(asiData_ContourNode)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();

  return contour_n->AsShape();
}
