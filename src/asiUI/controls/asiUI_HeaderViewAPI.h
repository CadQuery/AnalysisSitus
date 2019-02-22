//-----------------------------------------------------------------------------
// Created on: 22 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Anton Poletaev, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiUI_HeaderViewAPI_HeaderFile
#define asiUI_HeaderViewAPI_HeaderFile

// asiUI includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QItemSelectionModel>
#include <QProxyStyle>
#include <QHash>
#include <QList>
#include <QSet>
#pragma warning(pop)

//! Entity to address header items of all kinds: usual sections, section groups, header band items.
//! The section group addressing is used for organizing header hierarchy.
//! \sa asiUI_HeaderViewDataAPI.
class asiUI_EXPORT asiUI_HeaderIndex
{
public:

  //! Construct invalid index.
  //! For hierarchy groups it can be considered as root index.
  asiUI_HeaderIndex()
  : m_sectionIndex(-1),
    m_sectionCount(-1),
    m_bandLine(-1)
  {}

  //! Default destructor.
  ~asiUI_HeaderIndex()
  {}

public:

  //! Constructs index to address unitary header section.
  //! It corresponds to usual column or row in the table.
  //! \param theSection [in] the section.
  static asiUI_HeaderIndex UnitarySection(const int theSection)
  {
    asiUI_HeaderIndex aIndex;
    aIndex.m_sectionIndex =  theSection;
    aIndex.m_sectionCount =  1;
    aIndex.m_bandLine     = -1;
    return aIndex;
  }

  //! Constructs index to address header group section.
  //! Group sections can form header hierarchy tree.
  //! \param theStarts [in] the first section under the group.
  //! \param theEnds [in] the last section under the group.
  static asiUI_HeaderIndex GroupSection(const int theStarts, const int theEnds)
  {
    asiUI_HeaderIndex aIndex;
    aIndex.m_sectionIndex = theStarts;
    aIndex.m_sectionCount = theEnds - theStarts + 1;
    aIndex.m_bandLine     = -1;
    return aIndex;
  }

  //! Constructs index to address header band items.
  //! A cell on the intersection of the passed header section index
  //! and band line index is addressed.
  //! \param theSection [in] address the section of the header.
  //! \param theBandLine [in] address the line in band area.
  static asiUI_HeaderIndex Band(const int theSection, const int theBandLine)
  {
    asiUI_HeaderIndex aIndex;
    aIndex.m_sectionIndex = theSection;
    aIndex.m_sectionCount = 1;
    aIndex.m_bandLine     = theBandLine;
    return aIndex;
  }

public:

  //! \return TRUE if the header index represents header root.
  inline bool IsValid() const
  { 
    return m_sectionIndex >= 0 && m_sectionCount > 0;
  }

  //! \return TRUE if the header index represents unitary or group section.
  inline bool IsSection() const
  {
    return IsValid() && m_bandLine == -1;
  }

  //! \return TRUE if the header index represents unitary section.
  inline bool IsUnitarySection() const
  {
    return IsValid() && m_bandLine == -1 && m_sectionCount == 1;
  }

  //! \return TRUE if the header index represents group section.
  inline bool IsGroupSection() const
  {
    return IsValid() && m_bandLine == -1 && m_sectionCount > 1;
  }

  //! \return TRUE if the header index represents band header item.
  inline bool IsBand() const
  {
    return IsValid() && m_bandLine >= 0;
  }

public:

  //! \return section index. This is the index of first section in group.
  inline int GetSection() const
  {
    return m_sectionIndex;
  }

  //! \return first section index in the group.
  inline int GetFirstSection() const
  {
    return m_sectionIndex;
  }

  //! \return last section index in the group.
  inline int GetLastSection() const
  {
    return m_sectionIndex + m_sectionCount - 1;
  }

  //! \return number of sections in group.
  inline int GetSectionCount() const
  {
    return m_sectionCount;
  }

  //! return index of band line.
  inline int GetBandLine() const
  {
    return m_bandLine;
  }

public:

  //! compare with other header index.
  //! \param theOther [in] the other index.
  bool operator < (const asiUI_HeaderIndex &theOther) const
  {
    if ( !IsValid() && theOther.IsValid() )
    {
      return true;
    }

    if ( IsBand() && theOther.IsBand() )
    {
      return m_sectionIndex != theOther.m_sectionIndex
        ? m_sectionIndex < theOther.m_sectionIndex
        : m_bandLine     < theOther.m_bandLine;
    }

    if ( IsBand() && !theOther.IsBand() )
    {
      return true;
    }

    if ( IsSection() && theOther.IsSection() )
    {
      return m_sectionCount != theOther.m_sectionCount
        ? m_sectionCount < theOther.m_sectionCount
        : m_sectionIndex < theOther.m_sectionIndex;
    }

    return false;
  }

  //! check for equality with other header index.
  //! \param theOther [in] the other index.
  bool operator == (const asiUI_HeaderIndex& theOther) const
  {
    if ( IsBand() && theOther.IsBand() )
    {
      return m_sectionIndex == theOther.m_sectionIndex
          && m_bandLine     == theOther.m_bandLine;
    }

    if ( IsSection() && theOther.IsSection() )
    {
      return m_sectionIndex == theOther.m_sectionIndex
          && m_sectionCount == theOther.m_sectionCount;
    }

    return !IsValid() && !theOther.IsValid();
  }

private:

  int m_sectionIndex;
  int m_sectionCount;
  int m_bandLine;
};

inline uint qHash(const asiUI_HeaderIndex &theIndex)
{
  QString aHash = QString::number( theIndex.GetBandLine() ) + ":"
                + QString::number( theIndex.GetSection() )  + ":"
                + QString::number( theIndex.GetSectionCount() );

  return qHash(aHash);
}

typedef QList<asiUI_HeaderIndex> asiUI_HeaderIndexList;
typedef QSet<asiUI_HeaderIndex> asiUI_HeaderIndexSet;

//! \ingroup GUI
//!
//! Interface extension to arrange data both for hierarchical / band (multiple row)
//! headers. Using this interface only makes sense if the interface (or its part)
//! is supported by the header view.
//! It is not mandatory to extend current item model used in application,
//! instead it can be wrapped by any identity proxy model, which should support
//! the interface as independent layer.
//!
//! Interface for specifying section hierarchy:
//! The hierarchy of header sections is provided via concept of "section groups".
//! Briefly, the section group is a range of subsections (from/to) represented
//! by a spanned cell. The custom data roles can be bound to the range for the
//! data visualization purposes. The root of header groups is invisible and is
//! addressed as undefined range (e.g. root model index). Starting from that
//! root any other sub-range can be requested via interface methods: SectionGroup.
//! Please note that the overlaying section ranges are normally should be prohibited.
//! The approach of "movable" sections could not be applied properly with this feature.
//! <pre>
//!
//!  Example of section range definition:
//!    o----------------------------------------------------------------------------------------------------------o
//!    |                               root range undefined                                                       |
//!    o-------------------------------------------o-----------------------------------------------o--------------o
//!    |              range 1-4                    |                range 5-7                      |              |
//!    o-------------o---------------o-------------o-----------o-----------o-----------o-----------o              |
//!    |  section 1  |  section 2    |  section 3  | section 4 | section 5 | section 6 | section 7 |  section N   |
//!    o-------------o---------------o-------------o-----------o-----------o-----------o-----------o--------------o
//!
//!         then go band headers...
//!
//!    o-------------o---------------o-------------o-----------o-----------o-----------o-----------o--------------o
//!    | sec 1,idx 1 |  sec 2,idx 1  |    ....     |           |           |           |           |              |
//!    o-------------o---------------o-------------o-----------o-----------o-----------o-----------o--------------o
//!    | sec 1,idx 2 |  sec 2,idx 2  |    ....     |           |           |           |           |              |
//!    o-------------o---------------o-------------o-----------o-----------o-----------o-----------o--------------o
//!    | sec 1,idx 3 |  sec 2,idx 3  |    ....     |           |           |           |           |              |
//!    o-------------o---------------o-------------o-----------o-----------o-----------o-----------o--------------o
//!
//! </pre>
//!
//! Interface for specifying band (multiple row) representation:
//! The multi-row header takes its data from SectionRowData method for rows of sections.
//! The number of rows is the value passed from SectionRowCount method.
//!
class asiUI_HeaderViewDataApi
{
public:

  virtual int
    BandRowsCount(const Qt::Orientation theHeader) const = 0;

  virtual int
    SectionCount(const Qt::Orientation theHeader,
                 const asiUI_HeaderIndex& theParent) const = 0;

  virtual asiUI_HeaderIndex
    Section(const Qt::Orientation theHeader,
            const asiUI_HeaderIndex& theParent,
            const int theChildIdx) const  = 0;

  virtual asiUI_HeaderIndex
    ParentSection(const Qt::Orientation theHeader,
                  const asiUI_HeaderIndex& theIndex) const = 0;

  virtual void
    SetHeaderItemData(const Qt::Orientation theHeader,
                      const asiUI_HeaderIndex& theIndex,
                      const QVariant& theValue,
                      const int theRole) = 0;

  virtual QVariant
    HeaderItemData(const Qt::Orientation theHeader,
                   const asiUI_HeaderIndex& theIndex,
                   const int theRole) const = 0;
};

//! \ingroup GUI
//!
//! The interface to implement communication of view's item selection model with advanced header
//! view. Provides information on the current selection of header band items. Please note that
//! that this info can be used more broadly, e.g. for copy & paste.
class asiUI_HeaderViewSelectionApi
{
public:

  virtual const asiUI_HeaderIndexList&
    HeaderSelection(const Qt::Orientation theHeader,
                    const bool theIntersecting = false) const = 0;

public:

  virtual void 
    select(const Qt::Orientation theHeader,
           const asiUI_HeaderIndex& theIndex,
           QItemSelectionModel::SelectionFlags theCommand) = 0;

  virtual void
    select(const Qt::Orientation theHeader,
           const QList<asiUI_HeaderIndex>& theIndexes,
           QItemSelectionModel::SelectionFlags theCommand) = 0;
};

#endif
