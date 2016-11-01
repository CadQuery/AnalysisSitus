//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_FaceDataProvider.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (geometry) includes
#include <asiData_PartNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <TopExp.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Constructor accepting the set of source data structures.
//! \param theNodeId    [in] ID of the target Data Node.
//! \param theParamList [in] source Parameters.
asiVisu_FaceDataProvider::asiVisu_FaceDataProvider(const ActAPI_DataObjectId&           theNodeId,
                                                 const Handle(ActAPI_HParameterList)& theParamList)
: asiVisu_DataProvider()
{
  m_nodeID = theNodeId;
  m_params = theParamList;

  // Access owning geometry
  Handle(asiData_PartNode) geom_n = common_facilities::Instance()->Model->GetPartNode();

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
ActAPI_DataObjectId asiVisu_FaceDataProvider::GetNodeID() const
{
  return m_nodeID;
}

//! \return global index of the OCCT face to be visualized.
int asiVisu_FaceDataProvider::GetFaceIndexAmongSubshapes() const
{
  return ActParamTool::AsInt( m_params->Value(1) )->GetValue();
}

//! \return local index of the OCCT face to be visualized.
int asiVisu_FaceDataProvider::GetFaceIndexAmongFaces() const
{
  const int globalId = ActParamTool::AsInt( m_params->Value(1) )->GetValue();

  if ( globalId )
    return m_faces.FindIndex( m_subShapes.FindKey(globalId) );

  return 0;
}

//! \return topological face extracted from the part by its stored ID.
TopoDS_Face asiVisu_FaceDataProvider::ExtractFace() const
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
Handle(asiVisu_FaceDataProvider) asiVisu_FaceDataProvider::Clone() const
{
  return new asiVisu_FaceDataProvider(m_nodeID, m_params);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_FaceDataProvider::translationSources() const
{
  return ActParamStream() << m_params->Value(1); // Parameter for face index
}
