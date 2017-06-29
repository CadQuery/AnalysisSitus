//-----------------------------------------------------------------------------
// Created on: 10 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiUI_PrimDataView_h
#define asiUI_PrimDataView_h

// A-Situs includes
#include <asiUI.h>

// A-Situs (GUI) includes
#include <asiUI_PrimDataAdaptor.h>

// A-Situs (engine) includes
#include <asiEngine_Model.h>

// Qt includes
#pragma warning(push, 0)
#include <QWidget>
#pragma warning(pop)

//! Connector between GUI controls and their associated Data Model Parameters.
//! The concept of Data-View is introduced mainly for unification of data
//! transferring process from widgets to Data Model and vice versa.
//! Data-View is designed to be embedded into custom application widgets
//! taking responsibility for their data population and saving functionality.
//!
//! NOTICE: This Data-View is designed for usage in scope of a single Node for
//!         primitive (scalar) Parameter types only.
class asiUI_EXPORT asiUI_PrimDataView : public QObject
{
  Q_OBJECT

//-----------------------------------------------------------------------------
public:

  //! View part of the Data-View pattern.
  struct t_view
  {
    QWidget* pWidget; //!< Widget used to handle the data.

    //! Default constructor.
    t_view() : pWidget(NULL) {}

    //! \return true if the View has not been initialized.
    bool IsNull() const { return pWidget == NULL; }

    //! Assignment operator.
    //! \param OtherView [in] another View to copy into this.
    //! \return modified this.
    t_view& operator=(const t_view& OtherView)
    {
      this->pWidget = OtherView.pWidget;
      return *this;
    }

    //! Enable/disables the View.
    //! \param isEnabled [in] true/false.
    void SetEnabled(const bool isEnabled)
    {
      if ( pWidget )
        pWidget->setEnabled(isEnabled);
    }
  };

//-----------------------------------------------------------------------------

  //! Type short-cut for Data-View associations.
  typedef NCollection_DataMap<Handle(asiUI_PrimDataAdaptor),
                              t_view,
                              asiUI_DataAdaptor::Hasher> t_data_view_map;

//-----------------------------------------------------------------------------
public:

  asiUI_PrimDataView(const Handle(asiEngine_Model)& model,
                     QObject*                       parent = NULL)
  //
  : QObject(parent), m_model(model), m_streamChunk(Chunk_Data) {}

//-----------------------------------------------------------------------------
// API methods:
public:

  virtual bool
    ViewToData(QWidget* pView = NULL);

  virtual bool
    DataToView();

//-----------------------------------------------------------------------------
// Initialization:
public:

  asiUI_PrimDataView& operator<<(const Handle(asiUI_PrimDataAdaptor)& Data);
  asiUI_PrimDataView& operator<<(QWidget* pView);

//-----------------------------------------------------------------------------
// Reactions:
public:

  virtual void
    FinishEditing(QWidget* pView);

//-----------------------------------------------------------------------------
// Convenience methods:
public:

  virtual QWidget*
    ViewByData(const Handle(asiUI_PrimDataAdaptor)& Data) const;

  virtual QWidget*
    ViewByData(const int dataID) const;

  virtual Handle(asiUI_PrimDataAdaptor)
    DataByView(QWidget* pView) const;

  t_view
    ViewGroupByData(const Handle(asiUI_PrimDataAdaptor)& Data) const;

  t_view
    ViewGroupByData(const int dataID) const;

//-----------------------------------------------------------------------------
signals:

  void valueChanged(QWidget*);

//-----------------------------------------------------------------------------
protected slots:

  void
    onEditFinished();

//-----------------------------------------------------------------------------
private:

  bool
    viewToData(QWidget*                             pView,
               const Handle(asiUI_PrimDataAdaptor)& Data);

  bool
    dataToView(const Handle(asiUI_PrimDataAdaptor)& Data,
               QWidget*                             pView);

  bool
    isConsistent() const;

  Handle(asiUI_PrimDataAdaptor)
    dataByID(const int dataID) const;

//-----------------------------------------------------------------------------
// Core members:
protected:

  t_data_view_map         m_dataViewMap; //!< Data-View associations.
  Handle(asiEngine_Model) m_model;       //!< Data Model instance.

//-----------------------------------------------------------------------------
// Other members:
protected:

  //! Defines next data type for streaming.
  enum StreamChunk
  {
    Chunk_Data = 0x00001, //!< Data Adaptor goes 1st.
    Chunk_View = 0x00002, //!< View is streamed the 2nd.
  };

  //! Data Adaptor cached by the initialization routine.
  Handle(asiUI_PrimDataAdaptor) m_nextAdt;

  //! Next data type for streaming.
  int m_streamChunk;

};

#endif
