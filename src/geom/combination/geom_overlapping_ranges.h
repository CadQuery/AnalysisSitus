//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_overlapping_ranges_h
#define geom_overlapping_ranges_h

// Geometry includes
#include <geom_weighted_range.h>

//! Collects simple closed ranges taking into account their overlapping
//! properties, so that splitting of the interfering ranges is done when
//! needed. As the result, you obtain the series of non-intersecting ranges
//! where each range stores the number of its overlapping events. The
//! procedure is performed in a single global range which is adjusted as long
//! as you add new ranges into the tool.
//!
//! This class ensures that all your ranges are internally sorted
//! geometrically. You also have a possibility to eliminate gaps between
//! your ranges by adding the "ghost" intermediate ranges with weight
//! number set to 0 (weight reflects the number of overlapping
//! events).
class geom_overlapping_ranges
{
public:

  //! Constructor accepting (optionally) the working range. Despite on
  //! whether you specify the global range or not, it might be implicitly
  //! adjusted by the tool in case if it is not big enough for storing
  //! newly added ranges.
  //! \param globalMin [in] left value of the global range.
  //! \param globalMax [in] right value of the global range.
  geom_overlapping_ranges(const double globalMin =  RealLast(),
                          const double globalMax = -RealLast())
  {
    m_fGlobalMin = globalMin;
    m_fGlobalMax = globalMax;
  }

public:

  ASitus_EXPORT void
    Add(geom_weighted_range& New);

  ASitus_EXPORT void
    FillGaps();

  ASitus_EXPORT const geom_weighted_ranges&
    List() const;

  ASitus_EXPORT geom_weighted_ranges
    SortedByOverlaps(const double R1,
                     const double R2) const;

  ASitus_EXPORT geom_weighted_ranges
    SortedByOverlaps() const;

protected:

  ASitus_EXPORT void
    sortByOverlaps(geom_weighted_ranges& List) const;

  ASitus_EXPORT bool
    exhaustCoincidence(geom_weighted_range&       Ex,
                       const geom_weighted_range& New,
                       geom_weighted_range&       RestNewLeft,
                       geom_weighted_range&       RestNewRight,
                       bool&                      doCompleteRight);

protected:

  geom_weighted_ranges m_ranges;     //!< Collection of ranges.
  double               m_fGlobalMin; //!< Left value of global working range.
  double               m_fGlobalMax; //!< Right value of global working range.

};

#endif
