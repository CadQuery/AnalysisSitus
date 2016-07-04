//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_weighted_range.h>

//! Classifies the relation type between this range and the passed range.
//! \param Other [in] other range.
//! \return relation type.
geom_weighted_range::OverlappingType
  geom_weighted_range::Classify(const geom_weighted_range& Other) const
{
  const double PREC = Precision::Confusion();

  if ( Abs(PMin - Other.PMax) < PREC || PMin > Other.PMax )
    return Overlap_No;

  if ( Abs(PMax - Other.PMin) < PREC || PMax < Other.PMin )
    return Overlap_No;

  if ( Abs(PMin - Other.PMin) < PREC && Abs(PMax - Other.PMax) < PREC )
    return Overlap_Coincident;

  if ( Abs(PMin - Other.PMin) < PREC && PMax > Other.PMax )
    return Overlap_ContainsAndRightTail;

  if ( PMin < Other.PMin && Abs(PMax - Other.PMax) < PREC )
    return Overlap_ContainsAndLeftTail;

  if ( PMin > Other.PMin && PMin < Other.PMax && PMax > Other.PMax )
    return Overlap_SectionAndRightTail;

  if ( PMin < Other.PMin && PMax > Other.PMax )
    return Overlap_ContainsAndBothTails;

  if ( Other.PMin > PMin && Other.PMin < PMax && Other.PMax > PMax )
    return Overlap_SectionAndLeftTail;

  if ( Abs(PMin - Other.PMin) < PREC && PMax < Other.PMax )
    return Overlap_ContainedOnLeft;

  if ( Abs(PMax - Other.PMax) < PREC && PMin > Other.PMin )
    return Overlap_ContainedOnRight;

  if ( PMin > Other.PMin && PMax < Other.PMax )
    return Overlap_ContainedInside;

  return Overlap_NotClassified;
}
