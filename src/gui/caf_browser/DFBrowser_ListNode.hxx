//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_ListNode_h
#define _DFBrowser_ListNode_h

#include <Handle_DFBrowser_ListNode.hxx>
#include <DFBrowser_DFNode.hxx>

#include <Standard_CString.hxx>
#include <Handle_DFBrowser_AttrNode.hxx>
#include <DFBrowser_NodeType.hxx>

class DFBrowser_ListNode : public DFBrowser_DFNode
{
 public:

  Standard_EXPORT DFBrowser_ListNode();

  Standard_EXPORT virtual DFBrowser_NodeType GetType() const;

  Standard_EXPORT virtual void Update();

  Standard_EXPORT virtual const TCollection_AsciiString & Name();

  Standard_EXPORT virtual void AddSub(Handle(DFBrowser_DFNode)& theNode);

  Standard_EXPORT virtual Handle(DFBrowser_DFNode) Sub() const;

  inline const Handle(DFBrowser_AttrNode) & FirstAttribute() const
  {
    return myAttr;
  }

  inline void FirstAttribute(const Handle(DFBrowser_AttrNode)& theAttribute)
  {
    myAttr = theAttribute;
  }

  Standard_EXPORT Handle(DFBrowser_AttrNode) LastAttribute() const;

  Standard_EXPORT virtual void Del();

  DEFINE_STANDARD_RTTI_INLINE(DFBrowser_ListNode, DFBrowser_DFNode)

 private: 

  Handle_DFBrowser_AttrNode myAttr;
};

#endif
