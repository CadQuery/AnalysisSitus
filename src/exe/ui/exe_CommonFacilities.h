//-----------------------------------------------------------------------------
// Created on: 27 November 2015
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

#ifndef exe_CommonFacilities_h
#define exe_CommonFacilities_h

// asiVisu includes
#include <asiVisu_PrsManager.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiUI includes
#include <asiUI_IStatusBar.h>
#include <asiUI_JournalIV.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ProgressListener.h>
#include <asiUI_ProgressNotifier.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerHost.h>

// asiTcl includes
#include <asiTcl_Interp.h>

//! Common tools and objects for A-Situs application.
class exe_CommonFacilities : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(exe_CommonFacilities, Standard_Transient)

public:

  Handle(asiEngine_Model)          Model;            //!< Data Model instance.
  //
  asiUI_ObjectBrowser*             ObjectBrowser;    //!< Object browser.
  Handle(ActAPI_INode)             CurrentNode;      //!< Currently selected Node.
  //
  asiUI_ViewerPart*                ViewerPart;       //!< Viewer for part.
  asiUI_ViewerDomain*              ViewerDomain;     //!< Viewer for face parametric domain.
  asiUI_ViewerHost*                ViewerHost;       //!< Viewer for host geometry.
  //
  Handle(ActAPI_IProgressNotifier) ProgressNotifier; //!< Progress notifier.
  asiUI_ProgressListener*          ProgressListener; //!< Progress listener.
  Handle(ActAPI_IPlotter)          Plotter;          //!< Imperative plotter.
  Handle(asiUI_IStatusBar)         StatusBar;        //!< Status bar of the main window.
  Handle(asiUI_Logger)             Logger;           //!< Logger.
  //
  Handle(asiTcl_Interp)            Interp;           //!< Tcl interpreter.

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
  static Handle(exe_CommonFacilities) Instance();

private:

  exe_CommonFacilities() //!< ctor.
  //
    : ObjectBrowser    (NULL),
      ViewerPart       (NULL),
      ViewerDomain     (NULL),
      ViewerHost       (NULL),
      ProgressListener (NULL)
  {
    // Create Data Model
    Model = new asiEngine_Model;
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
    ProgressNotifier = new asiUI_ProgressNotifier;
  }

};

#endif
