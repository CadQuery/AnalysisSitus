//-----------------------------------------------------------------------------
// Created on: 10 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_prim_data_view_h
#define gui_prim_data_view_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_prim_data_adaptor.h>

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
class gui_prim_data_view : public QObject
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
    inline bool IsNull() const { return pWidget == NULL; }

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
  typedef NCollection_DataMap<Handle(gui_prim_data_adaptor),
                              t_view,
                              gui_data_adaptor::Hasher> t_data_view_map;

//-----------------------------------------------------------------------------
public:

  inline gui_prim_data_view(QObject* parent = NULL) : QObject(parent), m_streamChunk(Chunk_Data) {}

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

  gui_prim_data_view& operator<<(const Handle(gui_prim_data_adaptor)& Data);
  gui_prim_data_view& operator<<(QWidget* pView);

//-----------------------------------------------------------------------------
// Reactions:
public:

  virtual void
    FinishEditing(QWidget* pView);

//-----------------------------------------------------------------------------
// Convenience methods:
public:

  virtual QWidget*
    ViewByData(const Handle(gui_prim_data_adaptor)& Data) const;

  virtual QWidget*
    ViewByData(const int dataID) const;

  virtual Handle(gui_prim_data_adaptor)
    DataByView(QWidget* pView) const;

  t_view
    ViewGroupByData(const Handle(gui_prim_data_adaptor)& Data) const;

  t_view
    ViewGroupByData(const int dataID) const;

//-----------------------------------------------------------------------------
signals:

  void valueChanged(QWidget*);

//-----------------------------------------------------------------------------
protected slots:

  void onEditFinished();

//-----------------------------------------------------------------------------
private:

  bool
    viewToData(QWidget*                             pView,
               const Handle(gui_prim_data_adaptor)& Data);

  bool
    dataToView(const Handle(gui_prim_data_adaptor)& Data,
               QWidget*                             pView);

  bool
    isConsistent() const;

  Handle(gui_prim_data_adaptor)
    dataByID(const int dataID) const;

//-----------------------------------------------------------------------------
// Core members:
protected:

  t_data_view_map m_dataViewMap; //!< Data-View associations.

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
  Handle(gui_prim_data_adaptor) m_nextAdt;

  //! Next data type for streaming.
  int m_streamChunk;

};

#endif
