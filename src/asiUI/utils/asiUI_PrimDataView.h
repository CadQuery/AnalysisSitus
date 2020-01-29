//-----------------------------------------------------------------------------
// Created on: 10 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
    t_view() : pWidget(nullptr) {}

    //! \return true if the View has not been initialized.
    bool IsNull() const { return pWidget == nullptr; }

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
                     QObject*                       parent = nullptr)
  //
  : QObject(parent), m_model(model), m_streamChunk(Chunk_Data) {}

//-----------------------------------------------------------------------------
// API methods:
public:

  virtual bool
    ViewToData(QWidget* pView = nullptr);

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
