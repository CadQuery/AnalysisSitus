//-----------------------------------------------------------------------------
// Created on: 26 November 2015
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiEngine_Model.h>

// asiEngine includes
#include <asiEngine_Curve.h>
#include <asiEngine_IV.h>
#include <asiEngine_Part.h>
#include <asiEngine_Tessellation.h>
#include <asiEngine_TolerantShapes.h>
#include <asiEngine_Triangulation.h>

// Active Data includes
#include <ActData_CAFConverter.h>
#include <ActData_RealEvaluatorFunc.h>
#include <ActData_RealVarNode.h>
#include <ActData_RealVarPartition.h>
#include <ActData_UniqueNodeName.h>
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Register Node types
//-----------------------------------------------------------------------------

// Shipped with Active Data
REGISTER_NODE_TYPE(ActData_RealVarNode)

// Custom Nodes
REGISTER_NODE_TYPE(asiData_RootNode)
//
REGISTER_NODE_TYPE(asiData_PartNode)
REGISTER_NODE_TYPE(asiData_FaceNode)
REGISTER_NODE_TYPE(asiData_FaceNormsNode)
REGISTER_NODE_TYPE(asiData_FaceContourNode)
REGISTER_NODE_TYPE(asiData_SurfNode)
REGISTER_NODE_TYPE(asiData_EdgeNode)
REGISTER_NODE_TYPE(asiData_CurveNode)
REGISTER_NODE_TYPE(asiData_CurvatureCombsNode)
REGISTER_NODE_TYPE(asiData_BoundaryEdgesNode)
REGISTER_NODE_TYPE(asiData_ContourNode)
REGISTER_NODE_TYPE(asiData_VertexNode)
REGISTER_NODE_TYPE(asiData_TolerantShapesNode)
REGISTER_NODE_TYPE(asiData_TolerantRangeNode)
//
REGISTER_NODE_TYPE(asiData_TriangulationNode)
REGISTER_NODE_TYPE(asiData_TessNode)
REGISTER_NODE_TYPE(asiData_TessNormsNode)
//
REGISTER_NODE_TYPE(asiData_IVCurveNode)
REGISTER_NODE_TYPE(asiData_IVCurvesNode)
REGISTER_NODE_TYPE(asiData_IVCurve2dNode)
REGISTER_NODE_TYPE(asiData_IVCurves2dNode)
REGISTER_NODE_TYPE(asiData_IVNode)
REGISTER_NODE_TYPE(asiData_IVPoints2dNode)
REGISTER_NODE_TYPE(asiData_IVPointsNode)
REGISTER_NODE_TYPE(asiData_IVPointSet2dNode)
REGISTER_NODE_TYPE(asiData_IVPointSetNode)
REGISTER_NODE_TYPE(asiData_IVSurfaceNode)
REGISTER_NODE_TYPE(asiData_IVSurfacesNode)
REGISTER_NODE_TYPE(asiData_IVTessItemNode)
REGISTER_NODE_TYPE(asiData_IVTessNode)
REGISTER_NODE_TYPE(asiData_IVTopoItemNode)
REGISTER_NODE_TYPE(asiData_IVTopoNode)
REGISTER_NODE_TYPE(asiData_IVTextItemNode)
REGISTER_NODE_TYPE(asiData_IVTextNode)

//-----------------------------------------------------------------------------

//! Default constructor. Initializes Base Model foundation object so that
//! to enable Extended Transaction Mode.
asiEngine_Model::asiEngine_Model() : ActData_BaseModel(true)
{}

//-----------------------------------------------------------------------------

//! Populates Data Model.
void asiEngine_Model::Populate()
{
  // Add root Node
  Handle(asiData_RootNode)
    root_n = Handle(asiData_RootNode)::DownCast( asiData_RootNode::Instance() );
  //
  this->GetRootPartition()->AddNode(root_n);

  // Set name
  root_n->SetName("Analysis Situs");

  // Add Part Node
  root_n->AddChildNode( asiEngine_Part(this, NULL).CreatePart() );

  // Add Triangulation Node
  root_n->AddChildNode( asiEngine_Triangulation(this, NULL).CreateTriangulation() );

  // Add Tessellation Node
  root_n->AddChildNode( asiEngine_Tessellation(this, NULL).CreateTessellation() );

  // Add Imperative Viewer Node
  root_n->AddChildNode( asiEngine_IV(this).Create_IV() );
}

//-----------------------------------------------------------------------------

//! Clears the Model.
void asiEngine_Model::Clear()
{
  // NOTE: Part Node is not touched as it is structural. No sense in
  //       removing it since we will have to create it again once a new
  //       part is loaded. The same rule applies to other structural Nodes.

  // Perform deletion
  this->OpenCommand(); // tx start
  {
    // Clean up persistent selection
    this->GetPartNode()->GetFaceRepresentation()    ->SetSelectedFace   (0);
    this->GetPartNode()->GetNormsRepresentation()   ->SetSelectedFace   (0);
    this->GetPartNode()->GetSurfaceRepresentation() ->SetSelectedFace   (0);
    this->GetPartNode()->GetContourRepresentation() ->SetSelectedFace   (0);
    this->GetPartNode()->GetEdgeRepresentation()    ->SetSelectedEdge   (0);
    this->GetPartNode()->GetCurveRepresentation()   ->SetSelectedEdge   (0);
    this->GetPartNode()->GetVertexRepresentation()  ->SetSelectedVertex (0);

    // Delete nodes under the Curve
    asiEngine_Curve(this).Clean_All( this->GetPartNode()->GetCurveRepresentation() );

    // Delete all tolerant ranges.
    asiEngine_TolerantShapes(this, NULL).Clean_All();

    // Delete all Nodes serving imperative visualization
    asiEngine_IV(this).Clean_All();

    // Let sub-classes do some job
    this->clearCustom();
  }
  this->CommitCommand(); // tx end
}

//-----------------------------------------------------------------------------

//! \return single Geometry Part Node.
Handle(asiData_PartNode) asiEngine_Model::GetPartNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetPartPartition() ); it.More(); it.Next() )
  {
    Handle(asiData_PartNode) N = Handle(asiData_PartNode)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//-----------------------------------------------------------------------------

//! \return single Triangulation Node.
Handle(asiData_TriangulationNode) asiEngine_Model::GetTriangulationNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetTriangulationPartition() ); it.More(); it.Next() )
  {
    Handle(asiData_TriangulationNode) N = Handle(asiData_TriangulationNode)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}


//-----------------------------------------------------------------------------

//! \return single Tessellation Node.
Handle(asiData_TessNode) asiEngine_Model::GetTessellationNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetTessellationPartition() ); it.More(); it.Next() )
  {
    Handle(asiData_TessNode) N = Handle(asiData_TessNode)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//-----------------------------------------------------------------------------

//! \return single Imperative Viewer Node.
Handle(asiData_IVNode) asiEngine_Model::GetIVNode() const
{
  for ( ActData_BasePartition::Iterator it( this->GetIVPartition() ); it.More(); it.Next() )
  {
    Handle(asiData_IVNode) N = Handle(asiData_IVNode)::DownCast( it.Value() );
    //
    if ( !N.IsNull() && N->IsWellFormed() )
      return N;
  }
  return NULL;
}

//-----------------------------------------------------------------------------

//! Initializes Partitions.
void asiEngine_Model::initPartitions()
{
  REGISTER_PARTITION(asiData_Partition<asiData_RootNode>,           Partition_Root);
  //
  REGISTER_PARTITION(asiData_Partition<asiData_PartNode>,           Partition_Part);
  REGISTER_PARTITION(asiData_Partition<asiData_FaceNode>,           Partition_Face);
  REGISTER_PARTITION(asiData_Partition<asiData_FaceNormsNode>,      Partition_FaceNorms);
  REGISTER_PARTITION(asiData_Partition<asiData_FaceContourNode>,    Partition_FaceContour);
  REGISTER_PARTITION(asiData_Partition<asiData_SurfNode>,           Partition_Surf);
  REGISTER_PARTITION(asiData_Partition<asiData_EdgeNode>,           Partition_Edge);
  REGISTER_PARTITION(asiData_Partition<asiData_CurveNode>,          Partition_Curve);
  REGISTER_PARTITION(asiData_Partition<asiData_CurvatureCombsNode>, Partition_CurvatureCombs);
  REGISTER_PARTITION(asiData_Partition<asiData_BoundaryEdgesNode>,  Partition_BoundaryEdges);
  REGISTER_PARTITION(asiData_Partition<asiData_ContourNode>,        Partition_Contour);
  REGISTER_PARTITION(asiData_Partition<asiData_VertexNode>,         Partition_Vertex);
  REGISTER_PARTITION(asiData_Partition<asiData_TolerantShapesNode>, Partition_TolerantShapes);
  REGISTER_PARTITION(asiData_Partition<asiData_TolerantRangeNode>,  Partition_TolerantRange);
  //
  REGISTER_PARTITION(asiData_Partition<asiData_TriangulationNode>,  Partition_Triangulation);
  REGISTER_PARTITION(asiData_Partition<asiData_TessNode>,           Partition_Tessellation);
  REGISTER_PARTITION(asiData_Partition<asiData_TessNormsNode>,      Partition_TessellationNorms);
  //
  REGISTER_PARTITION(asiData_Partition<asiData_IVNode>,             Partition_IV);
  REGISTER_PARTITION(asiData_Partition<asiData_IVPoints2dNode>,     Partition_IV_Points2d);
  REGISTER_PARTITION(asiData_Partition<asiData_IVPointsNode>,       Partition_IV_Points);
  REGISTER_PARTITION(asiData_Partition<asiData_IVPointSet2dNode>,   Partition_IV_PointSet2d);
  REGISTER_PARTITION(asiData_Partition<asiData_IVPointSetNode>,     Partition_IV_PointSet);
  REGISTER_PARTITION(asiData_Partition<asiData_IVCurvesNode>,       Partition_IV_Curves);
  REGISTER_PARTITION(asiData_Partition<asiData_IVCurveNode>,        Partition_IV_Curve);
  REGISTER_PARTITION(asiData_Partition<asiData_IVCurves2dNode>,     Partition_IV_Curves2d);
  REGISTER_PARTITION(asiData_Partition<asiData_IVCurve2dNode>,      Partition_IV_Curve2d);
  REGISTER_PARTITION(asiData_Partition<asiData_IVSurfacesNode>,     Partition_IV_Surfaces);
  REGISTER_PARTITION(asiData_Partition<asiData_IVSurfaceNode>,      Partition_IV_Surface);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTopoNode>,         Partition_IV_Topo);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTopoItemNode>,     Partition_IV_TopoItem);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTessNode>,         Partition_IV_Tess);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTessItemNode>,     Partition_IV_TessItem);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTextNode>,         Partition_IV_Text);
  REGISTER_PARTITION(asiData_Partition<asiData_IVTextItemNode>,     Partition_IV_TextItem);
}

//-----------------------------------------------------------------------------

//! Initializes the Tree Functions bound to the Data Model.
void asiEngine_Model::initFunctionDrivers()
{
  REGISTER_TREE_FUNCTION(ActData_RealEvaluatorFunc);
}

//-----------------------------------------------------------------------------

//! Returns a Partition of Data Nodes representing Variables for Expression
//! Evaluation mechanism.
//! \param[in] varType type of Variable to return the dedicated Partition for.
//! \return Variable Partition.
Handle(ActAPI_IPartition)
  asiEngine_Model::getVariablePartition(const VariableType& varType) const
{
  switch ( varType )
  {
    case Variable_Real:
      return this->Partition(Partition_RealVar);
    default:
      break;
  }
  return NULL;
}

//-----------------------------------------------------------------------------

//! Accessor for the root Project Node.
//! \return root Project Node.
Handle(ActAPI_INode) asiEngine_Model::getRootNode() const
{
  asiData_Partition<asiData_RootNode>::Iterator anIt( this->GetRootPartition() );
  return ( anIt.More() ? anIt.Value() : NULL );
}

//-----------------------------------------------------------------------------

//! Populates the passed collections of references to pass out-scope filtering
//! in Copy/Paste operation.
//! \param[in,out] FuncGUIDs Function GUIDs to pass.
//! \param[in,out] Refs      Reference Parameters to pass.
void asiEngine_Model::invariantCopyRefs(ActAPI_FuncGUIDStream&         FuncGUIDs,
                                        ActAPI_ParameterLocatorStream& Refs) const
{
  asiEngine_NotUsed(FuncGUIDs);
  asiEngine_NotUsed(Refs);
}

//-----------------------------------------------------------------------------

//! Returns version of Data Model.
//! \return current version of Data Model.
int asiEngine_Model::actualVersionApp()
{
  return 0x0;
}

//-----------------------------------------------------------------------------

//! Callback supplying CAF converter required to perform application-specific
//! conversion of Data Model from older version of the application to the
//! recent one.
//! \return properly initialized CAF converter.
Handle(ActData_CAFConverter) asiEngine_Model::converterApp()
{
  return NULL;
}

//-----------------------------------------------------------------------------

//! Callback for custom Data Model clean up logic.
void asiEngine_Model::clearCustom()
{}
