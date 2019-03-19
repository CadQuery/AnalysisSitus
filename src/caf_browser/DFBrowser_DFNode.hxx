//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 OPEN CASCADE SAS
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_DFNode_h
#define _DFBrowser_DFNode_h

#include <Handle_DFBrowser_DFNode.hxx>
#include <Standard_Transient.hxx>

#include <Standard_CString.hxx>
#include <TCollection_AsciiString.hxx>
#include <DFBrowser_Colors.hxx>
#include <DFBrowser_Picture.hxx>
#include <DFBrowser_NodeType.hxx>
#include <Handle_DFBrowser_DFTree.hxx>

class DFBrowser_DFNode : public Standard_Transient
{
 public:

  Standard_EXPORT DFBrowser_DFNode();

  Standard_EXPORT virtual DFBrowser_NodeType GetType() const = 0;

  Standard_EXPORT virtual void Update() = 0;

  Standard_EXPORT void Next(const Handle(DFBrowser_DFNode)& theNext);

  inline const Handle(DFBrowser_DFNode) & Next() const
  {
    return myNext;
  }

  Standard_EXPORT void Parent(const Handle(DFBrowser_DFNode)& theParent);

  inline const Handle(DFBrowser_DFNode) & Parent() const
  {
    return myParent;
  }

  Standard_EXPORT virtual void AddSub(Handle(DFBrowser_DFNode)& theNode) = 0;

  Standard_EXPORT virtual Handle(DFBrowser_DFNode) Sub() const = 0;

  Standard_EXPORT virtual const TCollection_AsciiString & Name() = 0;

  inline const Handle(DFBrowser_DFTree) & Tree() const
  {
    return myTree;
  }

  inline void Tree(const Handle(DFBrowser_DFTree)& theTree)
  {
    myTree = theTree;
  }

  inline Standard_Boolean Opened() const
  {
    return myIsOpened;
  }

  Standard_EXPORT void Opened(const Standard_Boolean theOpened);

  inline Standard_Boolean CanOpen() const
  {
    return myCanOpen;
  }

  Standard_EXPORT void CanOpen(const Standard_Boolean theCanOpen);

  inline Standard_Boolean Selected() const
  {
    return myIsSelected;
  }

  inline void Selected(const Standard_Boolean theIsSelected)
  {
    myIsSelected = theIsSelected;
  }

  inline Standard_Boolean Changed() const
  {
    return myIsChanged;
  }

  inline void Changed(const Standard_Boolean theIsChanged)
  {
    myIsChanged = theIsChanged;
  }

  inline Standard_Boolean Visible() const
  {
    return myIsVisible;
  }

  Standard_EXPORT void Visible(const Standard_Boolean theIsVisible);

  inline DFBrowser_Colors Color() const
  {
    return myColor;
  }

  inline DFBrowser_Picture Pixmap() const
  {
    return myPixmap;
  }

  Standard_EXPORT virtual void Del() = 0;

  DEFINE_STANDARD_RTTI_INLINE(DFBrowser_DFNode, Standard_Transient)

 protected:

  TCollection_AsciiString myName;
  DFBrowser_Colors myColor;
  DFBrowser_Picture myPixmap;

 private:

  Handle(DFBrowser_DFTree) myTree;
  Handle(DFBrowser_DFNode) myNext;
  Handle(DFBrowser_DFNode) myParent;
  Standard_Boolean myIsOpened;
  Standard_Boolean myCanOpen;
  Standard_Boolean myIsSelected;
  Standard_Boolean myIsChanged;
  Standard_Boolean myIsVisible;
};

#endif
