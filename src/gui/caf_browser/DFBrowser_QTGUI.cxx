//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#include <DFBrowser_QTGUI.hxx>

#include <DFBrowser_DFTree.hxx>
#include "DFBrowser_QTGUI2.h"

// icons
////
#include "../../res/xpm/qt_atribute.xpm"
#include "../../res/xpm/qt_closed_icon.xpm"
#include "../../res/xpm/qt_file_small.xpm"
#include "../../res/xpm/qt_folder_small.xpm"
#include "../../res/xpm/qt_node_attr.xpm"
#include "../../res/xpm/qt_ns_attr.xpm"
#include "../../res/xpm/qt_open_icon.xpm"
#include "../../res/xpm/qt_close.xpm"

// qt
//#define QT_THREAD_SUPPORT
#include <QSemaphore>
#include <qapplication.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qmotifstyle.h>
#include <qthread.h>
#include <QShowEvent>
#include <QAccessible>

// threads
#ifndef DEBUGBROWSER_THREADS
#define DEBUGBROWSER_THREADS
#ifndef WNT
#include <pthread.h>
#include <unistd.h>
#else
#include <windows.h>
#include <winbase.h>
#endif
#endif

#include <QCoreApplication>

#include <QWidget>

class CAFBrowserWidget : public QWidget//QListViewItem
{
public:
  enum { Title = 2000, Show = 2001 };
public:

  CAFBrowserWidget(QWidget* parent = 0);
  ~CAFBrowserWidget();

  void           setTitle(QString&);
protected:

  virtual void   customEvent( QEvent* );
  
private:

  QString        myTitle;
};

DFBrowser_QTGUIMOC *myQTGUI;

CAFBrowserWidget::CAFBrowserWidget(QWidget* parent)
: QWidget(parent)
{}

CAFBrowserWidget::~CAFBrowserWidget()
{}

CAFBrowserWidget* cafBrowser = NULL;

void CAFBrowserWidget::customEvent(QEvent* theE)
{
  switch(theE->type()){
    case Title : 
      setWindowTitle(myTitle);
      break;
    case Show : {
      show();
      myQTGUI->set_attribute(0);
      break;
    }
    default : break;
  }
}

void CAFBrowserWidget::setTitle(QString& theTitle)
{
  myTitle = theTitle;
}

DFBrowser_PtrGUI thisGUIs;
QSemaphore* myAppStarted;

class DFQThread
{
 public:

  void closeApplication() {}

  virtual void run ()
  {
    //initialization of the qt application
    //in main thread!
/*
#ifdef WNT
   int a(0);
   char *b = new char[2];
   b[0]='a';b[1]=0;
   new QApplication(a,&b); 
#endif 
*/
    //creating of widgets 
    cafBrowser = new CAFBrowserWidget();

    QVBoxLayout* theLayout = new QVBoxLayout( cafBrowser );
    theLayout->setMargin(0);
    theLayout->setSpacing(0);
    
    cafBrowser->setWindowTitle( QString( "Debug Browser 6.4" ) );
    myQTGUI = new DFBrowser_QTGUIMOC(thisGUIs,cafBrowser); 
    theLayout->addWidget(myQTGUI);
    cafBrowser->resize( 470, 800 );
    (*myAppStarted).release();

    QEvent* aE = new QEvent((QEvent::Type)2001);
    QApplication::postEvent(cafBrowser,aE);
/*
#ifdef WNT
    //start main application loop
    exec();
#endif
*/
   }
};

DFQThread *myQTh;

DFBrowser_QTGUI::DFBrowser_QTGUI (const DFBrowser_PtrGUI& theGUI)
{
  thisGUIs = theGUI;
  if (cafBrowser==NULL) {

    myAppStarted = new QSemaphore(1);
    (*myAppStarted).acquire();

    myQTh = new DFQThread();
//#ifdef WNT
//   myQTh->start();
//#else
    myQTh->run();
//#endif
    (*myAppStarted).acquire();
    delete(myAppStarted);
  } else {
   myQTGUI->SetGUI(thisGUIs);
   QEvent* aE = new QEvent((QEvent::Type)2001);
   QApplication::postEvent(cafBrowser,aE);
  }
}

void DFBrowser_QTGUI::Update()
{
  TCollection_AsciiString myBrowserName("OCAF Browser for ");

  thisGUIs->Tree()->AddDocName(myBrowserName);

  QEvent* aE = new QEvent((QEvent::Type)2000);
  QString aName( myBrowserName.ToCString() );
  cafBrowser->setTitle(aName);
  QApplication::postEvent(cafBrowser,aE);

  myQTGUI->Update();
}

const TCollection_AsciiString & DFBrowser_QTGUI::Filter() const
{
  return myQTGUI->Filter();
}

void DFBrowser_QTGUI::Close()
{
  cafBrowser->hide();
/* 
#ifndef WNT
  cafBrowser=NULL ;
  QApplication::quit();
#endif  
  */
}

void DFBrowser_QTGUI::Description(const const TCollection_AsciiString &theDescription)
{
  myQTGUI->Description(theDescription);
}

const TCollection_AsciiString & DFBrowser_QTGUI::Description() const
{
  return myQTGUI->Description();
}

Standard_Address DFBrowser_QTGUI::Pixmap(const DFBrowser_Picture thePicture)
{
  static QPixmap myFolderP(qt_folder_small_xpm);// = QPixmap::fromImage( QImage("..\..\res\caf_browser\qt_folder_small.png") );
  static QPixmap myFileP(qt_file_small);
  static QPixmap myNodeP(qt_node_attr_xpm);
  static QPixmap myClose(qt_close_xpm);
  static QPixmap myAttributeP(qt_atribute_xpm);
  static QPixmap myNSP(qt_ns_attr_xpm);
  static QPixmap myOpenedP(qt_open_icon);
  static QPixmap myClosedP(qt_closed_icon);

  switch(thePicture) {
  case DFBrowser_FOLDER: return &myFolderP;
  case DFBrowser_FILE: return &myFileP;
  case DFBrowser_NODE: return &myNodeP;
  case DFBrowser_CLOSE: return &myClose;
  case DFBrowser_LIST_ATTRIBUTE: return &myFolderP;
  case DFBrowser_STANDARD_ATTRIBUTE: return &myAttributeP;
  case DFBrowser_NS_ATTRIBUTE: return &myNSP;
  case DFBrowser_OPENED_TRIGGER: return &myOpenedP;
  case DFBrowser_CLOSED_TRIGGER: return &myClosedP;
  default: return &myClosedP;
  }
  //return NULL;
  return &myClosedP;

};

//#ifdef BUILD_WITH_STATIC_QT

//#endif
