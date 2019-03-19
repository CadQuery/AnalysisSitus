//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 OPEN CASCADE SAS
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_AttrNode_h
#define _DFBrowser_AttrNode_h

#include <Handle_DFBrowser_AttrNode.hxx>
#include <DFBrowser_DFNode.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_CString.hxx>

class DFBrowser_AttrNode : public DFBrowser_DFNode
{
 public:

  Standard_EXPORT DFBrowser_AttrNode();

  Standard_EXPORT virtual DFBrowser_NodeType GetType() const;

  Standard_EXPORT virtual void AddSub(Handle(DFBrowser_DFNode)& theNode);

  Standard_EXPORT virtual Handle(DFBrowser_DFNode) Sub() const;

  Standard_EXPORT virtual const TCollection_AsciiString & Name();

  Standard_EXPORT virtual void Update();

  inline const Handle(TDF_Attribute)& Attribute() const
  {
    return myAttr;
  }

  inline void Attribute(const Handle(TDF_Attribute)& theAttr)
  {
    myAttr = theAttr;
  }

  Standard_EXPORT virtual void Del();

  DEFINE_STANDARD_RTTI_INLINE(DFBrowser_AttrNode, DFBrowser_DFNode)

 private:

  Handle(TDF_Attribute) myAttr;
};

#endif
