//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 OPEN CASCADE SAS
//-----------------------------------------------------------------------------

#include <DFBrowser_LabelNode.hxx>

#include <TDF_LabelMap.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <Standard_GUID.hxx>

#include <DFBrowser_DFNode.hxx>
#include <DFBrowser_LabelNode.hxx>
#include <DFBrowser_ListNode.hxx>
#include <DFBrowser_DFTree.hxx>
#include <DFBrowser_Attr.hxx>
#include <DFBrowser_AttrNode.hxx>
#include <DFBrowser_NSNode.hxx>

DFBrowser_LabelNode::DFBrowser_LabelNode ()
{
  myPixmap = DFBrowser_NONE;
  CanOpen(Standard_True);
}

DFBrowser_NodeType DFBrowser_LabelNode::GetType () const
{
  return DFBrowser_LABEL;
}

void DFBrowser_LabelNode::AddSub (Handle(DFBrowser_DFNode)& theNode)
{
  theNode->Parent(Handle(DFBrowser_LabelNode)(this));
  if (theNode->GetType()!=DFBrowser_LABEL) {
    theNode->Next(myChild);
    myChild = theNode;
  } else {
    if (myChild.IsNull()) {
      myChild = theNode;
      myFirstLabel = Handle(DFBrowser_LabelNode)::DownCast(theNode);
    } else {
      Handle(DFBrowser_DFNode) aNode = myChild;
      Handle(DFBrowser_DFNode) aPrev;
      while(!aNode.IsNull()) {
	if (aNode->GetType() == DFBrowser_LABEL) {
	  if (Handle(DFBrowser_LabelNode)::DownCast(aNode)->Label().Tag() > 
	      Handle(DFBrowser_LabelNode)::DownCast(theNode)->Label().Tag()) break;
	}
	aPrev = aNode;
	aNode = aNode->Next();
      }
      if (!aPrev.IsNull()) {
	theNode->Next(aPrev->Next());
	aPrev->Next(theNode);
      } else myFirstLabel = Handle(DFBrowser_LabelNode)::DownCast(theNode);
      theNode->Next(aNode);
    }
  }
  theNode->Update();
  if (!myList.IsNull()) myList->Update();
}

Handle(DFBrowser_DFNode) DFBrowser_LabelNode::Sub () const
{
  if (!myList.IsNull()) return myList;
  return myChild;
}

void DFBrowser_LabelNode::DelSub (Handle(DFBrowser_DFNode)& theNode)
{
  Handle(DFBrowser_DFNode) aNode = myChild;
  Handle(DFBrowser_DFNode) aPrev;
  Standard_Boolean aMore = Standard_True;
  while(!aNode.IsNull()) {
    if (aNode == theNode) break;
    aPrev = aNode;
    aNode = aNode->Next();
    if (aNode.IsNull() && !myList.IsNull() && aMore) {
      aNode = myFirstLabel;
      aMore = Standard_False;
    }
  }
  if (aNode.IsNull()) {
     cout<<"Error: DFBrowser_LabelNode::DelSub for incorrect node"<<endl;
  } else {
    if (!aPrev.IsNull()) aPrev->Next(theNode->Next()); else myChild = theNode->Next();
    if (myFirstLabel == theNode) myFirstLabel = Handle(DFBrowser_LabelNode)::DownCast(theNode->Next());
//     theNode->Next(NULL);
//     theNode->Parent(NULL);
    theNode->Del();
  }
}

void DFBrowser_LabelNode::Update ()
{
  myName.Clear();
  Changed(Standard_True);
  Handle(DFBrowser_DFNode) aChild = myChild,aNext;
  TDF_LabelMap aSubLabels;
  TDF_AttributeMap aSubAttributes;
  Standard_Boolean aMore = Standard_True;;
  while(!aChild.IsNull()) {
    aNext = aChild->Next();
    if (aChild->GetType() == DFBrowser_LABEL) {
      TDF_Label aLabel = Handle(DFBrowser_LabelNode)::DownCast(aChild)->Label();
      if (aLabel.IsNull()) DelSub(aChild); else {
	aSubLabels.Add(aLabel);
	aChild->Update();
      }
    } else {
      Handle(TDF_Attribute) anAttr = Handle(DFBrowser_AttrNode)::DownCast(aChild)->Attribute();
      if (anAttr->Label().IsNull()) DelSub(aChild); 
	  else {
	    aSubAttributes.Add(anAttr);
	    aChild->Update();
      }
    }
    aChild = aNext;
    if (aChild.IsNull() && !myList.IsNull() && aMore) {
      aChild = myFirstLabel;
      aMore = Standard_False;
    }
  }

  TDF_ChildIterator anIter1(myLabel);
  for(;anIter1.More();anIter1.Next()) {
    if (!aSubLabels.Contains(anIter1.Value()) && !anIter1.Value().IsNull()) {
//       cout<<"DFBrowser_LabelNode::Update add child"<<endl;
      Handle(DFBrowser_LabelNode) aNewNode = new DFBrowser_LabelNode();
      aNewNode->Label(anIter1.Value());
      aNewNode->Tree(Tree());
      AddSub(aNewNode);
    }
  }
  TDF_AttributeIterator anIter2(myLabel);
  for(;anIter2.More();anIter2.Next()) {
	  Handle(TDF_Attribute) pValue = anIter2.Value();
	if(pValue.IsNull()) {
#ifdef DEB
		cout << "NULL ATTR" <<endl; //
#endif
		continue;}
	if (pValue->IsValid() & !aSubAttributes.Contains(pValue)) {
//        cout<<"DFBrowser_LabelNode::Update add attribute"<<endl;
      Handle(DFBrowser_AttrNode) aNewNode;
       if (anIter2.Value()->ID() == TNaming_NamedShape::GetID()) aNewNode = new DFBrowser_NSNode();
       else aNewNode = new DFBrowser_AttrNode();
       aNewNode->Attribute(anIter2.Value());
       aNewNode->Tree(Tree());
       AddSub(aNewNode);
    }
  }
  myPixmap = (myChild.IsNull())? DFBrowser_CLOSE : DFBrowser_FOLDER;
  CanOpen(!myChild.IsNull());
  if (!myList.IsNull()) myList->Update();
}

const TCollection_AsciiString & DFBrowser_LabelNode::Name ()
{
  if (myName.IsEmpty())
  {
    myName = Tree()->Attr()->LoadLabel(myLabel);
    myColor = Tree()->Attr()->GetColor();
  }
  return myName;
}

void DFBrowser_LabelNode::AddList (const Standard_Boolean theAdd)
{
  if (theAdd) {
    myList = new DFBrowser_ListNode();
    myList->Parent(this);
    myList->Tree(Tree());
    myList->Update();
  } else {
    if (!myList->LastAttribute().IsNull()) myList->LastAttribute()->Next(myFirstLabel);
    myList->Del();
    Handle(DFBrowser_DFNode) aNode = myChild;
    while(!aNode.IsNull()) {
      aNode->Parent(this);
      aNode = aNode->Next();
    }
    myList.Nullify();
  }
}

void DFBrowser_LabelNode::Del ()
{
  Handle(DFBrowser_DFNode) aNode = myChild;
  Standard_Boolean aMore = Standard_True;
  if (!myList.IsNull()) myList->Del();
  while(!aNode.IsNull()) {
    myChild = aNode;
    aNode = aNode->Next();
    myChild->Del();
    if (aNode.IsNull() && !myList.IsNull() && aMore) {
      aNode = myFirstLabel;
      aMore = Standard_False;
    }
  }
  myChild.Nullify();
  myFirstLabel.Nullify();
  DFBrowser_DFNode::Del();
}
