//-----------------------------------------------------------------------------
// Created on: 19 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ContourDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! ctor.
//! \param contour [in] contour Node.
asiVisu_ContourDataProvider::asiVisu_ContourDataProvider(const Handle(asiData_ContourNode)& contour)
: asiVisu_ShapeDataProvider(), m_contour(contour)
{
  // Set Node ID
  m_nodeID = m_contour->GetId();

  Handle(ActAPI_HParameterList) params = new ActAPI_HParameterList;
  params->Append( m_contour->Parameter(asiData_ContourNode::PID_Coords) );
  params->Append( m_contour->Parameter(asiData_ContourNode::PID_IsClosed) );
  params->Append( m_contour->Parameter(asiData_ContourNode::PID_Geometry) );

  // Set tracked Parameters
  m_params = params;
}

//-----------------------------------------------------------------------------

//! Returns the OCCT topological shape to be visualized.
//! \return OCCT topological shape.
TopoDS_Shape asiVisu_ContourDataProvider::GetShape() const
{
  return m_contour->AsShape();
}
