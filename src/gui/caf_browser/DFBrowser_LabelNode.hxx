//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_LabelNode_h
#define _DFBrowser_LabelNode_h

#include <Handle_DFBrowser_LabelNode.hxx>
#include <DFBrowser_DFNode.hxx>

#include <Standard_CString.hxx>
#include <TDF_Label.hxx>
#include <Handle_DFBrowser_ListNode.hxx>
#include <DFBrowser_NodeType.hxx>

class DFBrowser_LabelNode : public DFBrowser_DFNode
{
 public:

  Standard_EXPORT DFBrowser_LabelNode();

  Standard_EXPORT virtual DFBrowser_NodeType GetType() const;

  Standard_EXPORT virtual void AddSub(Handle(DFBrowser_DFNode)& theNode);

  Standard_EXPORT virtual Handle(DFBrowser_DFNode) Sub() const;

  Standard_EXPORT void DelSub(Handle(DFBrowser_DFNode)& theNode);

  Standard_EXPORT virtual void Update();

  Standard_EXPORT virtual const TCollection_AsciiString & Name();

  inline const TDF_Label & Label() const
  {
    return myLabel;
  }

  inline void Label(const TDF_Label& theLabel)
  {
    myLabel = theLabel;
  }

  inline const Handle(DFBrowser_DFNode) & Child() const
  {
    return myChild;
  }

  inline const Handle(DFBrowser_LabelNode) & FirstLabel() const
  {
    return myFirstLabel;
  }

  Standard_EXPORT void AddList(const Standard_Boolean theAdd);

  inline const Handle(DFBrowser_ListNode) & List() const
  {
    return myList;
  }

  Standard_EXPORT virtual void Del();

  DEFINE_STANDARD_RTTI_INLINE(DFBrowser_LabelNode, DFBrowser_DFNode)

 private: 

  TDF_Label myLabel;
  Handle(DFBrowser_DFNode)    myChild;
  Handle(DFBrowser_LabelNode) myFirstLabel;
  Handle(DFBrowser_ListNode)  myList;
};

#endif
