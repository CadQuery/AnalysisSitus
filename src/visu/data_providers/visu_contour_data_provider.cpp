//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_contour_data_provider.h>

// Common includes
#include <common_facilities.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! ctor.
visu_contour_data_provider::visu_contour_data_provider()
: visu_shape_data_provider()
{
  Handle(geom_contour_node)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();

  // Set Node ID
  m_nodeID = contour_n->GetId();

  Handle(ActAPI_HParameterList) params = new ActAPI_HParameterList;
  params->Append( contour_n->Parameter(geom_contour_node::PID_Coords) );
  params->Append( contour_n->Parameter(geom_contour_node::PID_IsClosed) );
  params->Append( contour_n->Parameter(geom_contour_node::PID_Geometry) );

  // Set tracked Parameters
  m_params = params;
}

//-----------------------------------------------------------------------------

//! Returns the OCCT topological shape to be visualized.
//! \return OCCT topological shape.
TopoDS_Shape visu_contour_data_provider::GetShape() const
{
  Handle(geom_contour_node)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();

  return contour_n->AsShape();
}
