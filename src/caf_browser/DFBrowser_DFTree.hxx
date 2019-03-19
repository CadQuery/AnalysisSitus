//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 OPEN CASCADE SAS
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_DFTree_h
#define _DFBrowser_DFTree_h

// CAF Browser includes
#include <caf_browser.h>

#include <Handle_DFBrowser_DFTree.hxx>
#include <Standard_Transient.hxx>

#include <TCollection_AsciiString.hxx>
#include <DFBrowser_PtrGUI.hxx>
#include <DFBrowser_Attr.hxx>
#include <DFBrowser_AttrNode.hxx>
#include <DFBrowser_LabelNode.hxx>
#include <TDocStd_Document.hxx>

class TDF_Label;

class DFBrowser_DFTree : public Standard_Transient
{
 public:

  CAFBrowser_EXPORT
    DFBrowser_DFTree(const Handle(TDocStd_Document)& theDoc);

  CAFBrowser_EXPORT
    DFBrowser_DFTree();

  CAFBrowser_EXPORT
    Handle(DFBrowser_AttrNode) Node(const Handle(TDF_Attribute)& theAttr) const;

  CAFBrowser_EXPORT
    Handle(DFBrowser_LabelNode) Node(const TDF_Label& theLabel) const;

public:

  inline const Handle(DFBrowser_Attr) & Attr() const
  {
    return myAttr;
  }

  inline const Handle(DFBrowser_LabelNode) & Root() const
  {
    return myRoot;
  }

  Standard_EXPORT static Standard_Integer NbBrowsers();

  Standard_EXPORT void Update();

  Standard_EXPORT void Close();

  Standard_EXPORT void AddDocName(TCollection_AsciiString& theName) const;

  DEFINE_STANDARD_RTTI_INLINE(DFBrowser_DFTree, Standard_Transient)

 private: 

  Handle(TDocStd_Document) myDoc;
  Handle(DFBrowser_LabelNode) myRoot;
  Handle(DFBrowser_Attr) myAttr;
  DFBrowser_PtrGUI myGUI;
};

#endif
