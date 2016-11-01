//-----------------------------------------------------------------------------
// Created on: 01 November 2016 (got tired fitting two B-spline curves)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_Dialog_h
#define asiUI_Dialog_h

// A-Situs includes
#include <asiUI.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#pragma warning(pop)

//! Base class for Analysis Situs dialogs. This one allows you to benefit from
//! the aggregated progress notifier and imperative plotter.
class asiUI_Dialog : public QDialog
{
  Q_OBJECT

public:

  asiUI_Dialog(ActAPI_ProgressEntry progress,
               ActAPI_PlotterEntry  plotter,
               QWidget*             parent = NULL)
  //
  : QDialog    (parent),
    m_notifier (progress),
    m_plotter  (plotter)
  {}

protected:

  ActAPI_ProgressEntry m_notifier; //!< Progress notifier.
  ActAPI_PlotterEntry  m_plotter;  //!< Imperative plotter.

};

#endif
