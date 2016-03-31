//-----------------------------------------------------------------------------
// Created on: 10 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_feature_pattern_chole_h
#define geom_feature_pattern_chole_h

// Geometry includes
#include <geom_feature_pattern.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_feature_pattern_chole, geom_feature_pattern)

//! Feature pattern for a cylindrical hole.
class geom_feature_pattern_chole : public geom_feature_pattern
{
public:

  DEFINE_STANDARD_RTTI_INLINE(geom_feature_pattern_chole, geom_feature_pattern)

public:

  //! Creates feature pattern initializing it with the given AAG iterator.
  //! \param aag_it   [in] AAG iterator.
  //! \param target_R [in] radius of interest (upper barrier).
  inline geom_feature_pattern_chole(const Handle(geom_aag_iterator)& aag_it,
                                    const double                     target_R)
  : geom_feature_pattern (aag_it),
    m_fRadius            (0.0),
    m_fTargetRadius      (target_R)
  {}

private:

  virtual bool
    recognize(TopTools_IndexedMapOfShape& featureFaces);

public:

  //! \return radius of the detected hole.
  inline double GetRadius() const { return m_fRadius; }

  //! \return target radius.
  inline double GetTargetRadius() const { return m_fTargetRadius; }

protected:

  double m_fRadius;       //!< Radius of the detected hole.
  double m_fTargetRadius; //!< Target radius.

};

#endif
