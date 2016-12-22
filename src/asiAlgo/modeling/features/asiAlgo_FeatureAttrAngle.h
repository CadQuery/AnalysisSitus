//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_FeatureAttrAngle_h
#define asiAlgo_FeatureAttrAngle_h

// Feature includes
#include <asiAlgo_FeatureAttrAdjacency.h>
#include <asiAlgo_FeatureAngle.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_FeatureAttrAngle, asiAlgo_FeatureAttrAdjacency)

//! Attribute storing information about feature angle between faces.
class asiAlgo_FeatureAttrAngle : public asiAlgo_FeatureAttrAdjacency
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FeatureAttrAngle, asiAlgo_FeatureAttrAdjacency)

public:

  //! Creates feature angle attribute.
  asiAlgo_FeatureAttrAngle()
  : asiAlgo_FeatureAttrAdjacency(), m_angle(Angle_Undefined)
  {}

  //! Creates feature angle attribute.
  //! \param[in] angleProp angle property.
  asiAlgo_FeatureAttrAngle(const asiAlgo_FeatureAngle angleProp)
  : asiAlgo_FeatureAttrAdjacency(), m_angle(angleProp)
  {}

  //! Creates feature angle attribute with common edges.
  //! \param[in] a           angle property.
  //! \param[in] commonEdges common edges.
  asiAlgo_FeatureAttrAngle(const asiAlgo_FeatureAngle               a,
                           const TopTools_IndexedMapOfShape& commonEdges)
  : asiAlgo_FeatureAttrAdjacency(commonEdges), m_angle(a)
  {}

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("ADCC7A78-E062-4CE2-B394-7F25DDFD555E");
    return guid;
  }

  //! \return GUID associated with this type of attribute.
  virtual const Standard_GUID& GetGUID() const
  {
    return GUID();
  }

public:

  //! \return angle between faces.
  asiAlgo_FeatureAngle GetAngle() const { return m_angle; }

  //! Sets angle between faces.
  //! \param[in] a the angle value.
  void SetAngle(const asiAlgo_FeatureAngle a) { m_angle = a; }

protected:

  asiAlgo_FeatureAngle m_angle; //!< Angle property.

};

#endif
