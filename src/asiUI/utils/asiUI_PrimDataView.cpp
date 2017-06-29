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

// Own include
#include <asiUI_PrimDataView.h>

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
bool asiUI_PrimDataView::ViewToData(QWidget* pView)
{
  if ( !this->isConsistent() )
    return false;

  // Iterate over the entire set of associations to transfer data
  bool isAnythingDone = false;
  for ( t_data_view_map::Iterator it(m_dataViewMap); it.More(); it.Next() )
  {
    const Handle(asiUI_PrimDataAdaptor)& Data = it.Key();
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
bool asiUI_PrimDataView::DataToView()
{
  if ( !this->isConsistent() )
    return false;

  // Iterate over the entire set of associations to transfer data
  for ( t_data_view_map::Iterator it(m_dataViewMap); it.More(); it.Next() )
  {
    const Handle(asiUI_PrimDataAdaptor)& Data = it.Key();
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
asiUI_PrimDataView&
  asiUI_PrimDataView::operator<<(const Handle(asiUI_PrimDataAdaptor)& Data)
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
asiUI_PrimDataView& asiUI_PrimDataView::operator<<(QWidget* pView)
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
void asiUI_PrimDataView::FinishEditing(QWidget* pView)
{
  if ( m_model->HasOpenCommand() )
    return; // Some transaction is already active, so let's get out from here...

  bool isAnyChanged = false;
  //
  m_model->OpenCommand(); // tx begin
  {
    isAnyChanged = this->ViewToData(pView);
  }
  isAnyChanged ? m_model->CommitCommand() : m_model->AbortCommand(); // tx end

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
QWidget* asiUI_PrimDataView::ViewByData(const Handle(asiUI_PrimDataAdaptor)& Data) const
{
  return ViewGroupByData(Data).pWidget;
}

//! Returns View for the given Data addressed by ID.
//! \param dataID [in] Data ID to access the View for.
//! \return corresponding View.
QWidget* asiUI_PrimDataView::ViewByData(const int dataID) const
{
  return ViewGroupByData(dataID).pWidget;
}

//! Returns Data corresponding to the passed View.
//! \param pView [in] View to get data PID for.
//! \return Data corresponding to the passed View.
Handle(asiUI_PrimDataAdaptor) asiUI_PrimDataView::DataByView(QWidget* pView) const
{
  Handle(asiUI_PrimDataAdaptor) aResult;
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
asiUI_PrimDataView::t_view
  asiUI_PrimDataView::ViewGroupByData(const Handle(asiUI_PrimDataAdaptor)& Data) const
{
  return m_dataViewMap.Find(Data);
}

//! Returns View Group by its Data ID.
//! \param dataID [in] the Data identifier.
//! \return associated view group.
asiUI_PrimDataView::t_view
  asiUI_PrimDataView::ViewGroupByData(const int dataID) const
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
void asiUI_PrimDataView::onEditFinished()
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
bool asiUI_PrimDataView::viewToData(QWidget*                             pView,
                                    const Handle(asiUI_PrimDataAdaptor)& Data)
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
bool asiUI_PrimDataView::dataToView(const Handle(asiUI_PrimDataAdaptor)& Data,
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
bool asiUI_PrimDataView::isConsistent() const
{
  return (m_streamChunk & Chunk_Data);
}

//! \param dataID [in] registered Data identifier.
//! \return corresponding (to the specified ID) data adaptor.
Handle(asiUI_PrimDataAdaptor) asiUI_PrimDataView::dataByID(const int dataID) const
{
  Handle(asiUI_PrimDataAdaptor) result;
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
