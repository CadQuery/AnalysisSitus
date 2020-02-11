//-----------------------------------------------------------------------------
// Created on: 21 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
#include <asiEngine_Part.h>

// asiEngine includes
#include <asiEngine_Curve.h>
#include <asiEngine_TolerantShapes.h>

// asiVisu includes
#include <asiVisu_PartPrs.h>
#include <asiVisu_PartNodeInfo.h>
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_CheckDeviations.h>
#include <asiAlgo_MeshGen.h>
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkProperty.h>
#pragma warning(pop)

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! \return newly created Part Node.
Handle(asiData_PartNode) asiEngine_Part::CreatePart()
{
  // Add Part Node to Partition
  Handle(asiData_PartNode) geom_n = Handle(asiData_PartNode)::DownCast( asiData_PartNode::Instance() );
  m_model->GetPartPartition()->AddNode(geom_n);

  // Initialize geometry
  geom_n->Init(true);
  geom_n->SetUserFlags(NodeFlag_IsPresentedInPartView);
  geom_n->SetName("Part");

  // Create underlying face representation Node
  {
    Handle(ActAPI_INode) geom_face_base = asiData_FaceNode::Instance();
    m_model->GetFacePartition()->AddNode(geom_face_base);

    // Initialize
    Handle(asiData_FaceNode) geom_face_n = Handle(asiData_FaceNode)::DownCast(geom_face_base);
    geom_face_n->Init();
    geom_face_n->SetName("Face domain");

    // Set as child
    geom_n->AddChildNode(geom_face_n);
  }

  // Create underlying face norms representation Node
  {
    Handle(ActAPI_INode) geom_face_norms_base = asiData_FaceNormsNode::Instance();
    m_model->GetFaceNormsPartition()->AddNode(geom_face_norms_base);

    // Initialize
    Handle(asiData_FaceNormsNode) geom_face_norms_n = Handle(asiData_FaceNormsNode)::DownCast(geom_face_norms_base);
    geom_face_norms_n->Init();
    geom_face_norms_n->SetName("Face norms");

    // Set as child
    geom_n->AddChildNode(geom_face_norms_n);
  }

  // Create underlying surface representation Node
  {
    Handle(ActAPI_INode) geom_surf_base = asiData_SurfNode::Instance();
    m_model->GetSurfPartition()->AddNode(geom_surf_base);

    // Initialize
    Handle(asiData_SurfNode) geom_surf_n = Handle(asiData_SurfNode)::DownCast(geom_surf_base);
    geom_surf_n->Init();
    geom_surf_n->SetName("Host surface");

    // Set as child
    geom_n->AddChildNode(geom_surf_n);
  }

  // Create underlying contour representation Node
  {
    Handle(ActAPI_INode) geom_face_contour_base = asiData_FaceContourNode::Instance();
    m_model->GetFaceContourPartition()->AddNode(geom_face_contour_base);

    // Initialize
    Handle(asiData_FaceContourNode) geom_face_contour_n = Handle(asiData_FaceContourNode)::DownCast(geom_face_contour_base);
    geom_face_contour_n->Init();
    geom_face_contour_n->SetName("Face contour");

    // Set as child
    geom_n->AddChildNode(geom_face_contour_n);
  }

  // Create underlying edge representation Node
  {
    Handle(ActAPI_INode) geom_edge_base = asiData_EdgeNode::Instance();
    m_model->GetEdgePartition()->AddNode(geom_edge_base);

    // Initialize
    Handle(asiData_EdgeNode) geom_edge_n = Handle(asiData_EdgeNode)::DownCast(geom_edge_base);
    geom_edge_n->Init();
    geom_edge_n->SetName("Edge domain");

    // Set as child
    geom_n->AddChildNode(geom_edge_n);
  }

  // Create underlying curve representation Node
  {
    asiEngine_Curve(m_model).Create("Host curve", geom_n);
  }

  // Create underlying boundary edges representation Node
  {
    Handle(ActAPI_INode) geom_edges_base = asiData_BoundaryEdgesNode::Instance();
    m_model->GetBoundaryEdgesPartition()->AddNode(geom_edges_base);

    // Initialize
    Handle(asiData_BoundaryEdgesNode) geom_edges_n = Handle(asiData_BoundaryEdgesNode)::DownCast(geom_edges_base);
    geom_edges_n->Init();
    geom_edges_n->SetName("Boundary edges");

    // Set as child
    geom_n->AddChildNode(geom_edges_n);
  }

  // Create underlying Contour Node
  {
    Handle(ActAPI_INode) geom_contour_base = asiData_ContourNode::Instance();
    m_model->GetContourPartition()->AddNode(geom_contour_base);

    // Initialize
    Handle(asiData_ContourNode) geom_contour_n = Handle(asiData_ContourNode)::DownCast(geom_contour_base);
    geom_contour_n->Init();
    geom_contour_n->SetName("Custom contour");

    // Set as child
    geom_n->AddChildNode(geom_contour_n);
  }

  // Create underlying Vertex Node
  {
    Handle(ActAPI_INode) geom_vertex_base = asiData_VertexNode::Instance();
    m_model->GetVertexPartition()->AddNode(geom_vertex_base);

    // Initialize
    Handle(asiData_VertexNode) geom_vertex_n = Handle(asiData_VertexNode)::DownCast(geom_vertex_base);
    geom_vertex_n->Init();
    geom_vertex_n->SetName("Vertex");

    // Set as child
    geom_n->AddChildNode(geom_vertex_n);
  }

  // Create underlying Tolerant Shapes Node
  {
    Handle(ActAPI_INode) tolshapes_base = asiData_TolerantShapesNode::Instance();
    m_model->GetTolerantShapesPartition()->AddNode(tolshapes_base);

    // Initialize
    Handle(asiData_TolerantShapesNode)
      tolshapes_n = Handle(asiData_TolerantShapesNode)::DownCast(tolshapes_base);
    //
    tolshapes_n->Init();
    tolshapes_n->SetName("Tolerant shapes");

    // Set as child
    geom_n->AddChildNode(tolshapes_n);
  }

  // Create underlying Metadata Node
  this->CreateMetadata();

  // Return the just created Node
  return geom_n;
}

//-----------------------------------------------------------------------------

bool
  asiEngine_Part::CheckDeviation(const Handle(asiData_IVPointSetNode)& pcNode)
{
  Handle(asiData_DeviationNode) devNode;
  return this->CheckDeviation(pcNode, devNode);
}

//-----------------------------------------------------------------------------

bool
  asiEngine_Part::CheckDeviation(const Handle(asiData_IVPointSetNode)& pcNode,
                                 Handle(asiData_DeviationNode)&        devNode)
{
  // Get Part Node.
  Handle(asiData_PartNode) partNode = m_model->GetPartNode();

  // Check deviations.
  asiAlgo_CheckDeviations checkDeviations( pcNode->GetPoints(),
                                           m_progress,
                                           m_plotter );
  //
  if ( !checkDeviations.Perform( partNode->GetShape() ) )
    return false;

  // Create Deviation Node.
  Handle(ActAPI_INode) devNodeBase = asiData_DeviationNode::Instance();
  m_model->GetDeviationPartition()->AddNode(devNodeBase);

  // Initialize.
  devNode = Handle(asiData_DeviationNode)::DownCast(devNodeBase);
  //
  devNode->Init();
  devNode->SetName("Deviation");

  // Store deviations.
  devNode->SetMeshWithScalars( checkDeviations.GetResult() );

  // Add Deviation Node as a child of the Part Node.
  partNode->AddChildNode(devNode);

  return true;
}

//-----------------------------------------------------------------------------

//! Creates metadata holder.
//! \return Metadata Node.
Handle(asiData_MetadataNode) asiEngine_Part::CreateMetadata()
{
  Handle(ActAPI_INode) metadata_base = asiData_MetadataNode::Instance();
  m_model->GetMetadataPartition()->AddNode(metadata_base);

  // Initialize
  Handle(asiData_MetadataNode)
    metadata_n = Handle(asiData_MetadataNode)::DownCast(metadata_base);
  //
  metadata_n->Init();
  metadata_n->SetName("Metadata");

  // Set as child for the Part Node
  m_model->GetPartNode()->AddChildNode(metadata_n);

  return metadata_n;
}

//-----------------------------------------------------------------------------

//! Creates elementary metadata holder.
//! \param[in] name  name of the Node.
//! \param[in] shape shape.
//! \return Element Metadata Node.
Handle(asiData_ElemMetadataNode)
  asiEngine_Part::CreateElemMetadata(const TCollection_ExtendedString& name,
                                     const TopoDS_Shape&               shape)
{
  Handle(asiData_ElemMetadataNode)
    node = Handle(asiData_ElemMetadataNode)::DownCast( asiData_ElemMetadataNode::Instance() );
  //
  m_model->GetElemMetadataPartition()->AddNode(node);

  // Initialize.
  node->Init();
  node->SetName(name);
  node->SetShape(shape);

  // Set as child for the Metadata Node.
  m_model->GetMetadataNode()->AddChildNode(node);

  // Add reference in the part Node.
  m_model->GetPartNode()->ConnectReferenceToList(asiData_PartNode::PID_MetadataElems,
                                                 node);

  return node;
}

//-----------------------------------------------------------------------------

//! Cleans up metadata.
void asiEngine_Part::CleanMetadata()
{
  this->_cleanChildren( m_model->GetMetadataNode() );
}

//-----------------------------------------------------------------------------

//! Updates metadata available for the Part Node according to the passed
//! modification history.
//! \param[in] history modification history.
void asiEngine_Part::UpdateMetadata(const Handle(asiAlgo_History)& history)
{
  std::vector<Handle(asiData_ElemMetadataDTO)> dtos;

  if ( !history.IsNull() )
  {
    // Get references to metadata elements.
    Handle(ActData_ReferenceListParameter)
      refListParam = ActParamTool::AsReferenceList( m_model->GetPartNode()->Parameter(asiData_PartNode::PID_MetadataElems) );
    //
    Handle(ActAPI_HDataCursorList) refs = refListParam->GetTargets();
    //
    if ( refs.IsNull() )
      return;

    // Iterate over the existing metadata to gather data transfer objects (DTOs)
    // for transferring data.
    for ( ActAPI_HDataCursorList::Iterator it(*refs); it.More(); it.Next() )
    {
      const Handle(asiData_ElemMetadataNode)&
        MN = Handle(asiData_ElemMetadataNode)::DownCast( it.Value() );
      //
      if ( MN.IsNull() || !MN->IsWellFormed() )
        continue;

      // Prepare DTO.
      dtos.push_back( MN->CreateDTO() );
    }
  }

  // Clean up the existing metadata.
  this->CleanMetadata();

  // If there is no history, let's simply clean the metadata.
  if ( history.IsNull() )
    return;

  // Create new metadata objects from the collected DTOs.
  for ( size_t k = 0; k < dtos.size(); ++k )
  {
    std::cout << "\t" << asiAlgo_Utils::ShapeAddrWithPrefix(dtos[k]->Shape) << " >>> ";

    TopoDS_Shape imSh = history->GetLastImageOrArg(dtos[k]->Shape);
    //
    if ( imSh.IsNull() ) // Image is null, i.e., the shape was deleted.
    {
      std::cout << "null" << std::endl;
      continue;
    }
    //
    std::cout << asiAlgo_Utils::ShapeAddrWithPrefix(imSh) << std::endl;

    // Create elementary metadata Node.
    Handle(asiData_ElemMetadataNode)
      EMN = this->CreateElemMetadata(dtos[k]->Name, imSh);
    //
    EMN->SetColor(dtos[k]->Color);
  }
}

//-----------------------------------------------------------------------------

//! \return number of metadata elements.
int asiEngine_Part::GetNumOfMetadata() const
{
  Handle(ActData_ReferenceListParameter)
    refListParam = ActParamTool::AsReferenceList( m_model->GetPartNode()->Parameter(asiData_PartNode::PID_MetadataElems) );
  //
  const int numElems = refListParam->NbTargets();

  return numElems;
}

//-----------------------------------------------------------------------------

//! Gathers all Metadata Element Nodes.
//! \param[out] nodes Metadata Element Nodes.
void asiEngine_Part::GetMetadataElems(Handle(ActAPI_HNodeList)& nodes) const
{
  nodes = new ActAPI_HNodeList;

  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetMetadataNode()->GetChildIterator();
        cit->More(); cit->Next() )
  {
    Handle(asiData_ElemMetadataNode)
      elem_n = Handle(asiData_ElemMetadataNode)::DownCast( cit->Value() );
    //
    if ( !elem_n.IsNull() && elem_n->IsWellFormed() )
      nodes->Append(elem_n);
  }
}

//-----------------------------------------------------------------------------

//! Finds or creates elemental metadata for the passed shape which is normally
//! a sub-shape of the part shape.
//! \param[in] shape  sub-shape in question.
//! \param[in] create whether to create the metadata element if it does not exist.
//! \return found or newly created metadata element.
Handle(asiData_ElemMetadataNode)
  asiEngine_Part::FindElemMetadata(const TopoDS_Shape& shape,
                                   const bool          create)
{
  Handle(asiData_ElemMetadataNode)
    metadataElem_n = m_model->GetMetadataNode()->FindElemMetadata(shape);

  // Create if requested.
  if ( metadataElem_n.IsNull() && create )
  {
    // Prepare name.
    std::string nodeName("Element ");
    nodeName += asiAlgo_Utils::ShapeTypeStr(shape);

    // Create elementary metadata Node.
    metadataElem_n = this->CreateElemMetadata(nodeName.c_str(), shape);
  }

  return metadataElem_n;
}

//-----------------------------------------------------------------------------

//! \return newly created Octree Node.
Handle(asiData_OctreeNode) asiEngine_Part::CreateOctree()
{
  Handle(asiData_OctreeNode)
    node = Handle(asiData_OctreeNode)::DownCast( asiData_OctreeNode::Instance() );
  //
  m_model->GetOctreePartition()->AddNode(node);

  // Initialize.
  node->Init();
  node->SetName("SVO");

  // Set as child for the Part Node.
  m_model->GetPartNode()->AddChildNode(node);

  return node;
}

//-----------------------------------------------------------------------------

//! Finds or creates an Octree Node.
//! \param[in] create whether to create if the Octree Node does not exist.
//! \return found or the newly created Octree Node.
Handle(asiData_OctreeNode) asiEngine_Part::FindOctree(const bool create)
{
  // Get Part Node.
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return nullptr;

  Handle(asiData_OctreeNode) octree_n = part_n->GetOctree();

  // Create if requested.
  if ( octree_n.IsNull() && create )
  {
    // Create Octree Node.
    octree_n = this->CreateOctree();
  }

  return octree_n;
}

//-----------------------------------------------------------------------------

//! Updates part's geometry in a smart way, so all dependent attributes
//! are also actualized.
//! \param[in] model             CAD part to set.
//! \param[in] history           modification history (if available) to
//!                              actualize metadata.
//! \param[in] doResetTessParams indicates whether to reset tessellation
//!                              parameters.
//! \return Part Node.
Handle(asiData_PartNode) asiEngine_Part::Update(const TopoDS_Shape&            model,
                                                const Handle(asiAlgo_History)& history,
                                                const bool                     doResetTessParams)
{
  std::cout << ">>>>>>>>>>>>>>>>>>>>>>> asiEngine_Part::Update << " << std::endl;


  // Get Part Node.
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return part_n;

  // Actualize metadata.
  this->UpdateMetadata(history);

  // Reset data without cleaning up metadata.
  this->Clean(false);

  // Set working structures
  Handle(ActData_ShapeParameter)
    shapeParam = Handle(ActData_ShapeParameter)::DownCast( part_n->Parameter(asiData_PartNode::PID_Geometry) );
  //
  Handle(asiData_AAGParameter)
    aagParam = Handle(asiData_AAGParameter)::DownCast( part_n->Parameter(asiData_PartNode::PID_AAG) );
  //
  shapeParam->SetShape(model);

  // If BVH exists, we clean it up.
  if ( !part_n->GetBVH().IsNull() )
  {
    // Store in OCAF
    Handle(asiData_BVHParameter)
      bvhParam = Handle(asiData_BVHParameter)::DownCast( part_n->Parameter(asiData_PartNode::PID_BVH) );
    //
    bvhParam->SetBVH(nullptr);
  }

  // Build AAG automatically (if not auto-build is not disabled).
  if ( part_n->IsAutoAAG() )
    aagParam->SetAAG( new asiAlgo_AAG(model) );

  // Reset tessellation parameters if requested.
  if ( doResetTessParams )
  {
    part_n->SetLinearDeflection( asiAlgo_MeshGen::AutoSelectLinearDeflection(model) );
    part_n->SetAngularDeflection( asiAlgo_MeshGen::AutoSelectAngularDeflection(model) );
  }

  // Actualize naming if it is initialized.
  if ( part_n->HasNaming() )
    part_n->GetNaming()->Actualize(model);

  // Actualize presentation.
  if ( m_prsMgr )
    m_prsMgr->Actualize(part_n);

  return part_n;
}

//-----------------------------------------------------------------------------

bool asiEngine_Part::HasNaming() const
{
  // Get Part Node.
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return false;

  return part_n->HasNaming();
}

//-----------------------------------------------------------------------------

void asiEngine_Part::InitializeNaming()
{
  // Get Part Node.
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return;

  // Get part shape.
  TopoDS_Shape partShape = part_n->GetShape();
  //
  if ( partShape.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Part contains no B-Rep.");
    return;
  }

  // Prepare naming service.
  Handle(asiAlgo_Naming) naming = new asiAlgo_Naming(partShape, m_progress);
  //
  if ( !naming->InitNames() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Naming initialization failed.");
    return;
  }

  // Set naming service to part.
  Handle(asiData_NamingParameter)
    namingParam = Handle(asiData_NamingParameter)::DownCast( part_n->Parameter(asiData_PartNode::PID_Naming) );
  //
  namingParam->SetNaming(naming);
}

//-----------------------------------------------------------------------------

//! Stores history in the Part Node.
//! \param[in] history history to store.
void asiEngine_Part::StoreHistory(const Handle(asiAlgo_History)& history)
{
  // Get Part Node.
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return;

  // Set naming service to part.
  Handle(asiData_NamingParameter)
    namingParam = Handle(asiData_NamingParameter)::DownCast( part_n->Parameter(asiData_PartNode::PID_Naming) );
  //
  namingParam->SetNaming( new asiAlgo_Naming(history) );
}

//-----------------------------------------------------------------------------

//! Constructs BVH structure for the visualization facets stored in the
//! part shape.
//! \return constructed BVH.
Handle(asiAlgo_BVHFacets) asiEngine_Part::BuildBVH()
{
  // Get Part Node
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();

  // Build BVH for facets
  Handle(asiAlgo_BVHFacets)
    bvh = new asiAlgo_BVHFacets(part_n->GetShape(true),
                                asiAlgo_BVHFacets::Builder_Binned,
                                m_progress,
                                m_plotter);

  // Store in OCAF
  Handle(asiData_BVHParameter)
    bvhParam = Handle(asiData_BVHParameter)::DownCast( part_n->Parameter(asiData_PartNode::PID_BVH) );
  //
  bvhParam->SetBVH(bvh);

  return bvh;
}

//-----------------------------------------------------------------------------

//! Sets octree for the part.
//! \param[in] pOctree octree to set.
void asiEngine_Part::SetOctree(void* pOctree)
{
  // Get Octree Node.
  Handle(asiData_OctreeNode) octree_n = this->FindOctree(true);

  // Store in OCAF.
  if ( !octree_n.IsNull() && octree_n->IsWellFormed() )
    octree_n->SetOctree(pOctree);
}

//-----------------------------------------------------------------------------

//! Cleans up Data Model structure related to the Part Node.
void asiEngine_Part::Clean(const bool cleanMeta)
{
  std::cout << ">>>>>>>>>>>>> asiEngine_Part::Clean" << std::endl;

  // Get Part Node.
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return;

  // Reset data.
  part_n->GetFaceRepresentation()          ->Init();
  part_n->GetSurfaceRepresentation()       ->Init();
  part_n->GetEdgeRepresentation()          ->Init();
  part_n->GetCurveRepresentation()         ->Init();
  part_n->GetBoundaryEdgesRepresentation() ->Init();
  part_n->SetTransformation(0., 0., 0., 0., 0., 0.);

  // Clean up tolerant shapes.
  asiEngine_TolerantShapes tolApi(m_model, m_prsMgr, m_progress, m_plotter);
  //
  tolApi.Clean_All();

  // Clean up metadata.
  if ( cleanMeta )
    this->CleanMetadata();
}

//-----------------------------------------------------------------------------

//! Accessor for a transient pointer to a B-Rep face by its one-based index.
//! \param[in] oneBasedId one-based index of a face to access.
//! \return transient pointer to a face.
TopoDS_Face asiEngine_Part::GetFace(const int oneBasedId)
{
  // Get Part Node
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return TopoDS_Face();

  // Get AAG.
  Handle(asiAlgo_AAG) aag = part_n->GetAAG();
  //
  if ( aag.IsNull() )
    return TopoDS_Face();

  // Get face.
  if ( !aag->HasFace(oneBasedId) )
    return TopoDS_Face();

  return aag->GetFace(oneBasedId);
}

//-----------------------------------------------------------------------------

//! \return shape stored in Part Node.
TopoDS_Shape asiEngine_Part::GetShape()
{
  // Get Part Node
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return TopoDS_Shape();

  return part_n->GetShape();
}

//-----------------------------------------------------------------------------

//! Extracts sub-shape indices for the given collection of face indices.
//! \param[in]  faceIndices indices of faces.
//! \param[out] indices     their corresponding indices among all sub-shapes.
void asiEngine_Part::GetSubShapeIndicesByFaceIndices(const TColStd_PackedMapOfInteger& faceIndices,
                                                     TColStd_PackedMapOfInteger&       indices)
{
  const TopTools_IndexedMapOfShape&
    AllFaces = m_model->GetPartNode()->GetAAG()->GetMapOfFaces();
  //
  TopTools_IndexedMapOfShape SelectedFaces;

  // Get selected faces in topological form
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(faceIndices); fit.More(); fit.Next() )
  {
    const int input_face_idx = fit.Key();
    SelectedFaces.Add( AllFaces.FindKey(input_face_idx) );
  }

  // Get indices of the faces among all sub-shapes
  GetSubShapeIndices(SelectedFaces, indices);
}

//-----------------------------------------------------------------------------

//! Extracts sub-shape indices for the given collection of edge indices.
//! \param[in]  edgeIndices indices of edges.
//! \param[out] indices     their corresponding indices among all sub-shapes.
void asiEngine_Part::GetSubShapeIndicesByEdgeIndices(const TColStd_PackedMapOfInteger& edgeIndices,
                                                     TColStd_PackedMapOfInteger&       indices)
{
  const TopTools_IndexedMapOfShape&
    AllEdges = m_model->GetPartNode()->GetAAG()->RequestMapOfEdges();
  //
  TopTools_IndexedMapOfShape SelectedEdges;

  // Get selected edges in topological form
  for ( TColStd_MapIteratorOfPackedMapOfInteger fit(edgeIndices); fit.More(); fit.Next() )
  {
    const int input_edge_idx = fit.Key();
    SelectedEdges.Add( AllEdges.FindKey(input_edge_idx) );
  }

  // Get indices of the edges among all sub-shapes
  GetSubShapeIndices(SelectedEdges, indices);
}

//-----------------------------------------------------------------------------

//! Extracts sub-shape indices for the given collection of sub-shapes.
//! \param[in]  subShapes sub-shapes of interest.
//! \param[out] indices   their corresponding IDs.
void asiEngine_Part::GetSubShapeIndices(const TopTools_IndexedMapOfShape& subShapes,
                                        TColStd_PackedMapOfInteger&       indices)
{
  Handle(asiAlgo_AAG) aag = m_model->GetPartNode()->GetAAG();
  //
  if ( aag.IsNull() )
    return;

  const TopTools_IndexedMapOfShape& M = aag->RequestMapOfSubShapes();
  //
  for ( int i = 1; i <= subShapes.Extent(); ++i )
    indices.Add( M.FindIndex( subShapes.FindKey(i) ) );
}

//-----------------------------------------------------------------------------

//! Extracts sub-shape indices for the given collection of sub-shapes. The
//! output is distributed by faces, edges and vertices.
//! \param[in]  subShapes     sub-shapes of interest.
//! \param[out] faceIndices   global indices for faces.
//! \param[out] edgeIndices   global indices for edges.
//! \param[out] vertexIndices global indices for vertices.
void asiEngine_Part::GetSubShapeIndices(const TopTools_IndexedMapOfShape& subShapes,
                                        TColStd_PackedMapOfInteger&       faceIndices,
                                        TColStd_PackedMapOfInteger&       edgeIndices,
                                        TColStd_PackedMapOfInteger&       vertexIndices)
{
  const TopTools_IndexedMapOfShape&
    M = m_model->GetPartNode()->GetAAG()->RequestMapOfSubShapes();
  //
  for ( int i = 1; i <= subShapes.Extent(); ++i )
  {
    const TopoDS_Shape& sh = subShapes.FindKey(i);

    if ( sh.ShapeType() == TopAbs_FACE )
      faceIndices.Add( M.FindIndex(sh) );
    //
    else if ( sh.ShapeType() == TopAbs_WIRE )
    {
      for ( TopExp_Explorer eexp(sh, TopAbs_EDGE); eexp.More(); eexp.Next() )
        edgeIndices.Add( M.FindIndex( eexp.Current() ) );
    }
    //
    else if ( sh.ShapeType() == TopAbs_EDGE )
      edgeIndices.Add( M.FindIndex(sh) );
    //
    else if ( sh.ShapeType() == TopAbs_VERTEX )
      vertexIndices.Add( M.FindIndex(sh) );
  }
}

//-----------------------------------------------------------------------------

//! Highlights a single face.
//! \param[in] faceIndex face to highlight.
void asiEngine_Part::HighlightFace(const int faceIndex)
{
  // Prepare a fictive collection
  TColStd_PackedMapOfInteger faceIndices;
  faceIndices.Add(faceIndex);

  // Highlight
  HighlightFaces(faceIndices);
}

//-----------------------------------------------------------------------------

//! Highlights faces.
//! \param[in] faceIndices faces to highlight.
void asiEngine_Part::HighlightFaces(const TColStd_PackedMapOfInteger& faceIndices)
{
  // Convert face indices to sub-shape indices
  TColStd_PackedMapOfInteger ssIndices;
  GetSubShapeIndicesByFaceIndices(faceIndices, ssIndices);

  // Highlight
  HighlightSubShapes(ssIndices, SelectionMode_Face);
}

//-----------------------------------------------------------------------------

//! Highlights edges.
//! \param[in] edgeIndices edges to highlight.
void asiEngine_Part::HighlightEdges(const TColStd_PackedMapOfInteger& edgeIndices)
{
  // Convert edge indices to sub-shape indices
  TColStd_PackedMapOfInteger ssIndices;
  GetSubShapeIndicesByEdgeIndices(edgeIndices, ssIndices);

  // Highlight
  HighlightSubShapes(ssIndices, SelectionMode_Edge);
}

//-----------------------------------------------------------------------------

//! Highlights the passed sub-shapes identified by their indices.
//! \param[in] subShapeIndices indices of the sub-shapes to highlight.
//! \param[in] selMode         selection mode.
void asiEngine_Part::HighlightSubShapes(const TColStd_PackedMapOfInteger& subShapeIndices,
                                        const asiVisu_SelectionMode       selMode)
{
  // Get Part Node.
  Handle(asiData_PartNode) N = m_model->GetPartNode();

  // Get Presentation for the Part Node.
  Handle(asiVisu_PartPrs)
    prs = Handle(asiVisu_PartPrs)::DownCast( m_prsMgr->GetPresentation(N) );

  if ( prs.IsNull() )
    return;

  // Make sure to restore the previous selection mode.
  const int prevMode = m_prsMgr->GetCurrentSelection().GetSelectionModes();
  {
    // Highlight
    if ( selMode == SelectionMode_Face )
      m_prsMgr->Highlight(N, prs->MainActor(), subShapeIndices, selMode);
    else if ( selMode == SelectionMode_Edge )
      m_prsMgr->Highlight(N, prs->ContourActor(), subShapeIndices, selMode);
  }
  m_prsMgr->ChangeCurrentSelection().SetSelectionModes(prevMode);
}

//-----------------------------------------------------------------------------

//! Highlights the passed sub-shapes in Part Viewer.
//! \param[in] subShapes sub-shapes to highlight.
void asiEngine_Part::HighlightSubShapes(const TopTools_IndexedMapOfShape& subShapes)
{
  // Get global relative indices of the sub-shapes in the CAD model
  TColStd_PackedMapOfInteger selectedFaces, selectedEdges, selectedVertices;
  asiEngine_Part::GetSubShapeIndices(subShapes, selectedFaces, selectedEdges, selectedVertices);

  // Highlight
  if ( !selectedFaces.IsEmpty() )
    HighlightSubShapes(selectedFaces, SelectionMode_Face);
  //
  if ( !selectedEdges.IsEmpty() )
    HighlightSubShapes(selectedEdges, SelectionMode_Edge);
  //
  if ( !selectedVertices.IsEmpty() )
    HighlightSubShapes(selectedVertices, SelectionMode_Vertex);
}

//-----------------------------------------------------------------------------

//! Retrieves highlighted sub-shapes from the viewer.
//! \param[out] subShapes result collection.
void asiEngine_Part::GetHighlightedSubShapes(TopTools_IndexedMapOfShape& subShapes)
{
  Handle(asiAlgo_AAG) aag = m_model->GetPartNode()->GetAAG();
  //
  if ( aag.IsNull() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "AAG is null.");
    return;
  }

  if ( !m_prsMgr.GetPointer() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Presentation manager is null.");
    return;
  }

  // Get the map of ALL shapes to extract topology by selected index which
  // is global (related to full accessory graph)
  const TopTools_IndexedMapOfShape& M = aag->RequestMapOfSubShapes();

  // Get actual selection
  const asiVisu_ActualSelection&          sel      = m_prsMgr->GetCurrentSelection();
  const Handle(asiVisu_CellPickerResult)& pick_res = sel.GetCellPickerResult(SelectionNature_Persistent);
  //
  asiVisu_PartNodeInfo* nodeInfo = asiVisu_PartNodeInfo::Retrieve( pick_res->GetPickedActor() );
  //
  if ( !nodeInfo )
    return;

  const TColStd_PackedMapOfInteger& subshape_mask = pick_res->GetPickedElementIds();
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger mit(subshape_mask); mit.More(); mit.Next() )
  {
    const int           subshape_idx = mit.Key();
    const TopoDS_Shape& subshape     = M.FindKey(subshape_idx);
    //
    subShapes.Add(subshape);
  }
}

//-----------------------------------------------------------------------------

//! Retrieves indices of the highlighted faces.
//! \param[out] faceIndices indices of the highlighted faces.
void asiEngine_Part::GetHighlightedFaces(TColStd_PackedMapOfInteger& faceIndices)
{
  TopTools_IndexedMapOfShape subShapes;
  GetHighlightedSubShapes(subShapes);
  //
  if ( subShapes.IsEmpty() )
    return;

  // Get part
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return;

  // Get AAG
  Handle(asiAlgo_AAG) aag = part_n->GetAAG();
  //
  if ( aag.IsNull() )
    return;

  // Take all faces
  const TopTools_IndexedMapOfShape& allFaces = aag->GetMapOfFaces();

  // Filter out non-selected faces
  for ( int f = 1; f <= allFaces.Extent(); ++f )
  {
    if ( subShapes.Contains( allFaces(f) ) )
      faceIndices.Add(f);
  }
}

//-----------------------------------------------------------------------------

//! Retrieves indices of the highlighted edges.
//! \param[out] edgeIndices indices of the highlighted edges.
void asiEngine_Part::GetHighlightedEdges(TColStd_PackedMapOfInteger& edgeIndices)
{
  TopTools_IndexedMapOfShape subShapes;
  GetHighlightedSubShapes(subShapes);
  //
  if ( subShapes.IsEmpty() )
    return;

  // Take all edges
  const TopTools_IndexedMapOfShape&
    allEdges = m_model->GetPartNode()->GetAAG()->RequestMapOfEdges();

  // Filter out non-selected edges
  for ( int e = 1; e <= allEdges.Extent(); ++e )
  {
    if ( subShapes.Contains( allEdges(e) ) )
      edgeIndices.Add(e);
  }
}
