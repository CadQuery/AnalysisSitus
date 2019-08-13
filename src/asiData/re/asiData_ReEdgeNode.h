//-----------------------------------------------------------------------------
// Created on: 06 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiData_ReEdgeNode_h
#define asiData_ReEdgeNode_h

// asiData includes
#include <asiData_ReVertexNode.h>

// Active Data includes
#include <ActData_BaseNode.h>

// OCCT includes
#include <Geom_Curve.hxx>

//! Data Node representing topological edge used for interactive segmentation.
class asiData_ReEdgeNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_ReEdgeNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_ReEdgeNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //-------------------------//
    PID_Name,                //!< Name of the Node.
    PID_VertexFirstRef,      //!< Reference to the first vertex of the edge.
    PID_VertexLastRef,       //!< Reference to the last vertex of the edge.
    PID_VertexFirstIdx,      //!< Index of the pole in the polyline which corresponds to the first vertex.
    PID_VertexLastIdx,       //!< Index of the pole in the polyline which corresponds to the last vertex.
    PID_Polyline,            //!< Polyline representation of the edge.
    PID_PolylineInds,        //!< Indices of the triangles corresponding to polyline poles.
    PID_Curve,               //!< Curve representation of the edge.
  //-------------------------//
    PID_GroupGeometry,       //!< Group for geometric properties.
  //-------------------------//
    PID_ApproxToler,         //!< Approximation tolerance.
    PID_SmoothTransition,    //!< Whether the edge realizes smooth transition.
    PID_FuncApprox,          //!< Approximation function.
    PID_FuncSmoothenCorners, //!< Tree function to adapt curve network for smooth transition.
    PID_FuncSmoothenPatches, //!< Tree function to adapt adjacent surfaces for smooth transition.
  //-------------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  //! Returns new DETACHED instance of the Node ensuring its correct
  //! allocation in a heap.
  //! \return new instance of the Node.
  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  //! Performs initial actions required to make Node WELL-FORMED.
  //! \param[in] vfirst handle of the first vertex of the edge.
  //! \param[in] vlast  handle of the second vertex of the edge.
  asiData_EXPORT void
    Init(const Handle(asiData_ReVertexNode)& vfirst = NULL,
         const Handle(asiData_ReVertexNode)& vlast  = NULL);

// Generic naming support:
public:

  //! Accessor for the Node's name.
  //! \return name of the Node.
  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  //! Sets name for the Node.
  //! \param[in] name name to set.
  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString& name);

  //! Sets array storing the coordinates of polyline's poles.
  //! \param[in] coords array to store.
  asiData_EXPORT void
    SetPolyline(const Handle(HRealArray)& coords);

  //! Sets array storing the indices of triangles the polyline is snapped to.
  //! \param[in] indices array to store.
  asiData_EXPORT void
    SetPolylineTriangles(const Handle(HIntArray)& indices);

  //! \return number of poles in the polyline representation of the edge.
  asiData_EXPORT int
    GetNumPolylinePoles() const;

  //! \return array storing the coordinates of polyline's poles.
  asiData_EXPORT Handle(HRealArray)
    GetPolyline() const;

  //! Returns poles of the polyline.
  //! \param[out] pts poles defined explicitly.
  asiData_EXPORT void
    GetPolyline(std::vector<gp_XYZ>& pts) const;

  //! \return array storing the indices of polyline's triangles.
  asiData_EXPORT Handle(HIntArray)
    GetPolylineTriangles() const;

  //! Returns a point by its zero-based index in the persistent array.
  //! \param[in] zeroBasedIndex 0-based index of the point to access.
  //! \return coordinates of the point in question.
  asiData_EXPORT gp_XYZ
    GetPolylinePole(const int zeroBasedIndex) const;

  //! Adds another point to the polyline.
  //! \param[in] point  point to add.
  //! \param[in] triIdx 1-based index of the corresponding triangle.
  //! \return 0-based index of the just added point.
  asiData_EXPORT int
    AddPolylinePole(const gp_XYZ& point, const int triIdx = -1);

  //! Returns the stored geometry.
  //! \return stored geometry.
  asiData_EXPORT Handle(Geom_Curve)
    GetCurve() const;

  //! Returns the stored geometry.
  //! \param[out] f first parameter.
  //! \param[out] l last parameter.
  //! \return stored geometry.
  asiData_EXPORT Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

  //! Returns the stored curve in form of CAD edge. This method is used for
  //! compliance with visualization pipelines.
  //! \return BREP shape (TopoDS_Edge) representing the curve.
  asiData_EXPORT TopoDS_Shape
    GetCurveAsShape() const;

  //! Sets curve to store.
  //! \param[in] curve geometry to store.
  asiData_EXPORT void
    SetCurve(const Handle(Geom_Curve)& curve);

  //! Sets curve to store.
  //! \param[in] curve geometry to store.
  //! \param[in] f     first parameter of the curve.
  //! \param[in] l     last parameter of the curve.
  asiData_EXPORT void
    SetCurve(const Handle(Geom_Curve)& curve,
             const double              f,
             const double              l);

  //! \return first vertex of the edge.
  asiData_EXPORT Handle(asiData_ReVertexNode)
    GetFirstVertex() const;

  //! \return last vertex of the edge.
  asiData_EXPORT Handle(asiData_ReVertexNode)
    GetLastVertex() const;

  //! \return index of the first vertex of the edge.
  asiData_EXPORT int
    GetFirstVertexIndex() const;

  //! \return last vertex of the edge.
  asiData_EXPORT int
    GetLastVertexIndex() const;

  //! Sets the reference to the first vertex of the edge.
  //! \param[in] vertex vertex reference to set.
  asiData_EXPORT void
    SetFirstVertex(const Handle(asiData_ReVertexNode)& vertex);

  //! Sets the reference to the last vertex of the edge.
  //! \param[in] vertex vertex reference to set.
  asiData_EXPORT void
    SetLastVertex(const Handle(asiData_ReVertexNode)& vertex);

  //! Sets the 0-based index of the first vertex of the edge.
  //! \param[in] idx 0-based index to set.
  asiData_EXPORT void
    SetFirstVertexIndex(const int idx);

  //! Sets the 0-based index of the last vertex of the edge.
  //! \param[in] idx 0-based index to set.
  asiData_EXPORT void
    SetLastVertexIndex(const int idx);

  //! Sets approximation tolerance for the edge's curve.
  //! \param[in] toler value to set.
  asiData_EXPORT void
    SetApproxToler(const double toler);

  //! \return approximation tolerance for the edge's curve.
  asiData_EXPORT double
    GetApproxToler() const;

  //! Sets smooth transition mode on/off.
  //! \param[in] on Boolean value to set.
  asiData_EXPORT void
    SetSmoothTransition(const bool on);

  //! \return true if smooth transition is enabled, false -- otherwise.
  asiData_EXPORT bool
    IsSmoothTransition() const;

protected:

  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_ReEdgeNode();

};

#endif
