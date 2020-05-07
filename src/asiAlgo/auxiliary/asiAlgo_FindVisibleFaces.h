//-----------------------------------------------------------------------------
// Created on: 28 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

#ifndef asiAlgo_FindVisibleFaces_h
#define asiAlgo_FindVisibleFaces_h

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OpenCascade includes
#include <gp_Lin.hxx>

//-----------------------------------------------------------------------------

//! Finds all visible faces of a CAD part.
class asiAlgo_FindVisibleFaces : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FindVisibleFaces, ActAPI_IAlgorithm)

public:

  //! Data structure to store a bundle of rays associated with a specific
  //! CAD face.
  struct t_rayBundle
  {
    std::vector<gp_Lin> Rays;      //!< Ray bundle.
    int                 FaceIndex; //!< Reference to the corresponding CAD face.

    //! Default ctor leaving the face ID uninitialized.
    t_rayBundle() : FaceIndex(0) {}

    //! Ctor with initialization.
    //! \param[in] _ray    single ray to pass.
    //! \param[in] _faceId reference to the CAD face.
    t_rayBundle(const gp_Lin& _ray, const int _faceId)
    {
      Rays.push_back(_ray);
      FaceIndex = _faceId;
    }
  };

  //! This structure holds the results of intersection test
  //! for a single face.
  struct t_score
  {
    int NumHits;  //!< How many times for the given face we found intersections.
    int NumVoids; //!< How many times the intersection is void.

    //! Default ctor.
    t_score() : NumHits(0), NumVoids(0) {}

    //! Complete ctor.
    //! \param[in] _numHits  the number of intersections.
    //! \param[in] _numVoids the number of void tests.
    t_score(const int _numHits, const int _numVoids)
    : NumHits(_numHits), NumVoids(_numVoids) {}
  };

public:

  //! Ctor.
  //! \param[in] shape    B-rep shape of a CAD part to analyze.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_FindVisibleFaces(const TopoDS_Shape&  shape,
                             ActAPI_ProgressEntry progress = nullptr,
                             ActAPI_PlotterEntry  plotter  = nullptr);

public:

  //! Performs ray casting to determine whether a face is visible
  //! or not. The algorithm performs the ray test for each triangle
  //! of the BVH structure. As each triangle is associated with
  //! its originating face ID, these IDs are accumulated in the
  //! result map.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform();

  //! \return the accumulated collection of visible faces.
  asiAlgo_EXPORT const NCollection_DataMap<int, t_score>&
    GetResultScores() const;

  //! Gathers all visible faces. The second argument is used to
  //! control how many intersections we allow for a single face.
  //! If zero is passed (that is the default value), the faces should
  //! have no hits in their score.
  //! \param[out] faces   the collected visible faces.
  //! \param[in]  maxHits the max allowed number of hits.
  asiAlgo_EXPORT void
    GetResultFaces(TColStd_PackedMapOfInteger& faces,
                   const int                   maxHits = 0) const;

protected:

  //! Initializes the algorithm with all internal data structures
  //! required for the intersection tests.
  //! \param[in] shape the CAD model to build BVH for.
  asiAlgo_EXPORT void
    init(const TopoDS_Shape& shape);

  //! Tests whether the argument ray bundle is visible or not.
  //! \param[in] rb the ray bundle to test.
  asiAlgo_EXPORT bool
    isVisible(const t_rayBundle& rb) const;

  asiAlgo_EXPORT bool
    testLeaf(const gp_Lin&    ray,
             const double     length,
             const BVH_Vec4i& leaf,
             const int        face2Skip,
             int&             resultFacet,
             double&          resultRayParamNormalized,
             gp_XYZ&          hitPoint) const;

  //! Conducts basic intersection test of the given line with respect to the
  //! bounding box defined by its corner points.
  //! \param[in] L      line to test.
  //! \param[in] boxMin lower corner of the box to test.
  //! \param[in] boxMax upper corner of the box to test.
  //! \return true/false.
  asiAlgo_EXPORT bool
    isOut(const gp_Lin&    ray,
          const BVH_Vec3d& boxMin,
          const BVH_Vec3d& boxMax,
          const double     prec) const;

  asiAlgo_EXPORT bool
    isIntersected(const gp_XYZ& rayStart,
                  const gp_XYZ& rayFinish,
                  const gp_XYZ& pntTri1,
                  const gp_XYZ& pntTri2,
                  const gp_XYZ& pntTri3,
                  double&       hitParamNormalized,
                  gp_XYZ&       hitPoint) const;

protected:

  Handle(asiAlgo_BVHFacets)         m_bvh;        //!< BVH for facets.
  std::vector<t_rayBundle>          m_rayBundles; //!< Rays to test.
  NCollection_DataMap<int, t_score> m_scores;     //!< Intersection "score" for each face.

};

#endif
