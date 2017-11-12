//-----------------------------------------------------------------------------
// Created on: 21 September 2016
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

#ifndef asiAlgo_BVHFacets_h
#define asiAlgo_BVHFacets_h

// Analysis Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopoDS_Face.hxx>
#include <BVH_Types.hxx>
#include <BVH_PrimitiveSet.hxx>
#include <NCollection_Vector.hxx>

// STL includes
#include <vector>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

//-----------------------------------------------------------------------------

//! BVH-based accelerating structure representing CAD model's
//! facets in computations.
class asiAlgo_BVHFacets : public BVH_PrimitiveSet<double, 4>
{
public:

  //! Structure representing a single facet.
  struct t_facet
  {
    t_facet() : FaceIndex(-1) {}
    t_facet(const int fidx) : FaceIndex(fidx) {}

    BVH_Vec4d P0, P1, P2; //!< Triangle nodes.
    int       FaceIndex;  //!< Index of the host face.
  };

  //! Type of BVH builder to use.
  enum BuilderType
  {
    Builder_Binned,
    Builder_Linear
  };

public:

  asiAlgo_EXPORT
    asiAlgo_BVHFacets(const TopoDS_Shape&  model,
                      const BuilderType    builderType,
                      ActAPI_ProgressEntry progress,
                      ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT virtual int
    Size() const override;

  asiAlgo_EXPORT virtual BVH_Box<double, 4>
    Box(const int index) const override;

  asiAlgo_EXPORT virtual double
    Center(const int index,
           const int axis) const override;

  asiAlgo_EXPORT virtual void
    Swap(const int index1,
         const int index2) override;

public:

  asiAlgo_EXPORT void
    GetVertices(const int  index,
                BVH_Vec4d& vertex1,
                BVH_Vec4d& vertex2,
                BVH_Vec4d& vertex3) const;

  asiAlgo_EXPORT double
    GetBoundingDiag() const;

public:

  asiAlgo_EXPORT void
    Dump(ActAPI_PlotterEntry IV);

public:

  //! Returns a facet by its 0-based index.
  //! \param[in] index index of the facet of interest.
  //! \return requested facet.
  const t_facet& GetFacet(const int index)
  {
    return m_facets[index];
  }

  //! \return master CAD model.
  const TopoDS_Shape& GetShape() const
  {
    return m_model;
  }

protected:

  asiAlgo_EXPORT bool
    init(const TopoDS_Shape& model,
         const BuilderType   builderType);

  asiAlgo_EXPORT bool
    addFace(const TopoDS_Face& face,
            const int          face_idx);

protected:

  //! Master shape.
  TopoDS_Shape m_model;

  //! Array of facets.
  std::vector<t_facet> m_facets;

  //! Characteristic size of the model.
  double m_fBoundingDiag;

  //! Progress Entry.
  ActAPI_ProgressEntry m_progress;

  //! Imperative Plotter.
  ActAPI_PlotterEntry m_plotter;

};

#endif
