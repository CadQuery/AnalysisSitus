//-----------------------------------------------------------------------------
// Created on: 26 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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

#ifndef asiEngine_Model_h
#define asiEngine_Model_h

// asiEngine includes
#include <asiEngine.h>

// asiData includes
#include <asiData_BoundaryEdgesNode.h>
#include <asiData_CurvatureCombsNode.h>
#include <asiData_IVCurve2dNode.h>
#include <asiData_IVCurveNode.h>
#include <asiData_IVCurves2dNode.h>
#include <asiData_IVCurvesNode.h>
#include <asiData_IVNode.h>
#include <asiData_IVPointSet2dNode.h>
#include <asiData_IVPointSetNode.h>
#include <asiData_IVPoints2dNode.h>
#include <asiData_IVPointsNode.h>
#include <asiData_IVSurfaceNode.h>
#include <asiData_IVSurfacesNode.h>
#include <asiData_IVTessItemNode.h>
#include <asiData_IVTessNode.h>
#include <asiData_IVTextItemNode.h>
#include <asiData_IVTextNode.h>
#include <asiData_IVTopoItemNode.h>
#include <asiData_IVTopoNode.h>
#include <asiData_Partition.h>
#include <asiData_PartNode.h>
#include <asiData_ReCoEdgeNode.h>
#include <asiData_ReEdgeNode.h>
#include <asiData_ReEdgesNode.h>
#include <asiData_ReTopoNode.h>
#include <asiData_RePatchesNode.h>
#include <asiData_RePatchNode.h>
#include <asiData_ReVertexNode.h>
#include <asiData_ReVerticesNode.h>
#include <asiData_RootNode.h>
#include <asiData_SurfDeviationNode.h>
#include <asiData_TessNode.h>
#include <asiData_TessNormsNode.h>
#include <asiData_TolerantRangeNode.h>
#include <asiData_TriangulationNode.h>

// Active Data includes
#include <ActData_BaseModel.h>

//-----------------------------------------------------------------------------

//! Standard implementation of Data Model in Analysis Situs. This
//! standard implementation answers many typical needs related to analysis.
//! However, in the prototyping scope you normally need something more.
//! If the basic data model is not enough, you may either extend it with your
//! custom Data Nodes or implement a brand new one. In the latter case you
//! cannot use some handy services from the Engine package (e.g.
//! initialization of Part Nodes).
class asiEngine_Model : public ActData_BaseModel
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiEngine_Model, ActData_BaseModel)

public:

  asiEngine_EXPORT
    asiEngine_Model();

//-----------------------------------------------------------------------------
// Populate and clear:
public:

  asiEngine_EXPORT virtual void
    Populate();

  asiEngine_EXPORT virtual void
    Clear();

//-----------------------------------------------------------------------------
// Accessors to Nodes:
public:

  asiEngine_EXPORT Handle(asiData_PartNode)
    GetPartNode() const;

  asiEngine_EXPORT Handle(asiData_TriangulationNode)
    GetTriangulationNode() const;

  asiEngine_EXPORT Handle(asiData_TessNode)
    GetTessellationNode() const;

  asiEngine_EXPORT Handle(asiData_IVNode)
    GetIVNode() const;

  asiEngine_EXPORT Handle(asiData_ReTopoNode)
    GetReTopoNode() const;

//-----------------------------------------------------------------------------
// Overridden:
public:

  //! Create a cloned instance of Data Model.
  //! \return cloned instance.
  virtual Handle(ActAPI_IModel) Clone() const
  {
    return ActData_BaseModel::CloneInstance<asiEngine_Model>();
  }

//-----------------------------------------------------------------------------
// Partitions:
public:

  //! Accessor for a Partition instance dedicated to Root Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RootNode>) GetRootPartition() const
  {
    return Handle(asiData_Partition<asiData_RootNode>)::DownCast( this->Partition(Partition_Root) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Part Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_PartNode>) GetPartPartition() const
  {
    return Handle(asiData_Partition<asiData_PartNode>)::DownCast( this->Partition(Partition_Part) );
  }

  //! Accessor for a Partition instance dedicated to Face Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_FaceNode>) GetFacePartition() const
  {
    return Handle(asiData_Partition<asiData_FaceNode>)::DownCast( this->Partition(Partition_Face) );
  }

  //! Accessor for a Partition instance dedicated to Face Norms Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_FaceNormsNode>) GetFaceNormsPartition() const
  {
    return Handle(asiData_Partition<asiData_FaceNormsNode>)::DownCast( this->Partition(Partition_FaceNorms) );
  }

  //! Accessor for a Partition instance dedicated to Surf Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_SurfNode>) GetSurfPartition() const
  {
    return Handle(asiData_Partition<asiData_SurfNode>)::DownCast( this->Partition(Partition_Surf) );
  }

  //! Accessor for a Partition instance dedicated to Surface Deviation Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_SurfDeviationNode>) GetSurfDeviationPartition() const
  {
    return Handle(asiData_Partition<asiData_SurfDeviationNode>)::DownCast( this->Partition(Partition_SurfDeviation) );
  }

  //! Accessor for a Partition instance dedicated to Face Contour Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_FaceContourNode>) GetFaceContourPartition() const
  {
    return Handle(asiData_Partition<asiData_FaceContourNode>)::DownCast( this->Partition(Partition_FaceContour) );
  }

  //! Accessor for a Partition instance dedicated to Edge Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_EdgeNode>) GetEdgePartition() const
  {
    return Handle(asiData_Partition<asiData_EdgeNode>)::DownCast( this->Partition(Partition_Edge) );
  }

  //! Accessor for a Partition instance dedicated to Curve Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_CurveNode>) GetCurvePartition() const
  {
    return Handle(asiData_Partition<asiData_CurveNode>)::DownCast( this->Partition(Partition_Curve) );
  }

  //! Accessor for a Partition instance dedicated to Curvature Combs Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_CurvatureCombsNode>) GetCurvatureCombsPartition() const
  {
    return Handle(asiData_Partition<asiData_CurvatureCombsNode>)::DownCast( this->Partition(Partition_CurvatureCombs) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Boundary Edges Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_BoundaryEdgesNode>) GetBoundaryEdgesPartition() const
  {
    return Handle(asiData_Partition<asiData_BoundaryEdgesNode>)::DownCast( this->Partition(Partition_BoundaryEdges) );
  }

  //! Accessor for a Partition instance dedicated to Contour Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_ContourNode>) GetContourPartition() const
  {
    return Handle(asiData_Partition<asiData_ContourNode>)::DownCast( this->Partition(Partition_Contour) );
  }

  //! Accessor for a Partition instance dedicated to Vertex Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_VertexNode>) GetVertexPartition() const
  {
    return Handle(asiData_Partition<asiData_VertexNode>)::DownCast( this->Partition(Partition_Vertex) );
  }

  //! Accessor for a Partition instance dedicated to Tolerant Shapes Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_TolerantShapesNode>) GetTolerantShapesPartition() const
  {
    return Handle(asiData_Partition<asiData_TolerantShapesNode>)::DownCast( this->Partition(Partition_TolerantShapes) );
  }

  //! Accessor for a Partition instance dedicated to Tolerant Range Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_TolerantRangeNode>) GetTolerantRangePartition() const
  {
    return Handle(asiData_Partition<asiData_TolerantRangeNode>)::DownCast( this->Partition(Partition_TolerantRange) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Triangulation Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_TriangulationNode>) GetTriangulationPartition() const
  {
    return Handle(asiData_Partition<asiData_TriangulationNode>)::DownCast( this->Partition(Partition_Triangulation) );
  }

  //! Accessor for a Partition instance dedicated to Tessellation Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_TessNode>) GetTessellationPartition() const
  {
    return Handle(asiData_Partition<asiData_TessNode>)::DownCast( this->Partition(Partition_Tessellation) );
  }

  //! Accessor for a Partition instance dedicated to Tessellation Norms Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_TessNormsNode>) GetTessellationNormsPartition() const
  {
    return Handle(asiData_Partition<asiData_TessNormsNode>)::DownCast( this->Partition(Partition_TessellationNorms) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to ReTopo Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_ReTopoNode>) GetReTopoPartition() const
  {
    return Handle(asiData_Partition<asiData_ReTopoNode>)::DownCast( this->Partition(Partition_ReTopo) );
  }

  //! Accessor for a Partition instance dedicated to RePatch Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RePatchNode>) GetRePatchPartition() const
  {
    return Handle(asiData_Partition<asiData_RePatchNode>)::DownCast( this->Partition(Partition_RePatch) );
  }

  //! Accessor for a Partition instance dedicated to RePatches Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RePatchesNode>) GetRePatchesPartition() const
  {
    return Handle(asiData_Partition<asiData_RePatchesNode>)::DownCast( this->Partition(Partition_RePatches) );
  }

  //! Accessor for a Partition instance dedicated to CoEdge Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_ReCoEdgeNode>) GetReCoEdgePartition() const
  {
    return Handle(asiData_Partition<asiData_ReCoEdgeNode>)::DownCast( this->Partition(Partition_ReCoEdge) );
  }

  //! Accessor for a Partition instance dedicated to ReEdge Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_ReEdgeNode>) GetReEdgePartition() const
  {
    return Handle(asiData_Partition<asiData_ReEdgeNode>)::DownCast( this->Partition(Partition_ReEdge) );
  }

  //! Accessor for a Partition instance dedicated to ReEdges Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_ReEdgesNode>) GetReEdgesPartition() const
  {
    return Handle(asiData_Partition<asiData_ReEdgesNode>)::DownCast( this->Partition(Partition_ReEdges) );
  }

  //! Accessor for a Partition instance dedicated to ReVertex Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_ReVertexNode>) GetReVertexPartition() const
  {
    return Handle(asiData_Partition<asiData_ReVertexNode>)::DownCast( this->Partition(Partition_ReVertex) );
  }

  //! Accessor for a Partition instance dedicated to ReVertices Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_ReVerticesNode>) GetReVerticesPartition() const
  {
    return Handle(asiData_Partition<asiData_ReVerticesNode>)::DownCast( this->Partition(Partition_ReVertices) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to IV Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVNode>) GetIVPartition() const
  {
    return Handle(asiData_Partition<asiData_IVNode>)::DownCast( this->Partition(Partition_IV) );
  }

  //! Accessor for a Partition instance dedicated to IV Points 2D Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVPoints2dNode>) GetIVPoints2dPartition() const
  {
    return Handle(asiData_Partition<asiData_IVPoints2dNode>)::DownCast( this->Partition(Partition_IV_Points2d) );
  }

  //! Accessor for a Partition instance dedicated to IV Points Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVPointsNode>) GetIVPointsPartition() const
  {
    return Handle(asiData_Partition<asiData_IVPointsNode>)::DownCast( this->Partition(Partition_IV_Points) );
  }

  //! Accessor for a Partition instance dedicated to IV Point Set 2D Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVPointSet2dNode>) GetIVPointSet2dPartition() const
  {
    return Handle(asiData_Partition<asiData_IVPointSet2dNode>)::DownCast( this->Partition(Partition_IV_PointSet2d) );
  }

  //! Accessor for a Partition instance dedicated to IV Point Set Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVPointSetNode>) GetIVPointSetPartition() const
  {
    return Handle(asiData_Partition<asiData_IVPointSetNode>)::DownCast( this->Partition(Partition_IV_PointSet) );
  }

  //! Accessor for a Partition instance dedicated to IV Curves Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVCurvesNode>) GetIVCurvesPartition() const
  {
    return Handle(asiData_Partition<asiData_IVCurvesNode>)::DownCast( this->Partition(Partition_IV_Curves) );
  }

  //! Accessor for a Partition instance dedicated to IV Curve Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVCurveNode>) GetIVCurvePartition() const
  {
    return Handle(asiData_Partition<asiData_IVCurveNode>)::DownCast( this->Partition(Partition_IV_Curve) );
  }

  //! Accessor for a Partition instance dedicated to IV 2D Curves Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVCurves2dNode>) GetIVCurves2dPartition() const
  {
    return Handle(asiData_Partition<asiData_IVCurves2dNode>)::DownCast( this->Partition(Partition_IV_Curves2d) );
  }

  //! Accessor for a Partition instance dedicated to IV 2D Curve Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVCurve2dNode>) GetIVCurve2dPartition() const
  {
    return Handle(asiData_Partition<asiData_IVCurve2dNode>)::DownCast( this->Partition(Partition_IV_Curve2d) );
  }

  //! Accessor for a Partition instance dedicated to IV Surfaces Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVSurfacesNode>) GetIVSurfacesPartition() const
  {
    return Handle(asiData_Partition<asiData_IVSurfacesNode>)::DownCast( this->Partition(Partition_IV_Surfaces) );
  }

  //! Accessor for a Partition instance dedicated to IV Surface Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVSurfaceNode>) GetIVSurfacePartition() const
  {
    return Handle(asiData_Partition<asiData_IVSurfaceNode>)::DownCast( this->Partition(Partition_IV_Surface) );
  }

  //! Accessor for a Partition instance dedicated to IV Topo Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTopoNode>) GetIVTopoPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTopoNode>)::DownCast( this->Partition(Partition_IV_Topo) );
  }

  //! Accessor for a Partition instance dedicated to IV Topo Item Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTopoItemNode>) GetIVTopoItemPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTopoItemNode>)::DownCast( this->Partition(Partition_IV_TopoItem) );
  }

  //! Accessor for a Partition instance dedicated to IV Tessellation Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTessNode>) GetIVTessPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTessNode>)::DownCast( this->Partition(Partition_IV_Tess) );
  }

  //! Accessor for a Partition instance dedicated to IV Tessellation Item Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTessItemNode>) GetIVTessItemPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTessItemNode>)::DownCast( this->Partition(Partition_IV_TessItem) );
  }

  //! Accessor for a Partition instance dedicated to IV Text Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTextNode>) GetIVTextPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTextNode>)::DownCast( this->Partition(Partition_IV_Text) );
  }

  //! Accessor for a Partition instance dedicated to IV Text Item Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTextItemNode>) GetIVTextItemPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTextItemNode>)::DownCast( this->Partition(Partition_IV_TextItem) );
  }

protected:

  asiEngine_EXPORT virtual void
    initPartitions();

  asiEngine_EXPORT virtual void
    initFunctionDrivers();

protected:

  asiEngine_EXPORT virtual Handle(ActAPI_IPartition)
    getVariablePartition(const VariableType& varType) const;

  asiEngine_EXPORT virtual Handle(ActAPI_INode)
    getRootNode() const;

  asiEngine_EXPORT virtual void
    invariantCopyRefs(ActAPI_FuncGUIDStream&         FuncGUIDs,
                      ActAPI_ParameterLocatorStream& Refs) const;

protected:

  asiEngine_EXPORT virtual int
    actualVersionApp();

  asiEngine_EXPORT virtual Handle(ActData_CAFConverter)
    converterApp();

protected:

  asiEngine_EXPORT virtual void
    clearCustom();

protected:

  //! IDs of the registered Partitions.
  enum PartitionId
  {
    Partition_RealVar = 1, // Good practice to start with 1 when working with CAF
  //---------------------------------------------------------------------------
    Partition_Root,
  //---------------------------------------------------------------------------
    Partition_Part,
    Partition_Face,
    Partition_FaceNorms,
    Partition_FaceContour,
    Partition_Surf,
    Partition_Edge,
    Partition_Curve,
    Partition_CurvatureCombs,
    Partition_BoundaryEdges,
    Partition_Contour,
    Partition_Vertex,
    Partition_TolerantShapes,
    Partition_TolerantRange,
  //---------------------------------------------------------------------------
    Partition_Triangulation,
    Partition_Tessellation,
    Partition_TessellationNorms,
  //---------------------------------------------------------------------------
    Partition_ReTopo,
    Partition_RePatch,
    Partition_RePatches,
    Partition_ReCoEdge,
    Partition_ReEdge,
    Partition_ReEdges,
    Partition_ReVertex,
    Partition_ReVertices,
  //---------------------------------------------------------------------------
    Partition_IV,
    Partition_IV_Points2d,
    Partition_IV_Points,
    Partition_IV_PointSet2d,
    Partition_IV_PointSet,
    Partition_IV_Curves,
    Partition_IV_Curve,
    Partition_IV_Curves2d,
    Partition_IV_Curve2d,
    Partition_IV_Surfaces,
    Partition_IV_Surface,
    Partition_IV_Topo,
    Partition_IV_TopoItem,
    Partition_IV_Tess,
    Partition_IV_TessItem,
    Partition_IV_Text,
    Partition_IV_TextItem,
  //---------------------------------------------------------------------------
    Partition_SurfDeviation,
  //---------------------------------------------------------------------------
    Partition_LAST
  };

};

#endif
