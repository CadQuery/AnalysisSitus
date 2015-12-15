//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_viewer_h
#define gui_viewer_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (visualization) includes
#include <visu_prs_manager.h>

// Qt includes
#pragma warning(push, 0)
#include <QWidget>
#pragma warning(pop)

//! Base class for all viewers.
class gui_viewer : public QWidget
{
  Q_OBJECT

public:

  gui_viewer(QWidget* parent = NULL);
  virtual ~gui_viewer();

public:

  virtual void Repaint() = 0;

protected:

  vtkSmartPointer<visu_prs_manager> m_prs_mgr; //!< Presentation Manager.

};

#endif
