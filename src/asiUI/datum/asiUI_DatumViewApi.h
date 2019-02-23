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

#ifndef asiUI_DatumViewApi_HeaderFile
#define asiUI_DatumViewApi_HeaderFile

// asiUI includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QVariant>
#include <QString>
#pragma warning(pop)

class asiUI_DatumViewItem;
class QAbstractItemView;
class QModelIndex;

//! \ingroup GUI
//!
//! Public interface declaring basics application access to datum views.
class asiUI_EXPORT asiUI_DatumViewApi
{
public:

  //! indexes of datum view items
  struct ItemIndex
  {
  public:
    //! Construct index.
    //! \param theRow [in] the index row.
    //! \param theCol [in] the index col.
    ItemIndex(const int theRow, const int theCol)
    {
      Row = theRow;
      Col = theCol;
    }

  // properties
  public:

    int Row; //!< item index row.
    int Col; //!< item index col.
  };

  typedef QList<ItemIndex> IndexList;

public:

  //! Get value of the datum view item.
  //! If the datum is assigned, the returned value is the one converted to the SI 
  //! unit system. This value can differ with the displayed string.
  //! \param theRow [in] the data item's row.
  //! \param theColumn [in] the data item's column.
  //! \return SI unit system value or null value if invalid row or column specified.
  virtual QVariant 
    GetValue(const int theRow, 
             const int theColumn) const = 0;

  //! Set value for the datum view item.
  //! If the datum is assigned, the value will be considered as the one converted to the
  //! SI unit system. This value can differ with the displayed string.
  //! \param theRow [in] the data item's row.
  //! \param theColumn [in] the data item's column.
  //! \param theValue [in] the SI unit system value.
  virtual void
    SetValue(const int theRow, 
             const int theColumn,
             const QVariant& theValue) = 0;

  //! Get displayed string.
  //! The value corresponds to the editor's contents that corresponds to the value
  //! converted to local unit system.
  //! \param theRow [in] the data item's row.
  //! \param theColumn [in] the data item's column.
  //! \return displayed string or null string if invalid row or column is specified.
  virtual QString 
    GetString(const int theRow,
              const int theColumn) const = 0;

  //! Set displayed string.
  //! The value corresponds to the editor's contents which is a value converted
  //! to local unit system.
  //! \param theRow [in] the data item's row.
  //! \param theColumn [in] the data item's column.
  virtual void
    SetString(const int theRow,
              const int theColumn,
              const QString& theString) = 0;

  //! Return number of data rows in view's layout.
  //! \return number of data rows.
  virtual int
    GetRowCount() const = 0;

  //! Return number of data columns in view's layout.
  //! \return number of data columns.
  virtual int
    GetColumnCount() const = 0;

  //! Sets number of data rows in view's layout.
  //! \param theRows [in] the number of data rows.
  virtual void
    SetRowCount(const int theRows) = 0;

  //! Sets number of data columns in view's layout.
  //! \param theCols [in] the number of data columns.
  virtual void
    SetColumnCount(const int theCols) = 0;

  //! Insert new data rows at the specified row index.
  //! \param theRowAt [in] where to insert the rows.
  //! \param theRows [in] the number of rows to insert.
  virtual void
    InsertRows(const int theRowAt,
               const int theRows = 1) = 0;

  //! Insert new data columns at the specified column index.
  //! \param theColAt [in] where to insert the columns.
  //! \param theCols [in] the number of columns to insert.
  virtual void
    InsertColumns(const int theColAt,
                  const int theCols = 1) = 0;

  //! Remove data rows at the specified row index.
  //! \param theRowAt [in] where to remove the rows.
  //! \param theRows [in] the number of rows to remove.
  virtual void
    RemoveRows(const int theRowAt,
               const int theRows = 1) = 0;

  //! Remove data columns at the specified column index.
  //! \param theColAt [in] where to remove the columns.
  //! \param theCols [in] the number of columns to remove.
  virtual void
    RemoveColumns(const int theColAt,
                  const int theCols = 1) = 0;

  //! Access data item at the specified row and column index.
  //! \param theRow [in] the data row.
  //! \param theCol [in] the data col.
  virtual asiUI_DatumViewItem*
    AccessItem(const int theRow,
               const int theCol) const = 0;

  //! Set new item prototype for the view.
  //! \param thePrototype [in] the prototype to set up.
  virtual void
    SetItemPrototype(const asiUI_DatumViewItem* thePrototype) = 0;

  //! Get list of selected view item indexes.
  //! \return list of selected indexes.
  virtual IndexList
    GetSelectedIndexes() const = 0;

  //! Set an indexes from the list are selected.
  //! \param theList [in] the list of indexes.
  //! \return true if success, otherwise - false.
  virtual bool
    SelectIndexes(const IndexList &theList) = 0;

  //! Get index of data element at specified view's index.
  //! \param the item index.
  //! \return index of data element at specified view's index.
  virtual ItemIndex
    MapViewToItem(const QModelIndex& theIndex) const = 0;

  //! Get view index of data element at specified
  //! row and column index.
  //! \param theRow [in] the data row.
  //! \param theCol [in] the data col.
  //! \return view item model index.
  virtual QModelIndex
    MapItemToView(const int theRow, const int theCol) const = 0;

  //! Return data view widget pointer.
  //! \return data view widget pointer.
  operator QAbstractItemView*()
  {
    return this->Widget();
  }

  //! Access data view widget.
  //! \return data view widget pointer.
  virtual QAbstractItemView*
    Widget() = 0;
};

#endif
