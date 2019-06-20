//-----------------------------------------------------------------------------
// Created on: 22 August 2018
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

#ifndef asiAlgo_InterpolateSurfMesh_h
#define asiAlgo_InterpolateSurfMesh_h

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>
#include <asiAlgo_BVHFacets.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <Geom_BSplineSurface.hxx>
#include <NCollection_CellFilter.hxx>

//-----------------------------------------------------------------------------

//! Utility to build interpolation surface by mesh.
class asiAlgo_InterpolateSurfMesh : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_InterpolateSurfMesh, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT static bool
    CollectInteriorNodes(const Handle(Poly_Triangulation)&  tris,
                         const std::vector<gp_XYZ>&         contour,
                         const bool                         boxClipping,
                         Handle(asiAlgo_BaseCloud<double>)& pts,
                         ActAPI_ProgressEntry               progress,
                         ActAPI_PlotterEntry                plotter);

public:

  //! Structure representing the grid node.
  struct t_gridNode
  {
    int    id;              //!< Global 0-based ID.
    bool   isContourPoint;  //!< Whether this point is a pole on a contour.
    bool   isSampledPoint;  //!< Whether this point is a sampled point.
    bool   isMeshNodePoint; //!< Whether this point corresponds to a mesh node.
    gp_XY  uvInit;          //!< (u,v) coordinates on the initial surface.
    gp_XYZ xyzInit;         //!< (x,y,z) coordinates on the initial surface.
    gp_XY  uv;              //!< (u,v) coordinates on the final surface.
    gp_XYZ xyz;             //!< (x,y,z) coordinates on the final surface.

    //! Default ctor.
    t_gridNode()
    : id              (-1),
      isContourPoint  (false),
      isSampledPoint  (false),
      isMeshNodePoint (false)
    {}

    bool operator<(const t_gridNode& other)
    {
      return this->uvInit.X() < other.uvInit.X();
    }
  };

  //! Auxiliary class to search for coincident spatial points.
  class InspectXY : public NCollection_CellFilter_InspectorXY
  {
  public:

    typedef gp_XY Target;

    //! Constructor accepting resolution distance and point.
    InspectXY(const double resolution, const gp_XY& P) : m_fResolution(resolution), m_bFound(false), m_P(P) {}

    //! \return true/false depending on whether the node was found or not.
    bool IsFound() const { return m_bFound; }

    //! Implementation of inspection method.
    NCollection_CellFilter_Action Inspect(const gp_XY& Target)
    {
      m_bFound = ( (m_P - Target).SquareModulus() <= Square(m_fResolution) );
      return CellFilter_Keep;
    }

  private:

    gp_XY  m_P;           //!< Source point.
    bool   m_bFound;      //!< Whether two points are coincident or not.
    double m_fResolution; //!< Resolution to check for coincidence.

  };

  //! Auxiliary class to search for coincident tessellation nodes.
  class InspectNode : public InspectXY
  {
  public:

    typedef t_gridNode Target;

    //! Constructor accepting resolution distance and point.
    InspectNode(const double resolution, const t_gridNode& P) : InspectXY(resolution, P.uvInit), m_iID(-1) {}

    int GetID() const { return m_iID; }

    //! Implementation of inspection method.
    NCollection_CellFilter_Action Inspect(const t_gridNode& Target)
    {
      InspectXY::Inspect(Target.uvInit);

      if ( InspectXY::IsFound() )
        m_iID = Target.id;

      return CellFilter_Keep;
    }

  private:

    int m_iID; //!< Found target ID.

  };

public:

  //! Ctor.
  //! \param[in] tris     triangulation to interpolate.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_InterpolateSurfMesh(const Handle(Poly_Triangulation)& tris,
                                ActAPI_ProgressEntry              progress,
                                ActAPI_PlotterEntry               plotter);

public:

  asiAlgo_EXPORT bool
    Perform(const std::vector<gp_XYZ>&   contour,
            const double                 grainCoeff,
            const int                    degU,
            const int                    degV,
            Handle(Geom_BSplineSurface)& result);

public:

  //! Sets externally contructed BVH structure for acceleration of computations.
  //! \param[in] bvh BVH structure to set.
  void SetBVH(const Handle(asiAlgo_BVHFacets)& bvh)
  {
    m_bvh = bvh;
  }

protected:

  asiAlgo_EXPORT bool
    performInternal(const std::vector<gp_XYZ>&   contour,
                    const double                 grainCoeff,
                    const int                    degU,
                    const int                    degV,
                    Handle(Geom_BSplineSurface)& result);

  asiAlgo_EXPORT static bool
    collectInteriorNodes(const Handle(Poly_Triangulation)&    tris,
                         const std::vector<gp_XYZ>&           contour,
                         const bool                           boxClipping,
                         NCollection_CellFilter<InspectNode>& filter,
                         Handle(Geom_Plane)&                  avrPlane,
                         std::vector<t_gridNode>&             pts,
                         int&                                 lastPtIdx,
                         double&                              size,
                         ActAPI_ProgressEntry                 progress,
                         ActAPI_PlotterEntry                  plotter);

protected:

  Handle(Poly_Triangulation) m_tris; //!< Triangulation in question.
  Handle(asiAlgo_BVHFacets)  m_bvh;  //!< Accelerating structure for triangulation.

};

#endif
