//-----------------------------------------------------------------------------
// Created on: 02 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_FaceDataProvider.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param face_n [in] source Data Node.
asiVisu_FaceDataProvider::asiVisu_FaceDataProvider(const Handle(asiData_FaceNode)& face_n)
: asiVisu_SurfaceDataProvider()
{
  this->init(face_n);
}

//-----------------------------------------------------------------------------

//! Constructor.
//! \param surf_n [in] source Data Node.
asiVisu_FaceDataProvider::asiVisu_FaceDataProvider(const Handle(asiData_SurfNode)& surf_n)
: asiVisu_SurfaceDataProvider()
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

//-----------------------------------------------------------------------------

//! \return global index of the OCCT face to be visualized.
int asiVisu_FaceDataProvider::GetFaceIndexAmongSubshapes() const
{
  int globalId = 0;

  if ( m_node->IsInstance( STANDARD_TYPE(asiData_FaceNode) ) )
    globalId = Handle(asiData_FaceNode)::DownCast(m_node)->GetAnySelectedFace();
  else if ( m_node->IsInstance( STANDARD_TYPE(asiData_SurfNode) ) )
    globalId = Handle(asiData_SurfNode)::DownCast(m_node)->GetAnySelectedFace();

  return globalId;
}

//-----------------------------------------------------------------------------

//! \return local index of the OCCT face to be visualized.
int asiVisu_FaceDataProvider::GetFaceIndexAmongFaces() const
{
  const int globalId = this->GetFaceIndexAmongSubshapes();

  if ( globalId )
  {
    if ( m_partNode->GetAAG().IsNull() )
      return 0;

    const TopTools_IndexedMapOfShape&
      faces = m_partNode->GetAAG()->GetMapOfFaces();

    const TopTools_IndexedMapOfShape&
      subShapes = m_partNode->GetAAG()->RequestMapOfSubShapes();

    if ( globalId <= subShapes.Extent() )
      return faces.FindIndex( subShapes.FindKey(globalId) );
  }

  return 0;
}

//-----------------------------------------------------------------------------

//! \return topological face extracted from the part by its stored ID.
TopoDS_Face asiVisu_FaceDataProvider::ExtractFace() const
{
  const int fIdx = this->GetFaceIndexAmongSubshapes();
  if ( !fIdx )
    return TopoDS_Face();

  if ( m_partNode->GetAAG().IsNull() )
    return TopoDS_Face();

  const TopTools_IndexedMapOfShape&
    subShapes = m_partNode->GetAAG()->RequestMapOfSubShapes();

  if ( fIdx > subShapes.Extent() )
    return TopoDS_Face();

  const TopoDS_Shape& shape = subShapes.FindKey(fIdx);
  //
  if ( shape.ShapeType() != TopAbs_FACE )
    return TopoDS_Face();

  // Access face by the stored index
  const TopoDS_Face& F = TopoDS::Face(shape);
  return F;
}

//-----------------------------------------------------------------------------

//! \return surface type.
Handle(Standard_Type) asiVisu_FaceDataProvider::GetSurfaceType() const
{
  // Get face
  TopoDS_Face F = this->ExtractFace();
  //
  if ( F.IsNull() )
    return nullptr;

  // Get host surface
  Handle(Geom_Surface) surf = BRep_Tool::Surface(F);

  return surf->DynamicType();
}

//-----------------------------------------------------------------------------

// Accessor for the parametric surface.
//! \param uMin [out] min bound for U curvilinear axis.
//! \param uMax [out] max bound for U curvilinear axis.
//! \param vMin [out] min bound for V curvilinear axis.
//! \param vMax [out] max bound for V curvilinear axis.
//! \return surface.
Handle(Geom_Surface)
  asiVisu_FaceDataProvider::GetSurface(double& uMin,
                                       double& uMax,
                                       double& vMin,
                                       double& vMax) const
{
  // Get face
  TopoDS_Face F = this->ExtractFace();
  //
  if ( F.IsNull() )
    return nullptr;

  // Get host surface
  Handle(Geom_Surface) surf = BRep_Tool::Surface(F);

  // Initialize parametric bounds
  surf->Bounds(uMin, uMax, vMin, vMax);

  return surf;
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

  return nullptr;
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
    PID = asiData_FaceNode::PID_SelectedFaces;
  else if ( m_node->IsInstance( STANDARD_TYPE(asiData_SurfNode) ) )
    PID = asiData_SurfNode::PID_SelectedFaces;

  if ( PID == -1 )
    return nullptr;

  return ActParamStream() << m_node->Parameter(PID); // Parameter for face index
}
