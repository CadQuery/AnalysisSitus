//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeCC_CommonFacilities_h
#define exeCC_CommonFacilities_h

// exeCC includes
#include <exeCC_Model.h>

// asiAlgo includes
#include <asiAlgo_Notifier.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

// asiUI includes
#include <asiUI_JournalIV.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerSurface.h>

DEFINE_STANDARD_HANDLE(exeCC_CommonFacilities, Standard_Transient)

//! Common tools and objects for As-Built application.
class exeCC_CommonFacilities : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(exeCC_CommonFacilities, Standard_Transient)

public:

  Handle(exeCC_Model)              Model;         //!< Data Model instance.
  //
  asiUI_ObjectBrowser*             ObjectBrowser; //!< Object browser.
  Handle(ActAPI_INode)             CurrentNode;   //!< Currently selected Node.
  //
  asiUI_ViewerPart*                ViewerPart;    //!< Viewer for part.
  asiUI_ViewerDomain*              ViewerDomain;  //!< Viewer for face parametric domain.
  asiUI_ViewerSurface*             ViewerHost;    //!< Viewer for host geometry.
  //
  Handle(ActAPI_IProgressNotifier) Notifier;      //!< Algorithmic notifier.
  Handle(ActAPI_IPlotter)          Plotter;       //!< Algorithmic plotter.

  //! Visualization facilities.
  struct t_prs
  {
  //---------------------------------------------------------------------------
    vtkSmartPointer<asiVisu_PrsManager> Part;   //!< Part.
    vtkSmartPointer<asiVisu_PrsManager> Domain; //!< Face domain.
    vtkSmartPointer<asiVisu_PrsManager> Host;   //!< Host geometry.
  //---------------------------------------------------------------------------

    void ActualizeAll()
    {
      if ( Part )   Part   ->Actualize(exeCC_CommonFacilities::Instance()->Model->GetRootNode(), true);
      if ( Domain ) Domain ->Actualize(exeCC_CommonFacilities::Instance()->Model->GetRootNode(), true);
      if ( Host )   Host   ->Actualize(exeCC_CommonFacilities::Instance()->Model->GetRootNode(), true);
    }

    void DeleteAll()
    {
      if ( Part )   Part   ->DeleteAllPresentations();
      if ( Domain ) Domain ->DeleteAllPresentations();
      if ( Host )   Host   ->DeleteAllPresentations();
    }
  } Prs;

public:

  //! \return single instance of facilities.
  static Handle(exeCC_CommonFacilities) Instance()
  {
    if ( m_ref.IsNull() )
      m_ref = new exeCC_CommonFacilities;

    return m_ref;
  }

private:

  exeCC_CommonFacilities()
  //
    : ObjectBrowser (NULL),
      ViewerPart    (NULL),
      ViewerDomain  (NULL),
      ViewerHost    (NULL)
  {
    // Create Data Model
    Model = new exeCC_Model;
    if ( !Model->NewEmpty() )
    {
      Standard_ProgramError::Raise("Cannot create Data Model");
    }
    //
    Model->DisableTransactions();
    {
      Model->Populate();
    }
    Model->EnableTransactions();

    // Initialize notifier
    Notifier = new asiAlgo_Notifier;
  }

private:

  static Handle(exeCC_CommonFacilities) m_ref; //!< Single instance of facilities.

};

#endif
