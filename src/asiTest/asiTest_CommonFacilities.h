//-----------------------------------------------------------------------------
// Created on: 22 June 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiTest_CommonFacilities_h
#define asiTest_CommonFacilities_h

// asiTest includes
#include <asiTest_ProgressNotifier.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiTcl includes
#include <asiTcl_Interp.h>

// asiUI includes
#include <asiUI_IV.h>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

//! Common tools and objects for unit tests.
class asiTest_CommonFacilities : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiTest_CommonFacilities, Standard_Transient)

public:

  Handle(asiEngine_Model)          Model;            //!< Data Model instance.
  Handle(ActAPI_IProgressNotifier) ProgressNotifier; //!< Progress notifier.
  Handle(ActAPI_IPlotter)          Plotter;          //!< Plotter.
  Handle(asiTcl_Interp)            Interp;           //!< Tcl interpreter.

public:

  //! \return single instance of facilities.
  static Handle(asiTest_CommonFacilities) Instance();

private:

  asiTest_CommonFacilities() //!< ctor.
  //
  : Standard_Transient()
  {
    // Create Data Model.
    this->Model = new asiEngine_Model;
    if ( !Model->NewEmpty() )
    {
      Standard_ProgramError::Raise("Cannot create Data Model");
    }
    //
    this->Model->DisableTransactions();
    {
      this->Model->Populate();
    }
    this->Model->EnableTransactions();

    // Initialize progress notifier.
    this->ProgressNotifier = new asiTest_ProgressNotifier(std::cout);

    // Initialize plotter as we may want at least to work with the data
    // objects created by the plotter if not with their presentations.
    this->Plotter = new asiUI_IV(this->Model, NULL, NULL, NULL);

    // Construct the interpreter
    this->Interp = new asiTcl_Interp;
    this->Interp->Init();
    this->Interp->SetModel(this->Model);
    this->Interp->SetProgress(this->ProgressNotifier);
    this->Interp->SetPlotter(this->Plotter);
  }

};

#endif
