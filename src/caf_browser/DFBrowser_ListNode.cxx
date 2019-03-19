//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 OPEN CASCADE SAS
//-----------------------------------------------------------------------------

#include <DFBrowser_ListNode.hxx>

#include <DFBrowser_AttrNode.hxx>
#include <DFBrowser_DFNode.hxx>
#include <DFBrowser_LabelNode.hxx>
#include <DFBrowser_DFTree.hxx>
#include <DFBrowser_Attr.hxx>

DFBrowser_ListNode::DFBrowser_ListNode ()
{
  myPixmap = DFBrowser_FILE;
  myColor = DFBrowser_BLACK;
  CanOpen(Standard_True);
}

DFBrowser_NodeType DFBrowser_ListNode::GetType () const
{
  return DFBrowser_LIST;
}

void DFBrowser_ListNode::Update ()
{
  Next(Handle(DFBrowser_LabelNode)::DownCast(Parent())->FirstLabel());
  Handle(DFBrowser_DFNode) aNode = Handle(DFBrowser_LabelNode)::DownCast(Parent())->Child();
  if (aNode.IsNull()) myAttr.Nullify();
  else if (aNode->GetType() == DFBrowser_LABEL) myAttr.Nullify();
  else if (aNode->GetType() == DFBrowser_ATTRIBUTE) myAttr = Handle(DFBrowser_AttrNode)::DownCast(aNode);
  aNode = LastAttribute();
  if (!aNode.IsNull()) aNode->Next(NULL);
  for(aNode = myAttr;!aNode.IsNull();aNode = aNode->Next()) aNode->Parent(this);
  Visible(!myAttr.IsNull());
  Changed(Standard_True);
}

const TCollection_AsciiString & DFBrowser_ListNode::Name ()
{
  if (myName.IsEmpty())
    myName = "Attribute List ";
  return myName;
}

void DFBrowser_ListNode::AddSub (Handle(DFBrowser_DFNode)& theNode)
{
  myAttr = Handle(DFBrowser_AttrNode)::DownCast(theNode);
}

Handle(DFBrowser_DFNode) DFBrowser_ListNode::Sub () const
{
  return myAttr;
}

Handle(DFBrowser_AttrNode) DFBrowser_ListNode::LastAttribute () const
{
  Handle(DFBrowser_DFNode) aNode = myAttr;
  while(!aNode.IsNull()) {
    if (aNode->Next().IsNull()) break;
    if (aNode->Next()->GetType() != DFBrowser_ATTRIBUTE) break;
    aNode = aNode->Next();
  }
  return Handle(DFBrowser_AttrNode)::DownCast(aNode);
}

void DFBrowser_ListNode::Del ()
{
  myAttr.Nullify();
  DFBrowser_DFNode::Del();
}
