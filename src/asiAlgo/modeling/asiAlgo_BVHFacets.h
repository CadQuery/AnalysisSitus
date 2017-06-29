//-----------------------------------------------------------------------------
// Created on: 21 September 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
