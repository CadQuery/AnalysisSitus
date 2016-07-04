//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_QTGUI_h
#define _DFBrowser_QTGUI_h

#include <Standard.hxx>

#include <Standard_Address.hxx>
#include <TCollection_AsciiString.hxx>
#include <DFBrowser_PtrGUI.hxx>
#include <DFBrowser_Picture.hxx>

class DFBrowser_QTGUI
{
 public:

  void* operator new(size_t,void* anAddress) 
  {
    return anAddress;
  }
  void* operator new(size_t size) 
  {
    return Standard::Allocate(size); 
  }
  void  operator delete(void *anAddress) 
  {
    if (anAddress) Standard::Free((Standard_Address&)anAddress); 
  }

  Standard_EXPORT DFBrowser_QTGUI(const DFBrowser_PtrGUI& theGUI);

  Standard_EXPORT void Update();

  const TCollection_AsciiString & Filter() const;

  Standard_EXPORT void Close();

  Standard_EXPORT void Description(const TCollection_AsciiString &theDescription);

  Standard_EXPORT const TCollection_AsciiString & Description() const;

  Standard_EXPORT static Standard_Address Pixmap(const DFBrowser_Picture thePicture);
};

#endif
