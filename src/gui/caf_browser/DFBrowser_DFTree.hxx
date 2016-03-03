//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_DFTree_HeaderFile
#define _DFBrowser_DFTree_HeaderFile

// Analysis Situs includes
#include <analysis_situs.h>

#include <Handle_DFBrowser_DFTree.hxx>
#include <MMgt_TShared.hxx>

#include <TCollection_AsciiString.hxx>
#include <DFBrowser_PtrGUI.hxx>
#include <DFBrowser_Attr.hxx>
#include <DFBrowser_AttrNode.hxx>
#include <DFBrowser_LabelNode.hxx>
#include <TDocStd_Document.hxx>

class TDF_Label;

class DFBrowser_DFTree : public MMgt_TShared
{
 public:

  ASitus_EXPORT
    DFBrowser_DFTree(const Handle(TDocStd_Document)& theDoc);

  ASitus_EXPORT
    DFBrowser_DFTree();

  ASitus_EXPORT
    Handle(DFBrowser_AttrNode) Node(const Handle(TDF_Attribute)& theAttr) const;

  ASitus_EXPORT
    Handle(DFBrowser_LabelNode) Node(const TDF_Label& theLabel) const;

public:

  inline const Handle(DFBrowser_Attr) & Attr() const
  {
    return myAttr;
  }

  inline const Handle_DFBrowser_LabelNode & Root() const
  {
    return myRoot;
  }

  Standard_EXPORT static Standard_Integer NbBrowsers();

  Standard_EXPORT void Update();

  Standard_EXPORT void Close();

  Standard_EXPORT void AddDocName(TCollection_AsciiString& theName) const;

  DEFINE_STANDARD_RTTI_INLINE(DFBrowser_DFTree, MMgt_TShared)

 private: 

  Handle(TDocStd_Document) myDoc;
  Handle(DFBrowser_LabelNode) myRoot;
  Handle(DFBrowser_Attr) myAttr;
  DFBrowser_PtrGUI myGUI;
};

#endif
