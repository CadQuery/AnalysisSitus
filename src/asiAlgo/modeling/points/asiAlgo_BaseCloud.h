//-----------------------------------------------------------------------------
// Created on: 28 November 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_BaseCloud_h
#define asiAlgo_BaseCloud_h

// asiAlgo includes
#include <asiAlgo_CloudRegion.h>

// OCCT includes
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfReal.hxx>

// STL includes
#include <vector>

//-----------------------------------------------------------------------------

//! Simple cloud of elements (points or vectors).
template <typename TCoordType>
class asiAlgo_BaseCloud : public Standard_Transient
{
public:

  asiAlgo_EXPORT
    asiAlgo_BaseCloud();

  asiAlgo_EXPORT
    asiAlgo_BaseCloud(const std::vector<TCoordType>& coords);

public:

  asiAlgo_EXPORT void
    CopyTo(asiAlgo_BaseCloud<TCoordType>& copy) const;

  asiAlgo_EXPORT void
    Reserve(const int nElems);

  asiAlgo_EXPORT int
    GetNumberOfElements() const;

  asiAlgo_EXPORT void
    AddElement(const TCoordType x, const TCoordType y, const TCoordType z);

  asiAlgo_EXPORT void
    SetElement(const int elemIndex, const TCoordType x, const TCoordType y, const TCoordType z);

  asiAlgo_EXPORT void
    GetElement(const int elemIndex, TCoordType& x, TCoordType& y, TCoordType& z) const;

  asiAlgo_EXPORT const std::vector<TCoordType>&
    GetCoords() const;

  asiAlgo_EXPORT std::vector<TCoordType>&
    ChangeCoords();

  asiAlgo_EXPORT void
    ComputeBoundingBox(TCoordType& xMin, TCoordType& xMax,
                       TCoordType& yMin, TCoordType& yMax,
                       TCoordType& zMin, TCoordType& zMax) const;

  asiAlgo_EXPORT Handle(asiAlgo_BaseCloud<TCoordType>)
    ExtractRegion(const asiAlgo_CloudRegion& region) const;

public:

  asiAlgo_EXPORT bool
    Load(const char* filename);

  asiAlgo_EXPORT bool
    SaveAs(const char* filename) const;

protected:

  std::vector<TCoordType> m_coords; //!< Coordinates of cloud elements.

};

#endif
