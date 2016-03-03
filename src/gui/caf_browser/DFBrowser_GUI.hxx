//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_GUI_HeaderFile
#define _DFBrowser_GUI_HeaderFile

#include <Standard.hxx>

#include <Handle_DFBrowser_DFTree.hxx>
#include <Handle_DFBrowser_DFNode.hxx>
#include <DFBrowser_Picture.hxx>
#include <DFBrowser_Level.hxx>
#include <DFBrowser_AttrPrs.hxx>

class DFBrowser_DFTree;
class DFBrowser_DFNode;

class DFBrowser_GUI
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

  Standard_EXPORT DFBrowser_GUI(const Handle(DFBrowser_DFTree)& theTree);

  Standard_EXPORT void Selected(const Handle(DFBrowser_DFNode)& theNode);

  Standard_EXPORT void DoubleClicked(const Handle(DFBrowser_DFNode)& theNode) const;

  Standard_EXPORT const Handle_DFBrowser_DFNode & First() const;

  Standard_EXPORT Handle_DFBrowser_DFNode Next(const Handle(DFBrowser_DFNode)& theNode) const;

  Standard_EXPORT Handle_DFBrowser_DFNode NextVisible(const Handle(DFBrowser_DFNode)& theNode) const;

  Standard_EXPORT Standard_Integer NodeDepth(const Handle(DFBrowser_DFNode)& theNode) const;

  Standard_EXPORT void Update() const;

  Standard_EXPORT Standard_Address Pixmap(const DFBrowser_Picture theID) const;

  inline const Handle_DFBrowser_DFTree & Tree() const
  {
    return myTree;
  }

  Standard_EXPORT void SetLevel(const DFBrowser_Level theLevel);

  Standard_EXPORT void SetAttrPrs(const DFBrowser_AttrPrs thePrs);

  Standard_EXPORT Standard_Boolean DoFilter(const Handle(DFBrowser_DFNode)& theFrom);

  Standard_EXPORT void ShowNS(const Standard_Boolean theShow);

  Standard_EXPORT void Close();

 private:

  Handle_DFBrowser_DFTree myTree;
  Standard_Address myFLGUI;
  Standard_Address myQTGUI;
};

#endif
