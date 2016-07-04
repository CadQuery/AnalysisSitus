//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_overlapping_ranges.h>

// OCCT includes
#include <NCollection_QuickSort.hxx>
#include <Standard_ProgramError.hxx>

#define COUT_DEBUG
#undef DRAW_DEBUG

//! Adds a new range to the internal collection of ranges adjusting it so that
//! not to have interfering ones. Overlapping counts (weights) are updated
//! correspondingly.
//! \param New [in] new range to add.
void geom_overlapping_ranges::Add(geom_weighted_range& New)
{
  // Adjust global min/max if needed
  if ( New.PMin < m_fGlobalMin )
    m_fGlobalMin = New.PMin;
  if ( New.PMax > m_fGlobalMax )
    m_fGlobalMax = New.PMax;

  const int nRanges = m_ranges.Length();
  if ( !nRanges )
  {
    m_ranges.Append(New);
    return;
  }

  // Iterate over the ordered collection of ranges adding the
  // non-overlapping segments one-by-one
  geom_weighted_ranges NewRanges;
  geom_weighted_range NewLocal = New;
  bool isCoincident = false;
  for ( int i = 1; i <= nRanges; ++i )
  {
    geom_weighted_range& Ex = m_ranges.ChangeValue(i);
    geom_weighted_range RestNewLeft, RestNewRight;
    bool doCompleteRight = false;

    if ( !this->exhaustCoincidence(Ex, NewLocal, RestNewLeft, RestNewRight, doCompleteRight) )
    {
      NewRanges.Append(Ex); // Transfer non-affected as-is
      continue;
    }
    if ( !RestNewLeft.IsDegenerated() )
      NewRanges.Append(RestNewLeft); // Add left tail

    NewRanges.Append(Ex);
    if ( doCompleteRight || (i == nRanges) )
      NewRanges.Append(RestNewRight); // Add right tail
    else
      NewLocal = RestNewRight;

    if ( !isCoincident )
      isCoincident = true;
  }

  // New range is totally outside the existing collection
  if ( !isCoincident )
    NewRanges.Append(NewLocal);

  m_ranges = NewRanges;
}

//! Adds fictive (with overlap values equal to 0) ranges filling the
//! geometrical gaps between stored ranges.
void geom_overlapping_ranges::FillGaps()
{
  // Enrich collection with gap segments -- those having no overlaps
  geom_weighted_ranges NewRanges;
  const int nRanges = m_ranges.Length();
  geom_weighted_range PrevRange;
  for ( int i = 1; i <= nRanges; ++i )
  {
    const geom_weighted_range& Ex    = m_ranges.Value(i);
    const double            exMin = Ex.PMin;
    const double            exMax = Ex.PMax;

    // Add leading gap segment
    if ( i == 1 )
    {
      if ( Abs(exMin - m_fGlobalMin) > Precision::Confusion() && exMin > m_fGlobalMin )
      {
        geom_weighted_range LeadingGap(m_fGlobalMin, exMin, 0); // Zero overlaps here
        NewRanges.Append(LeadingGap);
      }
    }

    // Add intermediate gap
    if ( !PrevRange.IsDegenerated() )
    {
      const double prevMax = PrevRange.PMax;

      if ( Abs(prevMax - exMin) > Precision::Confusion() )
      {
        geom_weighted_range Gap(prevMax, exMin, 0); // Zero overlaps here
        NewRanges.Append(Gap);
      }
    }

    // Add actual segment
    NewRanges.Append(Ex);
    PrevRange = Ex;

    // Add trailing gap segment
    if ( i == nRanges )
    {
      if ( Abs(exMax - m_fGlobalMax) > Precision::Confusion() && exMax < m_fGlobalMax )
      {
        geom_weighted_range TrailingGap(exMax, m_fGlobalMax, 0); // Zero overlaps here
        NewRanges.Append(TrailingGap);
      }
    }
  }

  m_ranges = NewRanges;
}

//! Returns the internal collection of ranges for consultation. Note that no
//! re-ordering is performed here because the correct order is maintained
//! automatically by Add() method.
//! \return collection of ranges.
const geom_weighted_ranges& geom_overlapping_ranges::List() const
{
  return m_ranges;
}

//! Returns an extraction from the entire collection of ranges lying within
//! the passed closed range and sorted by overlap numbers.
//! \param R1 [in] left boundary of the capturing interval.
//! \param R2 [in] right boundary of the capturing interval.
//! \return collection of ranges.
geom_weighted_ranges
  geom_overlapping_ranges::SortedByOverlaps(const double R1,
                                            const double R2) const
{
  geom_weighted_ranges ResultList;
  geom_weighted_range GlobalRange(R1, R2, 0.0);

  for ( int i = 1; i <= m_ranges.Length(); ++i )
  {
    const geom_weighted_range& Range = m_ranges.Value(i);
    const geom_weighted_range::OverlappingType type = Range.Classify(GlobalRange);

    if ( type != geom_weighted_range::Overlap_Coincident &&
         type != geom_weighted_range::Overlap_ContainedInside &&
         type != geom_weighted_range::Overlap_ContainedOnLeft &&
         type != geom_weighted_range::Overlap_ContainedOnRight )
      continue;

    ResultList.Append(Range);
  }
  this->sortByOverlaps(ResultList);

  return ResultList;
}

//! Returns a new collection of ranges sorted by their associated overlap
//! counts. The ordering is ascendant, so the ranges with the minimal
//! weights go first.
//! \return collection of ranges sorted by their weights (overlap numbers).
geom_weighted_ranges geom_overlapping_ranges::SortedByOverlaps() const
{
  geom_weighted_ranges Result = m_ranges;
  this->sortByOverlaps(Result);
  return Result;
}

//! Sorts the passed list of ranges by their number of overlaps. The sorting
//! is ascendant, so the ranges with the minimal weights go first.
//! \param List [in/out] list of ranges to sort.
void geom_overlapping_ranges::sortByOverlaps(geom_weighted_ranges& List) const
{
  NCollection_Comparator<geom_weighted_range> Comp;
  NCollection_QuickSort<geom_weighted_ranges, geom_weighted_range>
    ::Perform( List, Comp, 1, List.Length() );
}

//! Internal method performing the actual resolution of interfering ranges with
//! updating of the correspondent weights.
//! \param _Ex              [in/out] existing range (will be possibly modified).
//! \param _New             [in]     range being added.
//! \param _RestNewLeft     [out]    left sub-segment (of old or new range) which is
//!                                  free from interference.
//! \param _RestNewRight    [out]    right sub-segment (of old or new range) which
//!                                  is free from interference.
//! \param _doCompleteRight [out]    notifier for the caller method. Indicates
//!                                  that right sub-segment must be immediately
//!                                  added to the resulting sequence rather than
//!                                  pushed for subsequent processing. Left
//!                                  segments are pushed to the resulting sequence
//!                                  unconditionally.
//! \return true in case of success, false -- otherwise.
bool
  geom_overlapping_ranges::exhaustCoincidence(geom_weighted_range&       _Ex,
                                              const geom_weighted_range& _New,
                                              geom_weighted_range&       _RestNewLeft,
                                              geom_weighted_range&       _RestNewRight,
                                              bool&                      _doCompleteRight)
{
  geom_weighted_range aRestNewLeft, aRestNewRight;
  const geom_weighted_range::OverlappingType type = _Ex.Classify(_New);

  // Just for sure
  if ( type == geom_weighted_range::Overlap_NotClassified )
    Standard_ProgramError::Raise("Cannot classify new range");

  bool isCoincident;
  if ( type != geom_weighted_range::Overlap_No )
  {
    isCoincident = true;
    if ( type != geom_weighted_range::Overlap_Coincident )
    {
      if ( type == geom_weighted_range::Overlap_ContainsAndRightTail )
      {
        aRestNewRight.PMin = _New.PMax;
        aRestNewRight.PMax = _Ex.PMax;
        _doCompleteRight   = true;

        // Reduce existing to new
        _Ex.PMax = _New.PMax;
      }
      else if ( type == geom_weighted_range::Overlap_ContainsAndBothTails )
      {
        aRestNewLeft.PMin  = _Ex.PMin;
        aRestNewLeft.PMax  = _New.PMin;
        aRestNewRight.PMin = _New.PMax;
        aRestNewRight.PMax = _Ex.PMax;
        _doCompleteRight   = true;

        // Reduce existing to new
        _Ex.PMin = _New.PMin;
        _Ex.PMax = _New.PMax;
      }
      else if ( type == geom_weighted_range::Overlap_ContainsAndLeftTail )
      {
        aRestNewLeft.PMin = _Ex.PMin;
        aRestNewLeft.PMax = _New.PMin;

        // Reduce existing to new
        _Ex.PMin = _New.PMin;
      }
      else if ( type == geom_weighted_range::Overlap_SectionAndRightTail )
      {
        aRestNewLeft.PMin  = _New.PMin;
        aRestNewLeft.PMax  = _Ex.PMin;
        aRestNewRight.PMin = _New.PMax;
        aRestNewRight.PMax = _Ex.PMax;
        _doCompleteRight   = true;

        // Reduce existing to section
        _Ex.PMax = _New.PMax;
      }
      else if ( type == geom_weighted_range::Overlap_SectionAndLeftTail )
      {
        aRestNewLeft.PMin  = _Ex.PMin;
        aRestNewLeft.PMax  = _New.PMin;
        aRestNewRight.PMin = _Ex.PMax;
        aRestNewRight.PMax = _New.PMax;

        // Reduce existing to section
        _Ex.PMin = _New.PMin;
      }
      else if ( type == geom_weighted_range::Overlap_ContainedOnLeft )
      {
        aRestNewRight.PMin = _Ex.PMax;
        aRestNewRight.PMax = _New.PMax;
      }
      else if ( type == geom_weighted_range::Overlap_ContainedOnRight )
      {
        aRestNewLeft.PMin = _New.PMin;
        aRestNewLeft.PMax = _Ex.PMin;
      }
      else if ( type == geom_weighted_range::Overlap_ContainedInside )
      {
        aRestNewLeft.PMin  = _New.PMin;
        aRestNewLeft.PMax  = _Ex.PMin;
        aRestNewRight.PMin = _Ex.PMax;
        aRestNewRight.PMax = _New.PMax;
      }
    }
  }
  else
  {
    isCoincident = false;
  }

  if ( isCoincident )
  {
    if ( !aRestNewLeft.IsDegenerated() )
      aRestNewLeft.Weight = _Ex.Weight;

    if ( !aRestNewRight.IsDegenerated() )
    {
      if ( _doCompleteRight )
        aRestNewRight.Weight = _Ex.Weight; // Will be completed
      else
        aRestNewRight.Weight = 1; // Will go further
    }

    _Ex.Weight++;
  }

  _RestNewLeft  = aRestNewLeft;
  _RestNewRight = aRestNewRight;

  return isCoincident;
}
