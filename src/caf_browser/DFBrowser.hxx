//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 OPEN CASCADE SAS
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_h
#define _DFBrowser_h

#include <TDocStd_Document.hxx>

class DFBrowser
{
 public:

  Standard_EXPORT static Standard_Integer DFBrowserCall(const Handle(TDocStd_Document)& theDoc);
  //! Loads DFBrowser functionality for not Draw based application. Used for plugin. <br>
  Standard_EXPORT static Standard_Integer Factory(const Handle(TDocStd_Document)& theDoc);
};



#endif
