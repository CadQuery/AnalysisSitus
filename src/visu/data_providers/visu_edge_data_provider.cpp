//-----------------------------------------------------------------------------
// Created on: 18 August 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_edge_data_provider.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (geometry) includes
#include <geom_part_node.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <TopExp.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Constructor accepting the set of source data structures.
//! \param theNodeId    [in] ID of the target Data Node.
//! \param theParamList [in] source Parameters.
visu_edge_data_provider::visu_edge_data_provider(const ActAPI_DataObjectId&           theNodeId,
                                                 const Handle(ActAPI_HParameterList)& theParamList)
: visu_data_provider()
{
  m_nodeID = theNodeId;
  m_params = theParamList;

  // Access owning geometry
  Handle(geom_part_node) geom_n = common_facilities::Instance()->Model->GetPartNode();

  // Build maps
  if ( !geom_n->GetShape().IsNull() )
  {
    TopExp::MapShapes(geom_n->GetShape(), m_subShapes);
    TopExp::MapShapes(geom_n->GetShape(), TopAbs_EDGE, m_edges);
  }
}

//-----------------------------------------------------------------------------

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId visu_edge_data_provider::GetNodeID() const
{
  return m_nodeID;
}

//! \return global index of the OCCT edge to be visualized.
int visu_edge_data_provider::GetEdgeIndexAmongSubshapes() const
{
  return ActParamTool::AsInt( m_params->Value(1) )->GetValue();
}

//! \return local index of the OCCT edges to be visualized.
int visu_edge_data_provider::GetEdgeIndexAmongEdges() const
{
  const int globalId = ActParamTool::AsInt( m_params->Value(1) )->GetValue();

  if ( globalId )
    return m_edges.FindIndex( m_subShapes.FindKey(globalId) );

  return 0;
}

//! \return topological edge extracted from the part by its stored ID.
TopoDS_Edge visu_edge_data_provider::ExtractEdge() const
{
  const int eIdx = this->GetEdgeIndexAmongSubshapes();
  if ( !eIdx )
    return TopoDS_Edge();

  const TopoDS_Shape& shape = m_subShapes.FindKey(eIdx);
  if ( shape.ShapeType() != TopAbs_EDGE )
    return TopoDS_Edge();

  // Access edge by the stored index
  const TopoDS_Edge& E = TopoDS::Edge(shape);
  return E;
}

//-----------------------------------------------------------------------------

//! Creates a copy of the Data Provider.
//! \return copy.
Handle(visu_edge_data_provider) visu_edge_data_provider::Clone() const
{
  return new visu_edge_data_provider(m_nodeID, m_params);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_edge_data_provider::translationSources() const
{
  return ActParamStream() << m_params->Value(1); // Parameter for edge index
}
