//-----------------------------------------------------------------------------
// Created on: 27 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef exe_CommonFacilities_h
#define exe_CommonFacilities_h

// asiVisu includes
#include <asiVisu_PrsManager.h>

// asiUI includes
#include <asiUI_CommonFacilities.h>
#include <asiUI_IStatusBar.h>
#include <asiUI_IV.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ProgressListener.h>
#include <asiUI_ProgressNotifier.h>

// asiTcl includes
#include <asiTcl_Interp.h>

//! Common tools and objects for B-Rep inspector application.
class exe_CommonFacilities : public asiUI_CommonFacilities
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(exe_CommonFacilities, asiUI_CommonFacilities)

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
  static Handle(exe_CommonFacilities) Instance();

private:

  exe_CommonFacilities() //!< ctor.
  //
  : asiUI_CommonFacilities (),
    ProgressListener       (NULL)
  {
    // Create Data Model
    this->Model = new asiEngine_Model;
    //
    if ( !this->Model->NewEmpty() )
    {
      Standard_ProgramError::Raise("Cannot create Data Model");
    }
    //
    this->Model->DisableTransactions();
    {
      this->Model->Populate();
    }
    this->Model->EnableTransactions();
  }

};

#endif
