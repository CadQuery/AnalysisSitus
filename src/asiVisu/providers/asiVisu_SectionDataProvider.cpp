//-----------------------------------------------------------------------------
// Created on: 17 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_SectionDataProvider.h>

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
asiVisu_SectionDataProvider::asiVisu_SectionDataProvider(const ActAPI_DataObjectId&           theNodeId,
                                                       const Handle(ActAPI_HParameterList)& theParamList)
: asiVisu_CurveDataProvider()
{
  m_nodeID = theNodeId;
  m_params = theParamList;
}

//-----------------------------------------------------------------------------

//! \return curve type.
Handle(Standard_Type) asiVisu_SectionDataProvider::GetCurveType() const
{
  double f, l;
  return this->GetCurve(f, l)->DynamicType();
}

//! Not used.
Handle(Geom2d_Curve) asiVisu_SectionDataProvider::GetCurve2d(double&, double&) const
{
  return NULL;
}

//! Accessor for curve.
//! \param f [out] first parameter.
//! \param l [out] last parameter.
//! \return curve.
Handle(Geom_Curve) asiVisu_SectionDataProvider::GetCurve(double& f, double& l) const
{
  TopoDS_Shape section_shape = this->GetShape();
  //
  TopExp_Explorer exp(section_shape, TopAbs_EDGE);
  TopoDS_Edge section_edge = TopoDS::Edge( exp.Current() );

  // Extract curve
  Handle(Geom_Curve) c3d = BRep_Tool::Curve(section_edge, f, l);
  return c3d;
}

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId asiVisu_SectionDataProvider::GetNodeID() const
{
  return m_nodeID;
}

//! Returns the OCCT topological shape to be visualized.
//! \return OCCT topological shape.
TopoDS_Shape asiVisu_SectionDataProvider::GetShape() const
{
  return ActParamTool::AsShape( m_params->Value(1) )->GetShape();
}

//-----------------------------------------------------------------------------

//! Creates a copy of the Data Provider.
//! \return copy.
Handle(asiVisu_SectionDataProvider) asiVisu_SectionDataProvider::Clone() const
{
  return new asiVisu_SectionDataProvider(m_nodeID, m_params);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_SectionDataProvider::translationSources() const
{
  return ActAPI_ParameterStream() << m_params->Value(1); // Shape Parameter
}
