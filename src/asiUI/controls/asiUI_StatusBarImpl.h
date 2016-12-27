//-----------------------------------------------------------------------------
// Created on: 27 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_StatusBarImpl_h
#define asiUI_StatusBarImpl_h

// asiUI includes
#include <asiUI_IStatusBar.h>

// Qt includes
#pragma warning(push, 0)
#include <QPointer>
#pragma warning(pop)

class asiUI_StatusBar;

//! Implementation of status bar interface for Analysis Situs.
class asiUI_StatusBarImpl : public asiUI_IStatusBar
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_StatusBarImpl, asiUI_IStatusBar)

public:

  asiUI_EXPORT
    asiUI_StatusBarImpl(asiUI_StatusBar* statusBar);

public:

  asiUI_EXPORT virtual void
    SetStatusText(const TCollection_AsciiString& statusText);

  asiUI_EXPORT virtual void
    ShowProgress(const bool isShown);

  asiUI_EXPORT virtual void
    UpdateProgress(const int percentage);

  asiUI_EXPORT virtual void
    SetProgressText(const TCollection_AsciiString& progressText);

  asiUI_EXPORT virtual void
    SetProgressInfinite(const bool isInfinite);

  asiUI_EXPORT virtual void
    SetInfoMessage(const TCollection_AsciiString& textMsg);

  asiUI_EXPORT asiUI_StatusBar*
    GetStatusBar() const;

private:

  //! Status bar widget.
  QPointer<asiUI_StatusBar> m_pStatusBar;

};

#endif
