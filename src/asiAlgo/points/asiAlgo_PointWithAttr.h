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

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
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
  //! \param[in] P point to initialize this one from.
  asiAlgo_PointWithAttr(const asiAlgo_PointWithAttr& P) : Coord(P.Coord), Status(P.Status), Index(P.Index), pData(P.pData) {}

  //! Complete ctor.
  //! \param[in] _coord    spatial coordinates.
  //! \param[in] _status   status (deleted, not deleted).
  //! \param[in] _index    index of the point in external collection.
  //! \param[in] _data_ptr user data.
  asiAlgo_PointWithAttr(const TCoord _coord    = TCoord(),
                        const int    _status   = 0,
                        const size_t _index    = 0,
                        void*        _data_ptr = NULL) : Coord(_coord), Status(_status), Index(_index), pData(_data_ptr) {}

  bool operator>  (const asiAlgo_PointWithAttr&) const { return false; }
  bool operator== (const asiAlgo_PointWithAttr&) const { return false; }

  //! Conversion operator to access coordinates only, without attributes.
  operator TCoord() const
  {
    return Coord;
  }

  TCoord Coord;  //!< Coordinates.
  int    Status; //!< General-purpose status.
  size_t Index;  //!< Index in the owning collection.
  void*  pData;  //!< Associated data.
};

//-----------------------------------------------------------------------------
// Point cloud
//-----------------------------------------------------------------------------

//! Point cloud where each element is enriched with metadata. This class is
//! to some extent similar to asiAlgo_BaseCloud, though, unlike the latter,
//! it allows storing not only point coordinates but also custom attributes.
//! This class is best suited for moderate amount of data because each point
//! here is stored explicitly (plus metadata it contains also significantly
//! contributes to the memory consumption).
//!
//! Note: interface similarity between this class and asiAlgo_BaseCloud is kept
//!       to allow for using common services like cloud purification, etc.
//!
//! \sa asiAlgo_BaseCloud
template <typename TCoord>
class asiAlgo_PointWithAttrCloud : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_PointWithAttrCloud<TCoord>, Standard_Transient)

public:

  //! Default ctor.
  asiAlgo_EXPORT
    asiAlgo_PointWithAttrCloud();

public:

  //! Cleans up the point cloud.
  asiAlgo_EXPORT virtual void
    Clear();

public:

  //! \return number of contained elements (points).
  asiAlgo_EXPORT virtual int
    GetNumberOfElements() const;

  //! Returns the point cloud element by its zero-based index.
  //! \param[in] zeroBasedIndex index of the point to access.
  //! \return const reference to the contained point cloud element.
  asiAlgo_EXPORT virtual const asiAlgo_PointWithAttr<TCoord>&
    GetElement(const int zeroBasedIndex) const;

  //! Returns the point cloud element by its zero-based index.
  //! \param[in] zeroBasedIndex index of the point to access.
  //! \return non-const reference to the contained point cloud element.
  asiAlgo_EXPORT virtual asiAlgo_PointWithAttr<TCoord>&
    ChangeElement(const int zeroBasedIndex);

  //! Adds another element to the point cloud.
  //! \param[in] coord element to add.
  asiAlgo_EXPORT virtual void
    AddElement(const asiAlgo_PointWithAttr<TCoord>& coord);

protected:

  //! Internal collection of points with attributes.
  std::vector< asiAlgo_PointWithAttr<TCoord> > m_pts;

};

#endif
