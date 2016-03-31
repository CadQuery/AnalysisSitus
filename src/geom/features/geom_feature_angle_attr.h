//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_feature_angle_attr_h
#define geom_feature_angle_attr_h

// Geometry includes
#include <geom_angle.h>
#include <geom_feature_attribute.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_feature_angle_attr, geom_feature_attribute)

//! Attribute storing information about feature angle between faces.
class geom_feature_angle_attr : public geom_feature_attribute
{
public:

  DEFINE_STANDARD_RTTI_INLINE(geom_feature_angle_attr, geom_feature_attribute)

public:

  //! Creates feature angle attribute.
  inline geom_feature_angle_attr() : geom_feature_attribute()
  {
    m_angle = Angle_Undefined;
  }

  //! Creates feature angle attribute.
  //! \param angleProp [in] angle property.
  inline geom_feature_angle_attr(const geom_angle angleProp) : geom_feature_attribute()
  {
    m_angle = angleProp;
  }

public:

  inline geom_angle GetAngle()                   const { return m_angle; }
  inline void       SetAngle(const geom_angle a)       { m_angle = a; }

protected:

  geom_angle m_angle; //!< Angle property.

};

#endif
