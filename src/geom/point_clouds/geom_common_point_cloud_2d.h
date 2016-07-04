//-----------------------------------------------------------------------------
// Created on: 15 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_common_point_cloud_2d_h
#define geom_common_point_cloud_2d_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <gp_XY.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColgp_HSequenceOfPnt2d.hxx>

// STL includes
#include <vector>

//-----------------------------------------------------------------------------
// Single point with additional information
//-----------------------------------------------------------------------------

//! Point enriched with additional information used in algorithms.
template <typename TCoord>
struct geom_common_point
{
  //! Copy ctor.
  //! \param P [in] point to initialize this one from.
  geom_common_point(const geom_common_point& P) : Coord(P.Coord), Status(P.Status), Index(P.Index), pData(P.pData) {}

  //! Complete ctor.
  //! \param _coord    [in] spatial coordinates.
  //! \param _status   [in] status (deleted, not deleted).
  //! \param _index    [in] index of the point in external collection.
  //! \param _data_ptr [in] user data.
  geom_common_point(const TCoord _coord    = TCoord(),
                    const int    _status   = 0,
                    const size_t _index    = 0,
                    void*        _data_ptr = NULL) : Coord(_coord), Status(_status), Index(_index), pData(_data_ptr) {}

  int operator>  (const geom_common_point&) const { return false; }
  int operator== (const geom_common_point&) const { return false; }

  TCoord Coord;  //!< Coordinates.
  int    Status; //!< General-purpose status.
  size_t Index;  //!< Index in the owning collection.
  void*  pData;  //!< Associated data.
};

//! Collection of 2D points with statuses.
typedef std::vector< geom_common_point<gp_XY>, std::allocator< geom_common_point<gp_XY> > > geom_common_point_2d_vec;

//-----------------------------------------------------------------------------
// Point cloud
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_common_point_cloud_2d, Standard_Transient)

//! Data adapter for two-dimensional point cloud.
class geom_common_point_cloud_2d : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_common_point_cloud_2d, Standard_Transient)

public:

  static Handle(geom_common_point_cloud_2d)
    RemoveCoincidentPoints(const Handle(geom_common_point_cloud_2d)& pts,
                           const double                              prec);

public:

  virtual Handle(geom_common_point_cloud_2d)
    EmptyCopy() const = 0;

  virtual const gp_XY&
    Value(const int oneBasedIndex) const = 0;

  virtual const int
    Length() const = 0;

  virtual Handle(TColStd_HArray1OfReal)
    Coords2d() const = 0;

  virtual void
    SetCoords2d(const Handle(TColStd_HArray1OfReal)& coords) = 0;

  virtual void
    AddCoord(const gp_XY& coord) = 0;

public:

  //! Reads point cloud from file.
  //! \param filename [in]  filename of the target file.
  //! \param points   [out] retrieved points.
  //! \return true in case of success, false -- otherwise.
  static bool
    Read(const TCollection_AsciiString&   filename,
         Handle(TColgp_HSequenceOfPnt2d)& points);

  //! Writes point cloud to file.
  //! \param points   [in] points to write.
  //! \param filename [in] filename of the target file.
  //! \return true in case of success, false -- otherwise.
  static bool
    Write(const Handle(TColgp_HSequenceOfPnt2d)& points,
          const TCollection_AsciiString&         filename);

};

#endif
