//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#include <DFBrowser_QTGUI2.h>

#include <DFBrowser_DFTree.hxx>
#include <DFBrowser_DFNode.hxx>
#include <DFBrowser_QTGUI.hxx>
#include <DFBrowser_LabelNode.hxx>
#include <DFBrowser_Attr.hxx>
// qt
#include <QLayout>
#include <qpushbutton.h>
#include <QHeaderView>
#include <QWindowsStyle>
#include <QColor>
#include <QComboBox>
#include <QCheckBox>
#include <qlabel.h>
#include <QLineEdit>
#include <QWidget>
#include <QGroupBox>
#include <QTextEdit>
#include <QMotifStyle>


#define QTGUI_ATTRIBUTE 0
#define QTGUI_ATTRIBUTELIST 1
#define QTGUI_HIDEATTRIBUTE 2

QGroupBox* gbbuttonFrame;
QGroupBox* gbdescriptionFrame;
QCheckBox* aDescription;
QCheckBox* aDescription2;
QTextEdit* anEdit;
QLineEdit* aFilter;
QLineEdit* aFilter2;

static QColor DFGetColor(const Handle(DFBrowser_DFNode) &node)
{
  switch (node->Color()) {
    case DFBrowser_BLACK: return Qt::black;
    case DFBrowser_GREEN: return Qt::darkGreen;
    case DFBrowser_LIGHTGRAY: return Qt::lightGray;
    case DFBrowser_GRAY: return Qt::gray;
    case DFBrowser_DARKGRAY: return Qt::darkGray;
    case DFBrowser_RED: return Qt::red;
    default : return Qt::black;
  }
  return Qt::black;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// QTGUIListViewItem //////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QTGUIListViewItem::rebuildCell()
{
  if(myNode.IsNull())
    return;
  this->setText(0,QString(myNode->Name().ToCString()));
  this->setIcon(0,QIcon(*((QPixmap*)(DFBrowser_QTGUI::Pixmap(myNode->Pixmap())))));

  QBrush aBrush(DFGetColor(myNode));
  this->setForeground(0,aBrush);

  Handle(DFBrowser_DFNode) aChild = myNode->Sub();
  Standard_Boolean aHasChilds = Standard_False;
  QTGUIListViewItem* anItem;
  QTGUIListViewItem* aPrevItem;
  // childs
  while(!aChild.IsNull()) 
    if (aChild->Visible()) {
      aHasChilds = Standard_True;
      break;
    } 
    else aChild = aChild->Next();
    if (!myNode->CanOpen()) aHasChilds = Standard_False;
    if (child(0) == NULL) {
      if (aHasChilds) { // just add all subitems
        aPrevItem = this;
        while(!aChild.IsNull()) {
	        if (!aChild->Visible()) {
	          aChild = aChild->Next();
	          continue;
	        }
	        anItem =  new QTGUIListViewItem(this,(QTreeWidgetItem*)aPrevItem);

          aPrevItem = anItem;
	        aChild->Update();
	        anItem->SetNode(aChild);
          anItem->rebuildCell();
	        aChild = aChild->Next();
        }
      }
    }
    else {
      if (aHasChilds) { // merge sturctures
        anItem = (QTGUIListViewItem*)child(0);
        while(anItem!=NULL) {
          if (anItem->GetNode().IsNull() ||
              anItem->GetNode()->Tree().IsNull() ||
              !anItem->GetNode()->Visible() ||
              anItem->GetNode()->Parent() != ((QTGUIListViewItem*)(anItem->parent()))->GetNode() ||
              (myNode->GetType() == DFBrowser_LABEL &&
               !(Handle(DFBrowser_LabelNode)::DownCast(myNode)->List().IsNull()) &&
               anItem->GetNode()->GetType() == DFBrowser_ATTRIBUTE))
          { // this node must be deleted here
            QTGUIListViewItem* anParent = (QTGUIListViewItem*)(anItem->parent());
            Standard_Integer aCurIdx = anParent->indexOfChild(anItem);
            anItem = (QTGUIListViewItem *)anParent->child( aCurIdx + 1 );
            this->takeChild( aCurIdx );
            continue;
          }
          QTGUIListViewItem* anParent = (QTGUIListViewItem*)(anItem->parent());
          Standard_Integer aCurIdx = anParent->indexOfChild(anItem);
          anItem = (QTGUIListViewItem *)anParent->child( aCurIdx + 1 );
        }
        aPrevItem = this;
        while(!aChild.IsNull()) {
	        if (!aChild->Visible()) {
	          aChild = aChild->Next();
	          continue;
	        }
	      anItem = (QTGUIListViewItem*)child(0);
	      while(anItem!=NULL) {
	        if (anItem->GetNode() == aChild) break;
          Standard_Integer aCurIdx = indexOfChild(anItem);
          anItem = (QTGUIListViewItem *)child( aCurIdx + 1 );
	      }
	      if (anItem==NULL) {
	        // remove all items at bottom
	        Handle(DFBrowser_DFNode) aBottom = aChild->Next();
	        while(!aBottom.IsNull()) {
	          if (!aBottom->Visible()) {
	            aBottom = aBottom->Next();
	            continue;
  	        }
	          QTGUIListViewItem * aBItem = (QTGUIListViewItem*)child( 0 );
	          while(aBItem!=NULL) {
	            if (aBItem->GetNode() == aBottom) break;
              Standard_Integer aCurIdx = indexOfChild(aBItem);
              aBItem = (QTGUIListViewItem *)child( aCurIdx + 1 );
	          }
	          if (aBItem!=NULL) {
              Standard_Integer aCurIdx = indexOfChild(aBItem);
              takeChild(aCurIdx);
            }
	          aBottom = aBottom->Next();
	        }
	  
	        anItem = new QTGUIListViewItem(this,aPrevItem);
	        aChild->Update();
	        anItem->SetNode(aChild);
	      }
	      aPrevItem = anItem;
	      aChild = aChild->Next();
      }
    }
    else {
      while(child(0)!=NULL){
        this->takeChild(0);
      }
    }
  }
  if (aHasChilds){
    bool isOpen = myNode->CanOpen() && myNode->Opened();
    myNode->Opened(isOpen);
    this->setExpanded(isOpen);
  }
  setText(0,text(0));
}

///////////////////////////////////////////////////////////////////////////////////
////////////////////////////// DFBrowser_QTGUIMOC //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
DFBrowser_QTGUIMOC::DFBrowser_QTGUIMOC(const DFBrowser_PtrGUI theGUI, QWidget* parent):QFrame(parent)
{
  myGUI = theGUI;
  
  // set the visual part
  qRegisterMetaType<QModelIndex>("QModelIndex");
  QVBoxLayout* theMain = new QVBoxLayout(this);
  theMain->setMargin(0);
  theMain->setSpacing(-1);
  qRegisterMetaType<QTreeWidgetItem>("QTreeWidgetItem");
  myLabelView = new QTreeWidget();
  gbbuttonFrame = new QGroupBox( ""/*, this*/ );
  QHBoxLayout *HBoxbuttonFrame = new QHBoxLayout(gbbuttonFrame);
  
  gbdescriptionFrame = new QGroupBox( ""/*, this*/ );
  QHBoxLayout *HBoxdescriptionFrame = new QHBoxLayout(gbdescriptionFrame);
  HBoxdescriptionFrame->setMargin(2);
  
  QFrame* theFirst = new QFrame(gbbuttonFrame);
  QGridLayout* theFirstlayout = new QGridLayout(theFirst);
  theFirstlayout->setMargin(0);
  theFirstlayout->setSpacing(-1);

  QComboBox* aLevel = new QComboBox(theFirst);
  aLevel->insertItem(DFBrowser_SHOWDATA,"data level");
  aLevel->insertItem(DFBrowser_SHOWFIRST,"first level");
  aLevel->insertItem(DFBrowser_SHOWALL,"all levels");
  aLevel->setCurrentIndex(DFBrowser_SHOWDATA);
  connect(aLevel,SIGNAL(triggered(int)), SLOT(set_level(int)));

  QComboBox* anAttr = new QComboBox(theFirst);
  anAttr->insertItem(QTGUI_ATTRIBUTE,"attribute");
  anAttr->insertItem(QTGUI_ATTRIBUTELIST,"attribute list");
  anAttr->insertItem(QTGUI_HIDEATTRIBUTE,"hide attribute");
  anAttr->setCurrentIndex(QTGUI_ATTRIBUTE);
  connect(anAttr,SIGNAL(triggered(int)), SLOT(set_attribute(int)));

  theFirstlayout->addWidget(aLevel,0,0,1,1);
  theFirstlayout->addWidget(anAttr, 0, 1,1,1);

  QFrame* theCheckFrame = new QFrame(gbbuttonFrame);
  QVBoxLayout* theChecklayout = new QVBoxLayout( theCheckFrame );
  theChecklayout->setMargin(0);
  theChecklayout->setSpacing(-1);
  QCheckBox* aNS = new QCheckBox("NS tree", theCheckFrame);
  connect(aNS, SIGNAL(toggled(bool)), SLOT(ns_show(bool)));
  theChecklayout->addWidget(aNS);
  
  QFrame* thePushFrame = new QFrame(gbbuttonFrame);
  QVBoxLayout* thePushlayout = new QVBoxLayout( thePushFrame );
  thePushlayout->setMargin(-1);
  thePushlayout->setSpacing(-1);
  QPushButton* update = new QPushButton("Update", thePushFrame);
  connect(update, SIGNAL(clicked()), SLOT(update_tree()));
  thePushlayout->addWidget(update);

  QFrame* theSecond = new QFrame(gbbuttonFrame);
  QHBoxLayout* theSecondlayout = new QHBoxLayout( theSecond );
  theSecondlayout->setMargin(0);
  theSecondlayout->setSpacing(-1);
  QLabel* aFilterL = new QLabel("Filter:", theSecond);
  aFilter = new QLineEdit(theSecond);
  connect(aFilter,SIGNAL(textChanged(const QString&)), SLOT(filter_changed(const QString&)));

  theSecondlayout->addWidget(aFilterL);
  theSecondlayout->addWidget(aFilter);
  theFirstlayout->addWidget(theSecond,1,0,1,2);

  aDescription = new QCheckBox("Description",theCheckFrame);
  connect(aDescription,SIGNAL(toggled(bool)),SLOT(description(bool)));
  theChecklayout->addWidget(aDescription);
  
  
  QPushButton* close_w = new QPushButton("Close",thePushFrame);
  connect(close_w, SIGNAL(clicked()), SLOT(close_app()));
  thePushlayout->addWidget(close_w);

  HBoxbuttonFrame->addWidget(theFirst);
  HBoxbuttonFrame->addWidget(theCheckFrame);
  HBoxbuttonFrame->addWidget(thePushFrame);

  //create descriptionframe
  // description frame content

  QWidget* aGrid = new QWidget(gbdescriptionFrame);
  QGridLayout *HGridlayout = new QGridLayout(aGrid);
  HGridlayout->setMargin(2);
  HGridlayout->setSpacing(-1);
  
  QComboBox* anAttr2 = new QComboBox(aGrid);
  anAttr2->insertItem(QTGUI_ATTRIBUTE,"attribute");
  anAttr2->insertItem(QTGUI_ATTRIBUTELIST,"attribute list");
  anAttr2->insertItem(QTGUI_HIDEATTRIBUTE,"hide attribute");
  anAttr2->setCurrentIndex(QTGUI_ATTRIBUTE);
  connect(anAttr2,SIGNAL(triggered(int)), SLOT(set_attribute(int)));
  HGridlayout->addWidget(anAttr2, 0, 0);

  aDescription2 = new QCheckBox("Description", aGrid);
  connect(aDescription2,SIGNAL(toggled(bool)),SLOT(description(bool)));
  HGridlayout->addWidget(aDescription2, 0, 1, 1, 1);

  QComboBox* aLevel2 = new QComboBox(aGrid);
  aLevel2->insertItem(DFBrowser_SHOWDATA,"data level");
  aLevel2->insertItem(DFBrowser_SHOWFIRST,"first level");
  aLevel2->insertItem(DFBrowser_SHOWALL,"all levels");
  aLevel2->setCurrentIndex(DFBrowser_SHOWDATA);
  connect(aLevel2,SIGNAL(triggered(int)), SLOT(set_level(int)));
  HGridlayout->addWidget(aLevel2, 1, 0, 1, 1);

  QCheckBox* aDetailed = new QCheckBox("Detailed", aGrid);
  aDetailed->setChecked(Standard_True);
  connect(aDetailed,SIGNAL(toggled(bool)),SLOT(detailed_event(bool)));
  HGridlayout->addWidget(aDetailed, 1, 1, 1, 1);

  QFrame* aFilterBox = new QFrame(aGrid);
  QHBoxLayout *HFilterBoxlayout = new QHBoxLayout( aFilterBox );
  HFilterBoxlayout->setMargin(2);
  HFilterBoxlayout->setSpacing(0);

  QLabel* aFilterL2 = new QLabel("Filter:", aFilterBox);
  aFilter2 = new QLineEdit(aFilterBox);
  connect(aFilter2,SIGNAL(textChanged(const QString&)), SLOT(filter_changed(const QString&)));
  HFilterBoxlayout->addWidget(aFilterL2);
  HFilterBoxlayout->addWidget(aFilter2);
  HGridlayout->addWidget(aFilterBox,2,0,1,2);

  QPushButton* update2 = new QPushButton("Update", aGrid);
  connect(update2, SIGNAL(clicked()), SLOT(update_tree()));
  HGridlayout->addWidget(update2, 3, 0, 1, 1);

  QPushButton* close_w2 = new QPushButton("Close", aGrid);
  connect(close_w2, SIGNAL(clicked()), SLOT(close()));
  HGridlayout->addWidget(close_w2, 3, 1, 1, 1);

  gbdescriptionFrame->hide();

  anEdit = new QTextEdit(aGrid);
  anEdit->setReadOnly(Standard_True);
  QFont aFont("Helvetica",10);
  anEdit->setFont(aFont);
  anEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  HGridlayout->addWidget(anEdit,0,2,4,1);

  HBoxdescriptionFrame->addWidget(aGrid);

  myLabelView->setRootIsDecorated(Standard_True);
  myLabelView->setAllColumnsShowFocus(Standard_True);
  myLabelView->setStyle(new QWindowsStyle);
  myLabelView->setColumnCount(1);
  myLabelView->setSortingEnabled(Standard_False);
  #ifndef WNT
  myLabelView->setStyle(new QMotifStyle);
  #endif
  myLabelView->header()->setResizeMode(QHeaderView::Fixed);
  myLabelView->header()->setResizeMode(0,QHeaderView::Interactive);
  myLabelView->header()->setMovable(Standard_False);
  myLabelView->header()->hide();
  myLabelView->setFont(aFont);

  connect(myLabelView,SIGNAL(itemSelectionChanged()),SLOT(selection_changed()));
  connect(myLabelView,SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int )),SLOT(double_clicked(QTreeWidgetItem*, int)));
  
  myLabelView->setPalette(QPalette(QColor(255,255,255),QColor(192,192,192)));
  gbbuttonFrame->setPalette(QPalette(QColor(192,192,192),QColor(192,192,192)));
  gbdescriptionFrame->setPalette(QPalette(QColor(192,192,192),QColor(192,192,192)));

  gbdescriptionFrame->setLayout(HBoxdescriptionFrame);

  theMain->addWidget(myLabelView);
  theMain->addWidget(gbbuttonFrame);
  theMain->addWidget(gbdescriptionFrame);
}

DFBrowser_QTGUIMOC::~DFBrowser_QTGUIMOC()
{
//   delete(buttonFrame);
//   delete(descriptionFrame);
//   delete(aDescription);
//   delete(aDescription2);
//   delete(anEdit);
//   delete(aFilter);
//   delete(aFilter2);
  QTGUIListViewItem* anItem = (QTGUIListViewItem*)(myLabelView->topLevelItem(0));
  if (anItem!=NULL) myLabelView->takeTopLevelItem(0);
}

void DFBrowser_QTGUIMOC::SetGUI(const DFBrowser_PtrGUI theGUI)
{
  QTGUIListViewItem* anItem = (QTGUIListViewItem*)(myLabelView->topLevelItem(0));
  if (anItem!=NULL) myLabelView->takeTopLevelItem(0);
  myGUI = theGUI;
  Standard_Boolean aGroupView = Standard_False; 
  if(aDescription2->checkState() == Qt::Checked)
    aGroupView = Standard_True;
  description(aGroupView);
}

void DFBrowser_QTGUIMOC::Update()
{
  QTGUIListViewItem* anItem = (QTGUIListViewItem*)(myLabelView->topLevelItem(0));
  if (anItem==NULL) {
    anItem =  new QTGUIListViewItem((QTreeWidget*)0);
    anItem->SetNode(myGUI->Tree()->Root());
    myLabelView->addTopLevelItem(anItem);
    myGUI->SetLevel((DFBrowser_Level)0);
    anItem->rebuildCell();
  }
  else {
    Standard_Boolean aBack = Standard_False;
    while(anItem !=NULL) {
      if (!anItem->GetNode().IsNull()) if (!anItem->GetNode()->Tree().IsNull()) if (anItem->GetNode()->Changed()) {
	        anItem->rebuildCell();
	        anItem->GetNode()->Changed(Standard_False);
      }
      if (anItem->child(0) != NULL && anItem->isExpanded() && !aBack)
        anItem = (QTGUIListViewItem*)(anItem->child(0));
      else {
        QTGUIListViewItem* aSibling = NULL;
        QTGUIListViewItem* anParent = (QTGUIListViewItem*)(anItem->parent());
        if(anParent!=NULL) {
          Standard_Integer aCurIdx = anParent->indexOfChild(anItem);
          aSibling = (QTGUIListViewItem *)anParent->child( aCurIdx + 1 );
        }        
        if (aSibling != NULL) {
	        anItem = (QTGUIListViewItem*)(aSibling);
	        aBack = Standard_False;
        } else if (anItem->parent() != NULL) {
	        aBack = Standard_True;
	        anItem = (QTGUIListViewItem*)(anItem->parent());
        } else anItem = NULL;
      }
    }
  }
}

void DFBrowser_QTGUIMOC::Description(const TCollection_AsciiString &theDescription)
{
  myDescription = theDescription;
  anEdit->setReadOnly(Standard_False);
  anEdit->clear();
  anEdit->insertPlainText((const char *)myDescription.ToCString());
  anEdit->setReadOnly(Standard_True);
}

void DFBrowser_QTGUIMOC::update_tree()
{
  myGUI->Tree()->Update();
}

void DFBrowser_QTGUIMOC::set_level(Standard_Integer index)
{
  myGUI->SetLevel((DFBrowser_Level)index);
  myGUI->Update();
  myLabelView->resizeColumnToContents(0);
}

void DFBrowser_QTGUIMOC::set_attribute(Standard_Integer index)
{
  myGUI->SetAttrPrs((DFBrowser_AttrPrs)index);
  myLabelView->takeTopLevelItem(0);
  myGUI->Update();
  myLabelView->resizeColumnToContents(0);
}

void DFBrowser_QTGUIMOC::ns_show(bool on)
{
  myGUI->ShowNS(on);
}

void DFBrowser_QTGUIMOC::description(bool on)
{
  if (on) {
    gbbuttonFrame->hide();
    gbdescriptionFrame->show();
    gbdescriptionFrame->setFixedHeight(anEdit->height()+20);
    aDescription2->setChecked(Standard_True);
    aFilter2->setText(aFilter->text());
  } else {
    gbbuttonFrame->show();
    gbdescriptionFrame->hide();
    aDescription->setChecked(Standard_False);
    aFilter->setText(aFilter2->text());
  }
}

void DFBrowser_QTGUIMOC::filter_changed(const QString& theText)
{
  if (theText.toLatin1().constData()!=NULL)
    myFilterString.AssignCat(theText.toLatin1().constData());
  else
    myFilterString.Clear();
  Handle(DFBrowser_DFNode) aFirst = myGUI->First();
  if (!myGUI->DoFilter(aFirst)) aFirst->Visible(Standard_True); // root must be visible always
  myLabelView->takeTopLevelItem(0);
  myGUI->Update();
}

void DFBrowser_QTGUIMOC::detailed_event(bool on)
{
  myGUI->Tree()->Attr()->Detailed(on);
  myGUI->Tree()->Update();
}

void DFBrowser_QTGUIMOC::close_app()
{
  ((QWidget*)parent())->hide();
}

void DFBrowser_QTGUIMOC::selection_changed()
{
  if(!myLabelView->selectedItems().isEmpty()) {
    QTGUIListViewItem* aCurList = (QTGUIListViewItem*)(myLabelView->selectedItems().first());
    if(aCurList!=NULL) {
      myGUI->Selected(aCurList->GetNode());
      myLabelView->resizeColumnToContents(0);
    }
  }
}

void DFBrowser_QTGUIMOC::double_clicked(QTreeWidgetItem* /*theItem*/, int col)
{
  if(col > -1) {
    myGUI->DoubleClicked(((QTGUIListViewItem*)(myLabelView->selectedItems().first()))->GetNode());
    myLabelView->resizeColumnToContents(0);
  }
}

void DFBrowser_QTGUIMOC::hideEvent(QHideEvent*)
{
  myGUI->Close();
}
