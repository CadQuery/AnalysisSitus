//-----------------------------------------------------------------------------
// Created on: 24 August 2017
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

#ifndef asiUI_CommonFacilities_h
#define asiUI_CommonFacilities_h

// asiUI includes
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ProgressNotifier.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerPart.h>
#include <asiUI_ViewerHost.h>
#include <asiUI_WidgetFactory.h>

// Qt includes
#pragma warning(push, 0)
#include <QStandardPaths>
#pragma warning(pop)

class QMainWindow;

//! Base class for common facilities.
class asiUI_CommonFacilities : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_CommonFacilities, Standard_Transient)

public:

  // Default ctor.
  asiUI_CommonFacilities() : Standard_Transient (),
                             MainWindow         (NULL),
                             ObjectBrowser      (NULL),
                             ViewerPart         (NULL),
                             ViewerDomain       (NULL),
                             ViewerHost         (NULL),
                             UnitManager        (NULL)
  {
    WidgetFactory = new asiUI_WidgetFactory(this);

    // Initialize notifier.
    this->Progress = ActAPI_ProgressEntry(new asiUI_ProgressNotifier);
  }

public:

  //! Cleans up all the passed viewers. Null pointers are allowed in case if
  //! some viewer should not be touched or does not exist.
  //! \param[in] pViewerPart   part viewer.
  //! \param[in] pViewerDomain domain viewer.
  //! \param[in] pViewerHost   host viewer.
  //! \param[in] repaint indicates whether to repaint the corresponding widgets.
  static void ClearViewers(asiUI_ViewerPart*   pViewerPart,
                           asiUI_ViewerDomain* pViewerDomain,
                           asiUI_ViewerHost*   pViewerHost,
                           const bool          repaint)
  {
    // Get all presentation managers
    asiVisu_PrsManager* partPM   = pViewerPart   ? pViewerPart->PrsMgr()   : NULL;
    asiVisu_PrsManager* hostPM   = pViewerDomain ? pViewerDomain->PrsMgr() : NULL;
    asiVisu_PrsManager* domainPM = pViewerHost   ? pViewerHost->PrsMgr()   : NULL;

    // Update viewers
    if ( partPM )   partPM   ->DeleteAllPresentations();
    if ( hostPM )   hostPM   ->DeleteAllPresentations();
    if ( domainPM ) domainPM ->DeleteAllPresentations();

    if ( repaint )
    {
      if ( pViewerPart )   pViewerPart->Repaint();
      if ( pViewerHost )   pViewerHost->Repaint();
      if ( pViewerDomain ) pViewerDomain->Repaint();
    }
  }

  //! Cleans up all standard viewers.
  //! \param[in] repaint indicates whether to repaint the corresponding widgets.
  void ClearViewers(const bool repaint)
  {
    ClearViewers(this->ViewerPart, this->ViewerDomain, this->ViewerHost, repaint);
  }

  //! Return the default directory to be used in file prompt dialogs.
  //! \return requested directory.
  QString GetDefaultDirectory() const
  {
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  }

public:

  QMainWindow*                MainWindow;    //!< Main window.
  asiUI_ObjectBrowser*        ObjectBrowser; //!< Object browser.
  asiUI_ViewerPart*           ViewerPart;    //!< Viewer for part.
  asiUI_ViewerDomain*         ViewerDomain;  //!< Viewer for face parametric domain.
  asiUI_ViewerHost*           ViewerHost;    //!< Viewer for host geometry.
  asiUI_UnitManager*          UnitManager;   //!< Utility to manage units.
  ActAPI_ProgressEntry        Progress;      //!< Progress entry.
  ActAPI_PlotterEntry         Plotter;       //!< Imperative plotter.
  Handle(asiUI_WidgetFactory) WidgetFactory; //!< Widget factory for datums.

};

#endif
