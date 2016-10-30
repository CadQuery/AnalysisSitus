//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_point_cloud_h
#define geom_point_cloud_h

// A-Situs includes
#include <asitusGeom.h>

// Geometry includes
#include <geom_intersection_point_cc.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// STL includes
#include <fstream>
#include <vector>

// OCCT includes
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>

DEFINE_STANDARD_HANDLE(geom_point_cloud, ActAPI_IAlgorithm)

//! Interoperability tool for point clouds.
class geom_point_cloud : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_point_cloud, ActAPI_IAlgorithm)

public:

  static bool
    ReadSections(const TCollection_AsciiString& filename,
                 TopoDS_Shape&                  result);

  static bool
    ReadScattered(const TCollection_AsciiString& filename,
                  TopoDS_Shape&                  result);

  static void
    RemoveCoincidentPoints(const double                 prec,
                           geom_intersection_points_cc& pts);

  static Handle(geom_point_cloud)
    FromIntersection(const geom_intersection_points_cc& pts);

public:

  geom_point_cloud(ActAPI_ProgressEntry progress = NULL,
                   ActAPI_PlotterEntry  plotter  = NULL);
  //
  geom_point_cloud(const Handle(TColStd_HArray1OfReal)& coords,
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

  bool SaveAs (const TCollection_AsciiString& filename);
  bool Load   (const TCollection_AsciiString& filename);

protected:

  bool readPositions(std::ifstream&       FILE,
                     std::vector<gp_XYZ>& points);

protected:

  Handle(TColStd_HArray1OfReal) m_coords; //!< Array of coordinates.

};

#endif
