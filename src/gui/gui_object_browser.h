//-----------------------------------------------------------------------------
// Created on: 03 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_object_browser_h
#define gui_object_browser_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// Qt includes
#pragma warning(push, 0)
#include <QTreeWidget>
#pragma warning(pop)

//! Tree view for A-Situs Data Model.
class gui_object_browser : public QTreeWidget
{
  Q_OBJECT

public:

  ASitus_EXPORT
    gui_object_browser(QWidget* parent = NULL);

  ASitus_EXPORT virtual
    ~gui_object_browser();

public:

  ASitus_EXPORT void
    Populate();

protected:

  void addChildren(const Handle(ActAPI_INode)& root_n, QTreeWidgetItem* root_ui);

};

#endif
