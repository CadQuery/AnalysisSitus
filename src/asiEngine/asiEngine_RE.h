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

#ifndef asiEngine_RE_h
#define asiEngine_RE_h

// asiEngine includes
#include <asiEngine_Base.h>

// asiData includes
#include <asiData_ReCoEdgeNode.h>

//! API for the data model operations related to reverse engineering.
class asiEngine_RE : public asiEngine_Base
{
public:

  //! ctor.
  //! \param[in] model    Data Model instance.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiEngine_RE(const Handle(asiEngine_Model)& model,
               ActAPI_ProgressEntry           progress = NULL,
               ActAPI_PlotterEntry            plotter  = NULL)
  //
  : asiEngine_Base(model, progress, plotter) {}

public:

  //! \return newly created Topology Node.
  asiEngine_EXPORT Handle(asiData_ReTopoNode)
    Create_Topo();

  //! Creates RePatches Node under the given ReTopology Node.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_RePatchesNode)
    Create_Patches(const Handle(asiData_ReTopoNode)& topo_n);

  //! Creates ReEdges Node under the given ReTopology Node.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_ReEdgesNode)
    Create_Edges(const Handle(asiData_ReTopoNode)& topo_n);

  //! Creates ReVertices Node under the given ReTopology Node.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_ReVerticesNode)
    Create_Vertices(const Handle(asiData_ReTopoNode)& topo_n);

public:

  //! Returns the sole Patches Node available in the project.
  //! \return Patches Node.
  asiEngine_EXPORT Handle(asiData_RePatchesNode)
    Get_Patches();

  //! Returns the sole Edges Node available in the project.
  //! \return Edges Node.
  asiEngine_EXPORT Handle(asiData_ReEdgesNode)
    Get_Edges();

  //! Returns the sole Vertices Node available in the project.
  //! \return Vertices Node.
  asiEngine_EXPORT Handle(asiData_ReVerticesNode)
    Get_Vertices();

  //! Creates a Patch Node under the single Patches Node available in the
  //! project.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_RePatchNode)
    Create_Patch();

  //! Creates an Edge Node under the single Edges Node available in the
  //! project.
  //! \param[in] vfirst handle of the first vertex of the edge.
  //! \param[in] vlast  handle of the second vertex of the edge.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_ReEdgeNode)
    Create_Edge(const Handle(asiData_ReVertexNode)& vfirst = NULL,
                const Handle(asiData_ReVertexNode)& vlast  = NULL);

  //! Creates a CoEdge Node under the passed Patch Node.
  //! \param[in] patch     owning patch.
  //! \param[in] edge      edge to reference.
  //! \param[in] samesense orientation flag.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_ReCoEdgeNode)
    Create_CoEdge(const Handle(asiData_RePatchNode)& patch,
                  const Handle(asiData_ReEdgeNode)&  edge,
                  const bool                         samesense);

  //! Creates a Vertex Node under the single Vertices Node available in the
  //! project.
  //! \param[in] coords vertex coordinates.
  //! \param[in] norm   normal vector at vertex position.
  //! \return newly created Node.
  asiEngine_EXPORT Handle(asiData_ReVertexNode)
    Create_Vertex(const gp_XYZ& coords, const gp_XYZ& norm);

public:

  //! Collects all points constituting the discrete boundary of the Patch Node.
  //! \param[in]  patch Patch Node in question.
  //! \param[out] pts   collected points.
  asiEngine_EXPORT void
    CollectBoundaryPoints(const Handle(asiData_RePatchNode)& patch,
                          std::vector<gp_XYZ>&               pts) const;

  //! Collects the points corresponding to the vertices of a Patch Node.
  //! \param[in]  patch Patch Node in question.
  //! \param[out] pts   collected points.
  asiEngine_EXPORT void
    CollectVertexPoints(const Handle(asiData_RePatchNode)& patch,
                        std::vector<gp_XYZ>&               pts) const;

  //! Collects all triangles interacting with the boundary of the given patch.
  //! \param[in]  patch Patch Node in question.
  //! \param[out] tris  indices of triangles.
  asiEngine_EXPORT void
    CollectContourTriangles(const Handle(asiData_RePatchNode)& patch,
                            TColStd_PackedMapOfInteger&        tris) const;

  //! Computes midpoint for the outer contour of the given patch.
  //! \param[in] patch patch in question.
  //! \return midpoint.
  asiEngine_EXPORT gp_XYZ
    ComputeMidPoint(const Handle(asiData_RePatchNode)& patch) const;

  //! Extracts the piece of triangulation bounded by the contour of the given
  //! patch. This method implements the following algorithm:
  //!
  //! - Find triangles covered by the contour (the indices of these triangles
  //!   are known at contour construction time, so here we do not need to
  //!   do any PMC tests).
  //! - Compute the center point of the contour and project it to the mesh in
  //!   order to selected a triangle somewhere inside the region of interest.
  //! - Traverse the neighbor triangles starting from the picked center until
  //!   the boundary triangles are reached. All the traversed triangles are
  //!   added to the result.
  //!
  //! This method works in assumption that there is connectivity relationship
  //! between the triangles of the source mesh. Otherwise, the traversal
  //! procedure will immediately stop in the center triangle.
  //!
  //! \param[in]  patch  patch in question.
  //! \param[out] region extracted region of the input mesh bounded by the
  //!                    patch contour.
  //! \return true in case of success, false -- otherwise.
  asiEngine_EXPORT bool
    ExtractBoundedRegion(const Handle(asiData_RePatchNode)& patch,
                         Handle(Poly_Triangulation)&        region) const;

  //! Returns patches which share the passed edge. The patches are classified
  //! as left and right depending on whether their corresponding coedges are
  //! collinear with the shared edge or not.
  //!
  //! \param[in]  edge       edge in question.
  //! \param[out] patchLeft  patch on the left (collinear).
  //! \param[out] patchRight patch on the right.
  //! \return true in case of success, false -- otherwise.
  asiEngine_EXPORT bool
    GetPatchesByEdge(const Handle(asiData_ReEdgeNode)& edge,
                     Handle(asiData_RePatchNode)&      patchLeft,
                     Handle(asiData_RePatchNode)&      patchRight) const;

  //! Fills the passed patch with a Coons surface.
  //! \param[in]  patch patch in question.
  //! \param[out] surf  constructed surface.
  //! \return true in case of success, false -- otherwise.
  asiEngine_EXPORT bool
    FillPatchCoons(const Handle(asiData_RePatchNode)& patch,
                   Handle(Geom_BSplineSurface)&       surf) const;

};

#endif
