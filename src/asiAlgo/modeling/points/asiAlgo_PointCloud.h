//-----------------------------------------------------------------------------
// Created on: 28 November 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_PointCloud_h
#define asiAlgo_PointCloud_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfReal.hxx>

// STL includes
#include <vector>

//-----------------------------------------------------------------------------

//! Simple point cloud.
template <typename TCoordType>
class asiAlgo_PointCloud : public Standard_Transient
{
public:

  asiAlgo_EXPORT
    asiAlgo_PointCloud();

  asiAlgo_EXPORT
    asiAlgo_PointCloud(const std::vector<TCoordType>& coords);

public:

  asiAlgo_EXPORT void
    Reserve(const int nPts);

  asiAlgo_EXPORT int
    GetNumberOfPoints() const;

  asiAlgo_EXPORT void
    AddPoint(const TCoordType x, const TCoordType y, const TCoordType z);

  asiAlgo_EXPORT void
    SetPoint(const int pointIndex, const TCoordType x, const TCoordType y, const TCoordType z);

  asiAlgo_EXPORT void
    GetPoint(const int pointIndex, TCoordType& x, TCoordType& y, TCoordType& z) const;

  asiAlgo_EXPORT const std::vector<TCoordType>&
    GetCoords() const;

  asiAlgo_EXPORT std::vector<TCoordType>&
    ChangeCoords();

public:

  asiAlgo_EXPORT bool
    Load(const char* filename);

  asiAlgo_EXPORT bool
    SaveAs(const char* filename) const;

protected:

  std::vector<TCoordType> m_coords; //!< Coordinates of a point cloud.

};

#endif
