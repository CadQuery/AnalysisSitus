//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_PointCloud_h
#define asiAlgo_PointCloud_h

// A-Situs includes
#include <asiAlgo.h>

// Geometry includes
#include <asiAlgo_IntersectionPointCC.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// STL includes
#include <fstream>
#include <vector>

// OCCT includes
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>

DEFINE_STANDARD_HANDLE(asiAlgo_PointCloud, ActAPI_IAlgorithm)

//! Interoperability tool for point clouds.
class asiAlgo_PointCloud : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_PointCloud, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT static bool
    ReadSections(const TCollection_AsciiString& filename,
                 TopoDS_Shape&                  result);

  asiAlgo_EXPORT static bool
    ReadScattered(const TCollection_AsciiString& filename,
                  TopoDS_Shape&                  result);

  asiAlgo_EXPORT static void
    RemoveCoincidentPoints(const double                 prec,
                           asiAlgo_IntersectionPointsCC& pts);

  asiAlgo_EXPORT static Handle(asiAlgo_PointCloud)
    FromIntersection(const asiAlgo_IntersectionPointsCC& pts);

public:

  asiAlgo_EXPORT
    asiAlgo_PointCloud(ActAPI_ProgressEntry progress = NULL,
                       ActAPI_PlotterEntry  plotter  = NULL);

  asiAlgo_EXPORT
    asiAlgo_PointCloud(const Handle(TColStd_HArray1OfReal)& coords,
                       ActAPI_ProgressEntry                 progress = NULL,
                       ActAPI_PlotterEntry                  plotter  = NULL);

public:

  //! Sets point coordinates.
  //! \param pts [in] points to set.
  void SetPoints(const Handle(TColStd_HArray1OfReal)& pts)
  {
    m_coords = pts;
  }

  //! \return coordinate array.
  const Handle(TColStd_HArray1OfReal)& GetPoints() const
  {
    return m_coords;
  }

  //! Returns a point by 0-based index.
  //! \param[in] idx point index.
  //! \return point.
  gp_Pnt GetPoint(const int idx) const
  {
    const double x = m_coords->Value(idx*3);
    const double y = m_coords->Value(idx*3 + 1);
    const double z = m_coords->Value(idx*3 + 2);
    //
    return gp_Pnt(x, y, z);
  }

  //! \return number of points in the cloud.
  int GetNumOfPoints() const
  {
    if ( m_coords.IsNull() )
      return 0;

    return m_coords->Length() / 3;
  }

public:

  asiAlgo_EXPORT bool SaveAs (const TCollection_AsciiString& filename);
  asiAlgo_EXPORT bool Load   (const TCollection_AsciiString& filename);

protected:

  bool readPositions(std::ifstream&       FILE,
                     std::vector<gp_XYZ>& points);

protected:

  Handle(TColStd_HArray1OfReal) m_coords; //!< Array of coordinates.

};

#endif
