//-----------------------------------------------------------------------------
// Created on: 15 April 2016
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

#ifndef asiAlgo_PointWithAttr_h
#define asiAlgo_PointWithAttr_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <gp_XY.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColgp_HSequenceOfPnt2d.hxx>
#include <TColStd_HArray1OfReal.hxx>

// STL includes
#include <vector>

//-----------------------------------------------------------------------------
// Single point with additional information
//-----------------------------------------------------------------------------

//! Point enriched with additional information used in algorithms.
template <typename TCoord>
struct asiAlgo_PointWithAttr
{
  //! Copy ctor.
  //! \param P [in] point to initialize this one from.
  asiAlgo_PointWithAttr(const asiAlgo_PointWithAttr& P) : Coord(P.Coord), Status(P.Status), Index(P.Index), pData(P.pData) {}

  //! Complete ctor.
  //! \param _coord    [in] spatial coordinates.
  //! \param _status   [in] status (deleted, not deleted).
  //! \param _index    [in] index of the point in external collection.
  //! \param _data_ptr [in] user data.
  asiAlgo_PointWithAttr(const TCoord _coord    = TCoord(),
                        const int    _status   = 0,
                        const size_t _index    = 0,
                        void*        _data_ptr = NULL) : Coord(_coord), Status(_status), Index(_index), pData(_data_ptr) {}

  bool operator>  (const asiAlgo_PointWithAttr&) const { return false; }
  bool operator== (const asiAlgo_PointWithAttr&) const { return false; }

  TCoord Coord;  //!< Coordinates.
  int    Status; //!< General-purpose status.
  size_t Index;  //!< Index in the owning collection.
  void*  pData;  //!< Associated data.
};

//! Collection of 2D points with statuses.
typedef std::vector< asiAlgo_PointWithAttr<gp_XY>,
                     std::allocator< asiAlgo_PointWithAttr<gp_XY> > > asiAlgo_PointWithAttr2dVec;

//-----------------------------------------------------------------------------
// Point cloud
//-----------------------------------------------------------------------------

//! Data adapter for two-dimensional point cloud.
class asiAlgo_PointWithAttr2dCloud : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_PointWithAttr2dCloud, Standard_Transient)

public:

  asiAlgo_EXPORT static Handle(asiAlgo_PointWithAttr2dCloud)
    RemoveCoincidentPoints(const Handle(asiAlgo_PointWithAttr2dCloud)& pts,
                           const double                                prec);

public:

  asiAlgo_EXPORT virtual Handle(asiAlgo_PointWithAttr2dCloud)
    EmptyCopy() const = 0;

  asiAlgo_EXPORT virtual const gp_XY&
    Value(const int oneBasedIndex) const = 0;

  asiAlgo_EXPORT virtual const int
    Length() const = 0;

  asiAlgo_EXPORT virtual Handle(TColStd_HArray1OfReal)
    Coords2d() const = 0;

  asiAlgo_EXPORT virtual void
    SetCoords2d(const Handle(TColStd_HArray1OfReal)& coords) = 0;

  asiAlgo_EXPORT virtual void
    AddCoord(const gp_XY& coord) = 0;

public:

  //! Reads point cloud from file.
  //! \param[in] filename filename of the target file.
  //! \param[out] points  retrieved points.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT static bool
    Read(const TCollection_AsciiString&   filename,
         Handle(TColgp_HSequenceOfPnt2d)& points);

  //! Writes point cloud to file.
  //! \param[in] points   points to write.
  //! \param[in] filename filename of the target file.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT static bool
    Write(const Handle(TColgp_HSequenceOfPnt2d)& points,
          const TCollection_AsciiString&         filename);

};

#endif
