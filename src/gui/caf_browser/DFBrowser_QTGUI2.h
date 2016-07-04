//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef QTDEBUGBROWSER_H
#define QTDEBUGBROWSER_H

#pragma warning(push, 0)
#include <QFrame>
#include <QTreeWidget>
#include <QButtonGroup>
#pragma warning(pop)

#include <TCollection_AsciiString.hxx>

#include <DFBrowser_GUI.hxx>
#include <DFBrowser_PtrGUI.hxx>
#include <DFBrowser_DFNode.hxx>

/************************************************************/
class QTGUIListViewItem : public QTreeWidgetItem //QListViewItem
{
 public:

  inline QTGUIListViewItem(QTreeWidgetItem* parent)
  : QTreeWidgetItem(parent) {}

  inline QTGUIListViewItem(QTreeWidgetItem* parent, QTreeWidgetItem* after)
  : QTreeWidgetItem(parent,after) {}

  inline QTGUIListViewItem(QTreeWidget* parent)
  : QTreeWidgetItem(parent) {}

  inline ~QTGUIListViewItem()
  {}

  inline void SetNode(const Handle(DFBrowser_DFNode)& theNode)
  {
    myNode = theNode;
  }

  inline const Handle(DFBrowser_DFNode) & GetNode() const
  {
    return myNode;
  }

  // GUI methods
  void rebuildCell();

 private:
  Handle(DFBrowser_DFNode) myNode;
};
/************************************************************/

/************************************************************/
class DFBrowser_QTGUIMOC : public QFrame
{
  Q_OBJECT
 public:
  DFBrowser_QTGUIMOC(const DFBrowser_PtrGUI theGUI, QWidget* parent = 0);

  ~DFBrowser_QTGUIMOC();

  inline const TCollection_AsciiString & Filter() const
  {
    return myFilterString;
  }

  void Description(const TCollection_AsciiString &theDescription);

  inline const TCollection_AsciiString & Description() const
  {
    return myDescription;
  }

  void SetGUI(const DFBrowser_PtrGUI theGUI);

 protected:
  void hideEvent(QHideEvent*);

 public slots:
  void Update();
  void update_tree();
  void close_app();
  void description(bool on);
  void set_level(int index);
  void set_attribute(int index);
  void ns_show(bool on);
  void filter_changed(const QString& theText);
  void detailed_event(bool on);
  void selection_changed();
  void double_clicked(QTreeWidgetItem*, int);

 private:
  DFBrowser_PtrGUI myGUI;
  QTreeWidget* myLabelView;
  TCollection_AsciiString myFilterString;
  TCollection_AsciiString myDescription;
};
/************************************************************/

#endif /* QTDEBUGBROWSER_H */
