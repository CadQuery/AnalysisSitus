//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_face_data_provider.h>

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
visu_face_data_provider::visu_face_data_provider(const ActAPI_DataObjectId&           theNodeId,
                                                 const Handle(ActAPI_HParameterList)& theParamList)
: visu_data_provider()
{
  m_nodeID = theNodeId;
  m_params = theParamList;

  // Access owning geometry
  Handle(geom_part_node) geom_n = common_facilities::Instance()->Model->PartNode();

  // Build maps
  if ( !geom_n->GetShape().IsNull() )
  {
    TopExp::MapShapes(geom_n->GetShape(), m_subShapes);
    TopExp::MapShapes(geom_n->GetShape(), TopAbs_FACE, m_faces);
  }
}

//-----------------------------------------------------------------------------

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId visu_face_data_provider::GetNodeID() const
{
  return m_nodeID;
}

//! \return global index of the OCCT face to be visualized.
int visu_face_data_provider::GetFaceIndexAmongSubshapes() const
{
  return ActParamTool::AsInt( m_params->Value(1) )->GetValue();
}

//! \return local index of the OCCT face to be visualized.
int visu_face_data_provider::GetFaceIndexAmongFaces() const
{
  const int globalId = ActParamTool::AsInt( m_params->Value(1) )->GetValue();

  if ( globalId )
    return m_faces.FindIndex( m_subShapes.FindKey(globalId) );

  return 0;
}

//! \return topological face extracted from the part by its stored ID.
TopoDS_Face visu_face_data_provider::ExtractFace() const
{
  const int fIdx = this->GetFaceIndexAmongSubshapes();
  if ( !fIdx )
    return TopoDS_Face();

  const TopoDS_Shape& shape = m_subShapes.FindKey(fIdx);
  if ( shape.ShapeType() != TopAbs_FACE )
    return TopoDS_Face();

  // Access face by the stored index
  const TopoDS_Face& F = TopoDS::Face(shape);
  return F;
}

//-----------------------------------------------------------------------------

//! Creates a copy of the Data Provider.
//! \return copy.
Handle(visu_face_data_provider) visu_face_data_provider::Clone() const
{
  return new visu_face_data_provider(m_nodeID, m_params);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_face_data_provider::translationSources() const
{
  return ActParamStream() << m_params->Value(1); // Parameter for face index
}
