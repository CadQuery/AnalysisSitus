//-----------------------------------------------------------------------------
// Created on: 21 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_feature_pattern_slot_h
#define geom_feature_pattern_slot_h

// Geometry includes
#include <geom_feature_pattern.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_feature_pattern_slot, geom_feature_pattern)

//! Feature pattern for slot.
class geom_feature_pattern_slot : public geom_feature_pattern
{
public:

  DEFINE_STANDARD_RTTI_INLINE(geom_feature_pattern_slot, geom_feature_pattern)

public:

  //! Creates feature pattern initializing it with the given AAG iterator.
  //! \param aag_it [in] AAG iterator.
  inline geom_feature_pattern_slot(const Handle(geom_aag_iterator)& aag_it)
  : geom_feature_pattern (aag_it)
  {}

private:

  virtual bool
    recognize(TopTools_IndexedMapOfShape& featureFaces);

};

#endif
