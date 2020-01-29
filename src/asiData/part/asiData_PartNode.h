//-----------------------------------------------------------------------------
// Created on: 28 November 2015
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

#ifndef asiData_PartNode_h
#define asiData_PartNode_h

// asiData includes
#include <asiData_AAGParameter.h>
#include <asiData_BoundaryEdgesNode.h>
#include <asiData_BVHParameter.h>
#include <asiData_ContourNode.h>
#include <asiData_CurveNode.h>
#include <asiData_EdgeNode.h>
#include <asiData_FaceContourNode.h>
#include <asiData_FaceNode.h>
#include <asiData_FaceNormsNode.h>
#include <asiData_MetadataNode.h>
#include <asiData_NamingParameter.h>
#include <asiData_OctreeNode.h>
#include <asiData_SurfNode.h>
#include <asiData_TolerantShapesNode.h>
#include <asiData_VertexNode.h>

// Active Data includes
#include <ActData_ShapeParameter.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

//! Node representing b-rep part.
class asiData_PartNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_PartNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_PartNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //----------------------//
  // Common               //
  //----------------------//
    PID_Name,             //!< Name of the Node.
  //----------------------//
  // Geometry             //
  //----------------------//
    PID_Geometry,         //!< B-Rep structure.
    PID_AutoAAG,          //!< Whether to build AAG automatically.
    PID_AAG,              //!< AAG structure.
    PID_BVH,              //!< BVH structure.
    PID_Naming,           //!< Topology naming service.
  //----------------------//
  // Transformation       //
  //----------------------//
    PID_GroupTrsf,        //!< Transformation group.
    PID_TrsfTx,           //!< OX displacement.
    PID_TrsfTy,           //!< OY displacement.
    PID_TrsfTz,           //!< OZ displacement.
    PID_TrsfRx,           //!< Rotation around OX.
    PID_TrsfRy,           //!< Rotation around OY.
    PID_TrsfRz,           //!< Rotation around OZ.
    PID_TrsfMx,           //!< Matrix of transformation.
  //----------------------//
  // Facets               //
  //----------------------//
    PID_GroupTess,        //!< Tessellation group.
    PID_TessLinDefl,      //!< Linear deflection of visualization tessellation.
    PID_TessAngDefl,      //!< Angular deflection of visualization tessellation.
    PID_KeepTessParams,   //!< Indicates whether to keep tessellation parameters frozen.
  //----------------------//
  // Presentation         //
  //----------------------//
    PID_GroupPrs,         //!< Presentation group.
    PID_DisplayMode,      //!< Display mode.
    PID_HasColor,         //!< Indicates whether the Color Parameter is active.
    PID_Color,            //!< Color.
    PID_HasVertices,      //!< Indicates whether vertices are active.
  //----------------------//
  // References           //
  //----------------------//
    PID_MetadataElems,    //!< Metadata elements.
  //----------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Generic naming support:
public:

  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString&);

// Handy accessors to the stored data:
public:

  asiData_EXPORT TopoDS_Shape
    GetShape(const bool applyTransform = false) const;

  asiData_EXPORT void
    SetAutoAAG(const bool flag) const;

  asiData_EXPORT bool
    IsAutoAAG() const;

  asiData_EXPORT Handle(asiAlgo_AAG)
    GetAAG() const;

  asiData_EXPORT Handle(asiAlgo_BVHFacets)
    GetBVH() const;

  asiData_EXPORT Handle(asiAlgo_Naming)
    GetNaming() const;

  asiData_EXPORT bool
    HasNaming() const;

  asiData_EXPORT void
    SetTransformation(const double tx,
                      const double ty,
                      const double tz,
                      const double rxDeg,
                      const double ryDeg,
                      const double rzDeg);

  asiData_EXPORT void
    GetTransformation(double& tx,
                      double& ty,
                      double& tz,
                      double& rxDeg,
                      double& ryDeg,
                      double& rzDeg) const;

  asiData_EXPORT void
    SetTransformationMx(const double a11,
                        const double a12,
                        const double a13,
                        const double a14,
                        const double a21,
                        const double a22,
                        const double a23,
                        const double a24,
                        const double a31,
                        const double a32,
                        const double a33,
                        const double a34);

  asiData_EXPORT void
    GetTransformationMx(double& a11,
                        double& a12,
                        double& a13,
                        double& a14,
                        double& a21,
                        double& a22,
                        double& a23,
                        double& a24,
                        double& a31,
                        double& a32,
                        double& a33,
                        double& a34) const;

  asiData_EXPORT gp_Trsf
    GetTransformationMx() const;

  asiData_EXPORT void
    UpdateTransformationMx();

  asiData_EXPORT void
    SetLinearDeflection(const double);

  asiData_EXPORT double
    GetLinearDeflection() const;

  asiData_EXPORT void
    SetAngularDeflection(const double);

  asiData_EXPORT double
    GetAngularDeflection() const;

  asiData_EXPORT void
    SetKeepTessParams(const bool flag) const;

  asiData_EXPORT bool
    IsKeepTessParams() const;

  asiData_EXPORT void
    SetHasColor(const bool);

  asiData_EXPORT bool
    HasColor() const;

  asiData_EXPORT void
    SetColor(const int);

  asiData_EXPORT int
    GetColor() const;

  asiData_EXPORT void
    SetDisplayMode(const int);

  asiData_EXPORT int
    GetDisplayMode() const;

  asiData_EXPORT void
    SetHasVertices(const bool);

  asiData_EXPORT bool
    HasVertices() const;

  //-------------------------------------------------------------------------//

  asiData_EXPORT Handle(asiData_FaceNode)
    GetFaceRepresentation() const;

  asiData_EXPORT Handle(asiData_FaceNormsNode)
    GetNormsRepresentation() const;

  asiData_EXPORT Handle(asiData_SurfNode)
    GetSurfaceRepresentation() const;

  asiData_EXPORT Handle(asiData_FaceContourNode)
    GetContourRepresentation() const;

  asiData_EXPORT Handle(asiData_EdgeNode)
    GetEdgeRepresentation() const;

  asiData_EXPORT Handle(asiData_CurveNode)
    GetCurveRepresentation() const;

  asiData_EXPORT Handle(asiData_BoundaryEdgesNode)
    GetBoundaryEdgesRepresentation() const;

  asiData_EXPORT Handle(asiData_ContourNode)
    GetContour() const;

  asiData_EXPORT Handle(asiData_VertexNode)
    GetVertexRepresentation() const;

  asiData_EXPORT Handle(asiData_TolerantShapesNode)
    GetTolerantShapes() const;

  asiData_EXPORT Handle(asiData_MetadataNode)
    GetMetadata() const;

  asiData_EXPORT Handle(asiData_OctreeNode)
    GetOctree() const;

// Initialization:
public:

  asiData_EXPORT void
    Init(const bool resetNaming);

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_PartNode();

protected:

  //! We prohibit to set shape from external code as such update should
  //! normally include update of AAG, and, optionally, BVH, etc.
  //! Therefore, there should be "smart update" logic defined at the upper
  //! level of software architecture, while simply substituting the B-rep
  //! shape is not enough.
  asiData_EXPORT void
    setShape(const TopoDS_Shape&);

  //! See comment for setShape() method.
  asiData_EXPORT void
    setAAG(const Handle(asiAlgo_AAG)&);

  //! See comment for setShape() method.
  asiData_EXPORT void
    setBVH(const Handle(asiAlgo_BVHFacets)&);

  //! See comment for setShape() method.
  asiData_EXPORT void
    setNaming(const Handle(asiAlgo_Naming)&);

};

#endif
