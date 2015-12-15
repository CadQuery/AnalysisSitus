//-----------------------------------------------------------------------------
// Created on: 10 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <gui_prim_data_view.h>

// A-Situs (common) includes
#include <common_facilities.h>

// Qt includes
#pragma warning(push, 0)
#include <QLineEdit>
#pragma warning(pop)

//-----------------------------------------------------------------------------
// Data transferring
//-----------------------------------------------------------------------------

//! Transfers data from View to Model. Notice that if you are using
//! transactional approach to data modification, then this method should be
//! invoked in a transactional scope.
//!
//! \param pView [in] optional reference to the actual View to update.
//! \return true in case if anything has been transferred, false -- otherwise.
bool gui_prim_data_view::ViewToData(QWidget* pView)
{
  if ( !this->isConsistent() )
    return false;

  // Iterate over the entire set of associations to transfer data
  bool isAnythingDone = false;
  for ( t_data_view_map::Iterator it(m_dataViewMap); it.More(); it.Next() )
  {
    const Handle(gui_prim_data_adaptor)& Data = it.Key();
    QWidget* pWidget = it.Value().pWidget;

    if ( pView && (pWidget != pView) )
      continue; // Skip improper ones

    if ( this->viewToData(pWidget, Data) )
      if ( !isAnythingDone )
        isAnythingDone = true;
  }

  return isAnythingDone;
}

//! Transfers data from Model to View. This is a read-only method, so you
//! can invoke it out of a transactional scope.
//! \return true in case if anything has been transferred, false -- otherwise.
bool gui_prim_data_view::DataToView()
{
  if ( !this->isConsistent() )
    return false;

  // Iterate over the entire set of associations to transfer data
  for ( t_data_view_map::Iterator it(m_dataViewMap); it.More(); it.Next() )
  {
    const Handle(gui_prim_data_adaptor)& Data = it.Key();
    QWidget* pWidget = it.Value().pWidget;

    if ( !this->dataToView(Data, pWidget) )
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

//! Registers the passed Adaptor in role of Data. Next streamed item
//! is expected to be View.
//! \param Data [in] Adaptor to use for data sourcing.
//! \return this for subsequent streaming.
gui_prim_data_view&
  gui_prim_data_view::operator<<(const Handle(gui_prim_data_adaptor)& Data)
{
  if ( !(m_streamChunk & Chunk_Data) )
    Standard_ProgramError::Raise("Bad order of Data->View streaming (Prim DV): Data");

  m_nextAdt     = Data;
  m_streamChunk = Chunk_View;
  return *this;
}

//! Registers the passed Datum in role of View. Next streamed item
//! (if any) is expected to be PID.
//! \param pView [in] Datum representing the View.
//! \return this for subsequent streaming.
gui_prim_data_view& gui_prim_data_view::operator<<(QWidget* pView)
{
  if ( !(m_streamChunk & Chunk_View) )
    Standard_ProgramError::Raise("Bad order of Data->View streaming (Prim DV): View");

  // Creates view group
  t_view ViewGroup;
  ViewGroup.pWidget = pView;

  // Establish a listener for edit completion
  if ( qobject_cast<QLineEdit*>(pView) )
  {
    connect( pView, SIGNAL( editingFinished() ), this, SLOT( onEditFinished() ) );
  }

  // Bind Data-View connection
  m_dataViewMap.Bind( m_nextAdt, ViewGroup );
  m_streamChunk = Chunk_Data;
  return *this;
}

//-----------------------------------------------------------------------------
// Reactions
//-----------------------------------------------------------------------------

//! Finalizes editing session for the passed View.
//! \param pView [in] View to complete editing session for.
void gui_prim_data_view::FinishEditing(QWidget* pView)
{
  Handle(ActAPI_IModel) M = common_facilities::Instance()->Model;
  if ( M->HasOpenCommand() )
    return; // Some transaction is already active, so let's get out from here...

  bool isAnyChanged = false;
  //
  M->OpenCommand(); // tx begin
  {
    isAnyChanged = this->ViewToData(pView);
  } // tx end
  isAnyChanged ? M->CommitCommand() : M->AbortCommand();

  if ( isAnyChanged )
  {
    // Send a signal to allow somebody else to put his reaction
    emit valueChanged(pView);
  }
}

//-----------------------------------------------------------------------------
// Convenience methods
//-----------------------------------------------------------------------------

//! Returns View for the given Data.
//! \param Data [in] Data to access the View for.
//! \return corresponding View.
QWidget* gui_prim_data_view::ViewByData(const Handle(gui_prim_data_adaptor)& Data) const
{
  return ViewGroupByData(Data).pWidget;
}

//! Returns View for the given Data addressed by ID.
//! \param dataID [in] Data ID to access the View for.
//! \return corresponding View.
QWidget* gui_prim_data_view::ViewByData(const int dataID) const
{
  return ViewGroupByData(dataID).pWidget;
}

//! Returns Data corresponding to the passed View.
//! \param pView [in] View to get data PID for.
//! \return Data corresponding to the passed View.
Handle(gui_prim_data_adaptor) gui_prim_data_view::DataByView(QWidget* pView) const
{
  Handle(gui_prim_data_adaptor) aResult;
  for ( t_data_view_map::Iterator it(m_dataViewMap); it.More(); it.Next() )
  {
    if ( it.Value().pWidget == pView )
    {
      aResult = it.Key();
      break;
    }
  }
  return aResult;
}

//! Returns View Group for the given Data.
//! \param Data [in] Data to access the View Group for.
//! \return corresponding View Group.
gui_prim_data_view::t_view
  gui_prim_data_view::ViewGroupByData(const Handle(gui_prim_data_adaptor)& Data) const
{
  return m_dataViewMap.Find(Data);
}

//! Returns View Group by its Data ID.
//! \param dataID [in] the Data identifier.
//! \return associated view group.
gui_prim_data_view::t_view
  gui_prim_data_view::ViewGroupByData(const int dataID) const
{
  t_view result;
  for ( t_data_view_map::Iterator it(m_dataViewMap); it.More(); it.Next() )
  {
    if ( it.Key()->ID() == dataID )
    {
      result = it.Value();
      break;
    }
  }

  return result;
}

//-----------------------------------------------------------------------------
// Qt slots
//-----------------------------------------------------------------------------

//! Triggered once modification on any managed Datum completes.
void gui_prim_data_view::onEditFinished()
{
  // Access sender
  QWidget* sender = qobject_cast<QWidget*>( this->sender() );
  if ( !sender )
    return; // Hm-m...

  // Finalize editing session
  this->FinishEditing(sender);
}

//-----------------------------------------------------------------------------
// Internals
//-----------------------------------------------------------------------------

//! Transfers data from the given View to the given Data Model item.
//! \param pView [in] source View.
//! \param Data  [in] target Data Adaptor.
//! \return true if data was transferred, false -- otherwise.
bool gui_prim_data_view::viewToData(QWidget*                             pView,
                                    const Handle(gui_prim_data_adaptor)& Data)
{
  if ( !qobject_cast<QLineEdit*>(pView) )
    Standard_ProgramError::Raise("This kind of view is not supported");

  QVariant QDataVal = Data->GetValue();
  QVariant QViewVal = qobject_cast<QLineEdit*>(pView)->text();

  if ( !QDataVal.isValid() || !QViewVal.isValid() )
    return false;

  //---------------------------------------------------------------------------
  // Smart way to modify data is to check whether it is really going to change
  //---------------------------------------------------------------------------

  if ( (QDataVal.type() == QVariant::Int) && QDataVal.toInt() == QViewVal.toInt() )
    return false;

  if ( (QDataVal.type() == QVariant::Double) && Abs( QDataVal.toDouble() - QViewVal.toDouble() ) < RealSmall() )
    return false;

  if ( (QDataVal.type() == QVariant::Bool) && QDataVal.toBool() == QViewVal.toBool() )
    return false;

  if ( (QDataVal.type() == QVariant::String) && QDataVal.toString() == QViewVal.toString() )
    return false;

  Data->SetValue(QViewVal);
  return true;
}

//! Transfers data from the given Data Model item to the given View.
//! \param Data  [in] source Data Adaptor.
//! \param pView [in] target View.
//! \return true if data was transferred, false -- otherwise.
bool gui_prim_data_view::dataToView(const Handle(gui_prim_data_adaptor)& Data,
                                    QWidget*                             pView)
{
  if ( !qobject_cast<QLineEdit*>(pView) )
    Standard_ProgramError::Raise("This kind of view is not supported");

  pView->blockSignals(true); // Let's put our gift in Ded Moroz mode
  {
    t_view ViewGroup = this->ViewGroupByData(Data);
    //
    QVariant QDataVal = Data->GetValue();
    qobject_cast<QLineEdit*>(pView)->setText( QDataVal.toString() );
  }
  pView->blockSignals(false);
  return true;
}

//! Returns true if Data-View is initialized properly, false -- otherwise.
//! \return true/false.
bool gui_prim_data_view::isConsistent() const
{
  return (m_streamChunk & Chunk_Data);
}

//! \param dataID [in] registed Data identifier.
//! \return corresponding (to the specified ID) data adaptor.
Handle(gui_prim_data_adaptor) gui_prim_data_view::dataByID(const int dataID) const
{
  Handle(gui_prim_data_adaptor) result;
  for ( t_data_view_map::Iterator it(m_dataViewMap); it.More(); it.Next() )
  {
    if ( it.Key()->ID() == dataID )
    {
      result = it.Key();
      break;
    }
  }

  return result;
}
