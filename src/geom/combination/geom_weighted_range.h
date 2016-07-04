//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_weighted_range_h
#define geom_weighted_range_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>

//! Represents a simple closed weighted range like [x, y]. The weight might be
//! used by external algorithms, e.g. in order to assign a priority to the
//! given range or count the number of overlapping situations detected.
class geom_weighted_range
{
public:

  //! Enumerates all meaningful mutual relations between ranges.
  enum OverlappingType
  {
    Overlap_No,                   //!< E.g. this is [1, 2] and other is [2, 3].
    Overlap_SectionAndRightTail,  //!< E.g. this is [2, 4] and other is [1, 3].
    Overlap_ContainsAndRightTail, //!< E.g. this is [1, 4] and other is [1, 3].
    Overlap_ContainsAndBothTails, //!< E.g. this is [1, 4] and other is [2, 3].
    Overlap_ContainsAndLeftTail,  //!< E.g. this is [1, 3] and other is [2, 3].
    Overlap_SectionAndLeftTail,   //!< E.g. this is [1, 3] and other is [2, 4].
    Overlap_Coincident,           //!< E.g. this is [1, 2] and other is [1, 2].
    Overlap_ContainedInside,      //!< E.g. this is [2, 3] and other is [1, 4].
    Overlap_ContainedOnLeft,      //!< E.g. this is [2, 3] and other is [2, 4].
    Overlap_ContainedOnRight,     //!< E.g. this is [2, 3] and other is [1, 3].
    Overlap_NotClassified         //!< Overlap relation is not accessible.
  };

public:

  //! Left value of the range.
  double PMin;

  //! Right value of the range.
  double PMax;

  //! Weight value.
  double Weight;

public:

  //! Default constructor.
  geom_weighted_range() : PMin(0.0), PMax(0.0), Weight(0) {}

  //! Assignment constructor.
  //! \param Other [in] other range to copy into this one.
  geom_weighted_range(const geom_weighted_range& Other)
  {
    this->operator=(Other);
  }

  //! Constructor.
  //! \param _PMin   [in] first value of the range.
  //! \param _PMax   [in] second value of the range.
  //! \param _Weight [in] weight value to associate.
  geom_weighted_range(const double _PMin,
                      const double _PMax,
                      const double _Weight)
  : PMin(_PMin), PMax(_PMax), Weight(_Weight) {}

public:

  //! Classifies the relation type between this range and the passed range.
  //! \param theOther [in] other range.
  //! \return relation type.
  OverlappingType Classify(const geom_weighted_range& theOther) const;

  //! Checks whether this range is degenerated or not. Degenerated range
  //! has left and right values equal with OCCT confusion precision.
  //! \return true/false.
  inline bool IsDegenerated() const
  {
    return Abs(PMin - PMax) < Precision::Confusion();
  }

public:

  //! Assignment operator.
  //! \param Other [in] other range.
  //! \return re-assigned this.
  geom_weighted_range& operator=(const geom_weighted_range& Other)
  {
    PMin   = Other.PMin;
    PMax   = Other.PMax;
    Weight = Other.Weight;
    return *this;
  }

  //! Comparison operator. Checks mutual geometrical position for the
  //! couple of ranges.
  //! \param Other [in] other range.
  //! \return true if this is "greater", false -- otherwise.
  bool operator>(const geom_weighted_range& Other) const
  {
    return Abs(PMin - Other.PMax) < Precision::Confusion() ||
           PMin > Other.PMax;
  }

  //! Comparison operator. Checks mutual geometrical position for the
  //! couple of ranges. Also checks weights.
  //! \param Other [in] other range.
  //! \return true if the ranges are fully the same, false -- otherwise.
  bool operator==(const geom_weighted_range& theOther) const
  {
    const double PREC = Precision::Confusion();
    return Abs(PMin   - theOther.PMin)   < PREC &&
           Abs(PMax   - theOther.PMax)   < PREC &&
           Abs(Weight - theOther.Weight) < PREC;
  }
};

//! Type shortcut for an ordered direct-access collection of Weighted Ranges.
typedef NCollection_Sequence<geom_weighted_range> geom_weighted_ranges;

#endif
