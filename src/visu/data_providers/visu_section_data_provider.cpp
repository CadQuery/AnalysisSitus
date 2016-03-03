//-----------------------------------------------------------------------------
// Created on: 17 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_section_data_provider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//-----------------------------------------------------------------------------

//! Constructor accepting the set of source data structures.
//! \param theNodeId    [in] ID of the target Data Node.
//! \param theParamList [in] source Parameters.
visu_section_data_provider::visu_section_data_provider(const ActAPI_DataObjectId&           theNodeId,
                                                       const Handle(ActAPI_HParameterList)& theParamList)
: visu_curve_data_provider()
{
  m_nodeID = theNodeId;
  m_params = theParamList;
}

//-----------------------------------------------------------------------------

//! \return curve type.
Handle(Standard_Type) visu_section_data_provider::GetCurveType() const
{
  return this->GetCurve()->DynamicType();
}

//! Not used.
Handle(Geom2d_Curve) visu_section_data_provider::GetCurve2d() const
{
  return NULL;
}

//! Not used.
Handle(Geom_Curve) visu_section_data_provider::GetCurve() const
{
  TopoDS_Shape section_shape = this->GetShape();
  //
  TopExp_Explorer exp(section_shape, TopAbs_EDGE);
  TopoDS_Edge section_edge = TopoDS::Edge( exp.Current() );

  // Extract curve
  double f, l;
  Handle(Geom_Curve) c3d = BRep_Tool::Curve(section_edge, f, l);
  return c3d;
}

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId visu_section_data_provider::GetNodeID() const
{
  return m_nodeID;
}

//! Returns the OCCT topological shape to be visualized.
//! \return OCCT topological shape.
TopoDS_Shape visu_section_data_provider::GetShape() const
{
  return ActParamTool::AsShape( m_params->Value(1) )->GetShape();
}

//-----------------------------------------------------------------------------

//! Creates a copy of the Data Provider.
//! \return copy.
Handle(visu_section_data_provider) visu_section_data_provider::Clone() const
{
  return new visu_section_data_provider(m_nodeID, m_params);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_section_data_provider::translationSources() const
{
  return ActAPI_ParameterStream() << m_params->Value(1); // Shape Parameter
}
