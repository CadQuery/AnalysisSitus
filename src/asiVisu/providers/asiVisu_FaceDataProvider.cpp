//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_FaceDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <TopExp.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param face_n [in] source Data Node.
asiVisu_FaceDataProvider::asiVisu_FaceDataProvider(const Handle(asiData_FaceNode)& face_n)
: asiVisu_DataProvider()
{
  this->init(face_n);
}

//-----------------------------------------------------------------------------

//! Constructor.
//! \param surf_n [in] source Data Node.
asiVisu_FaceDataProvider::asiVisu_FaceDataProvider(const Handle(asiData_SurfNode)& surf_n)
: asiVisu_DataProvider()
{
  this->init(surf_n);
}

//-----------------------------------------------------------------------------

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId asiVisu_FaceDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//! \return global index of the OCCT face to be visualized.
int asiVisu_FaceDataProvider::GetFaceIndexAmongSubshapes() const
{
  int globalId = 0;

  if ( m_node->IsInstance( STANDARD_TYPE(asiData_FaceNode) ) )
    globalId = Handle(asiData_FaceNode)::DownCast(m_node)->GetSelectedFace();
  else if ( m_node->IsInstance( STANDARD_TYPE(asiData_SurfNode) ) )
    globalId = Handle(asiData_SurfNode)::DownCast(m_node)->GetSelectedFace();

  return globalId;
}

//! \return local index of the OCCT face to be visualized.
int asiVisu_FaceDataProvider::GetFaceIndexAmongFaces()
{
  const int globalId = this->GetFaceIndexAmongSubshapes();

  if ( globalId )
  {
    const TopTools_IndexedMapOfShape&
      faces = m_partNode->GetAAG()->GetMapOfFaces();

    const TopTools_IndexedMapOfShape&
      subShapes = m_partNode->GetAAG()->GetMapOfSubShapes();

    return faces.FindIndex( subShapes.FindKey(globalId) );
  }

  return 0;
}

//! \return topological face extracted from the part by its stored ID.
TopoDS_Face asiVisu_FaceDataProvider::ExtractFace()
{
  const int fIdx = this->GetFaceIndexAmongSubshapes();
  if ( !fIdx )
    return TopoDS_Face();

  const TopTools_IndexedMapOfShape&
    subShapes = m_partNode->GetAAG()->GetMapOfSubShapes();

  const TopoDS_Shape& shape = subShapes.FindKey(fIdx);
  //
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
  if ( m_node->IsInstance( STANDARD_TYPE(asiData_FaceNode) ) )
  {
    Handle(asiData_FaceNode)
      faceNode = Handle(asiData_FaceNode)::DownCast(m_node);
    //
    return new asiVisu_FaceDataProvider(faceNode);
  }

  if ( m_node->IsInstance( STANDARD_TYPE(asiData_SurfNode) ) )
  {
    Handle(asiData_SurfNode)
      surfNode = Handle(asiData_SurfNode)::DownCast(m_node);
    //
    return new asiVisu_FaceDataProvider(surfNode);
  }

  return NULL;
}

//-----------------------------------------------------------------------------

//! Initializes data provider.
//! \param subNode [in] Node to initialize the data provider with.
void asiVisu_FaceDataProvider::init(const Handle(ActAPI_INode)& subNode)
{
  m_node = subNode;

  // Access owning geometry
  m_partNode = Handle(asiData_PartNode)::DownCast( m_node->GetParentNode() );
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_FaceDataProvider::translationSources() const
{
  int PID = -1;
  if ( m_node->IsInstance( STANDARD_TYPE(asiData_FaceNode) ) )
    PID = asiData_FaceNode::PID_SelectedFace;
  else if ( m_node->IsInstance( STANDARD_TYPE(asiData_SurfNode) ) )
    PID = asiData_SurfNode::PID_SelectedFace;

  if ( PID == -1 )
    return NULL;

  return ActParamStream() << m_node->Parameter(PID); // Parameter for face index
}
