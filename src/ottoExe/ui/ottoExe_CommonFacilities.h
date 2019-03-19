//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoExe_CommonFacilities_h
#define ottoExe_CommonFacilities_h

// ottoEngine includes
#include <ottoEngine_Model.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

// asiTcl includes
#include <asiTcl_Interp.h>

// asiUI includes
#include <asiUI_CommonFacilities.h>
#include <asiUI_IV.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ProgressListener.h>
#include <asiUI_ProgressNotifier.h>

//! Common tools and objects for ottoExe application.
class ottoExe_CommonFacilities : public asiUI_CommonFacilities
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(ottoExe_CommonFacilities, asiUI_CommonFacilities)

public:

  asiUI_ProgressListener*  ProgressListener; //!< Progress listener.
  Handle(asiUI_IStatusBar) StatusBar;        //!< Status bar of the main window.
  Handle(asiUI_Logger)     Logger;           //!< Logger.
  Handle(asiTcl_Interp)    Interp;           //!< Tcl interpreter.

  //! Visualization facilities.
  struct t_prs
  {
  //---------------------------------------------------------------------------
    vtkSmartPointer<asiVisu_PrsManager> Part;   //!< Part.
    vtkSmartPointer<asiVisu_PrsManager> Domain; //!< Face domain.
    vtkSmartPointer<asiVisu_PrsManager> Host;   //!< Host geometry.
  //---------------------------------------------------------------------------

    void DeleteAll()
    {
      if ( Part )   Part   ->DeleteAllPresentations();
      if ( Domain ) Domain ->DeleteAllPresentations();
      if ( Host )   Host   ->DeleteAllPresentations();
    }
  } Prs;

public:

  //! \return single instance of facilities.
  static Handle(ottoExe_CommonFacilities) Instance()
  {
    if ( m_ref.IsNull() )
      m_ref = new ottoExe_CommonFacilities;

    return m_ref;
  }

private:

  ottoExe_CommonFacilities()
  //
    : asiUI_CommonFacilities (),
      ProgressListener       (NULL)
  {
    // Create Data Model.
    Model = new ottoEngine_Model;
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

    // Set progress notifier for tree functions.
    Model->FuncSetProgressNotifier( this->Progress.Access() );
    Model->FuncProgressNotifierOn();
  }

private:

  static Handle(ottoExe_CommonFacilities) m_ref; //!< Single instance of facilities.

};

#endif
