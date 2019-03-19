//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 OPEN CASCADE SAS
//-----------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS

#include <DFBrowser_GUI.hxx>

#include <stdio.h>
#include <TNaming_NamedShape.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TDF_Label.hxx>
#include <BRepTools.hxx>
#include <TPrsStd_AISViewer.hxx>

#include <DFBrowser_DFTree.hxx>
#include <DFBrowser_DFNode.hxx>
#include <DFBrowser_Attr.hxx>
#include <DFBrowser_AttrNode.hxx>
#include <DFBrowser_LabelNode.hxx>
#include <DFBrowser_NSNode.hxx>

#include <DFBrowser_PtrQTGUI.hxx>
#include <DFBrowser_QTGUI.hxx>

DFBrowser_GUI::DFBrowser_GUI (const Handle(DFBrowser_DFTree)& theTree)
: myTree(theTree)
{
  myQTGUI = new DFBrowser_QTGUI(this);
}

void DFBrowser_GUI::DoubleClicked (const Handle(DFBrowser_DFNode)& theNode) const
{
  if (theNode->GetType() == DFBrowser_ATTRIBUTE) {
    Handle(TDF_Attribute) attr = (Handle(DFBrowser_AttrNode)::DownCast(theNode))->Attribute();
    if (attr.IsNull()) return;
    if (attr->ID() == TNaming_NamedShape::GetID()) { // named shape display
      if ((Handle(DFBrowser_NSNode)::DownCast(theNode))->GetShape().IsNull()) {
	Handle(TNaming_NamedShape) aNamedShape = Handle(TNaming_NamedShape)::DownCast(attr);
	if (!aNamedShape.IsNull() && !aNamedShape->IsEmpty()) {
	  Handle(TPrsStd_AISPresentation) aPresentation;
	  if (!aNamedShape->FindAttribute(TPrsStd_AISPresentation::GetID(),aPresentation)) {
	    aPresentation = TPrsStd_AISPresentation::Set(aNamedShape);
	    aPresentation->Display();
	    aPresentation->SetColor(Quantity_NOC_MAGENTA1);
	    theNode->Parent()->Update();
	  } else {
	    aPresentation->Erase();
	    aPresentation->Unset(aPresentation->Label());
	    theNode->Parent()->Update();
	  }
	}
	// repaint viewer
	TPrsStd_AISViewer::Update(aNamedShape->Label());
      } else { // save TopoDS_Shape
	BRepTools::Write((Handle(DFBrowser_NSNode)::DownCast(theNode))->GetShape(),"db_shape.brep");
	cout<<"write to the file db_shape.brep"<<endl;
      }
    } else if (attr->ID() == TPrsStd_AISPresentation::GetID()) {
      Handle(TPrsStd_AISPresentation) aPresentation = Handle(TPrsStd_AISPresentation)::DownCast(attr);
      if (!aPresentation.IsNull()) {
	if (aPresentation->IsDisplayed()) {
	  aPresentation->Erase();
	} else {
	  aPresentation->Display();
	  aPresentation->SetColor(Quantity_NOC_MAGENTA1);
	}
	// repaint viewer
	TPrsStd_AISViewer::Update(aPresentation->Label());
      }
      theNode->Update();
    }
  } else if (theNode->GetType() == DFBrowser_LABEL || theNode->GetType() == DFBrowser_LIST) {
    Standard_Boolean aBack = Standard_False;
    theNode->Opened(Standard_True);
    Handle(DFBrowser_DFNode) aNode = theNode->Sub();
    while(!aNode.IsNull() && aNode!=theNode) {
      if (aNode->CanOpen()) aNode->Opened(Standard_True);
      if (!aNode->Sub().IsNull() && !aBack) aNode = aNode->Sub();
      else if (!aNode->Next().IsNull()) {
	aNode = aNode->Next();
	aBack = Standard_False;
      } else {
	aNode = aNode->Parent();
	aBack = Standard_True;
      }
    }
  }
  Update();
}

void DFBrowser_GUI::Selected (const Handle(DFBrowser_DFNode)& theNode)
{
  TCollection_AsciiString aName;
  if (theNode->GetType() == DFBrowser_ATTRIBUTE)
  {
    Handle(TDF_Attribute) attr=Handle(DFBrowser_AttrNode)::DownCast(theNode)->Attribute();
    if (!attr.IsNull()) {
      aName = "Attribute ";
	  aName += myTree->Attr()->LoadAttribute(attr, Standard_False);
      char strptr[256];
      sprintf(strptr, " 0x%p\n", (void*)&(*(attr)));
      aName += strptr;
      myTree->Attr()->Description(attr,aName);
    }
  }
  else if (theNode->GetType() == DFBrowser_LABEL)
  {
    TDF_Label aLabel = Handle(DFBrowser_LabelNode)::DownCast(theNode)->Label();
    if (!aLabel.IsNull()) {
      aName.Clear();
      myTree->Attr()->Description(aLabel,aName);
    }
  }
  ((DFBrowser_PtrQTGUI)myQTGUI)->Description(aName);
}

const Handle(DFBrowser_DFNode) & DFBrowser_GUI::First () const
{
  return myTree->Root();
}

Handle(DFBrowser_DFNode) DFBrowser_GUI::NextVisible (const Handle(DFBrowser_DFNode)& theNode) const
{
  Handle(DFBrowser_DFNode) aRes;
  if (!theNode->Sub().IsNull() && theNode->Opened()) aRes = theNode->Sub();
  else if (!theNode->Next().IsNull()) aRes = theNode->Next();
  else {
    aRes = theNode->Parent();
    while (!aRes.IsNull())
      if (!aRes->Next().IsNull()) {
	aRes = aRes->Next(); 
	break;
      } else aRes = aRes->Parent();
  }
  if (aRes.IsNull()) return aRes;
  if (aRes->Visible()) return aRes;
  return NextVisible(aRes);
}

Handle(DFBrowser_DFNode) DFBrowser_GUI::Next (const Handle(DFBrowser_DFNode)& theNode) const
{
  if (!theNode->Sub().IsNull()) return theNode->Sub();
  if (!theNode->Next().IsNull()) return theNode->Next();
  Handle(DFBrowser_DFNode) aRes = theNode->Parent();
  while (!aRes.IsNull()) if (!aRes->Next().IsNull()) return aRes->Next(); else aRes = aRes->Parent();
  return aRes;
}

Standard_Integer DFBrowser_GUI::NodeDepth (const Handle(DFBrowser_DFNode)& theNode) const
{
  Standard_Integer aDepth = 0;
  Handle(DFBrowser_DFNode) aRes = theNode;
  while (!aRes.IsNull()) {
    aDepth++;
    aRes = aRes->Parent();
  }
  return aDepth-1;
}

void DFBrowser_GUI::Update () const
{
  ((DFBrowser_PtrQTGUI)myQTGUI)->Update();
}

Standard_Address DFBrowser_GUI::Pixmap (const DFBrowser_Picture theID) const
{
  return (Standard_Address)(((DFBrowser_PtrQTGUI)myQTGUI)->Pixmap(theID));
}

void DFBrowser_GUI::SetLevel (const DFBrowser_Level theLevel)
{
  Handle(DFBrowser_DFNode) aNode = myTree->Root();
  while(!aNode.IsNull()) {
    aNode->Opened(theLevel == DFBrowser_SHOWALL && aNode->CanOpen());
    aNode = Next(aNode);
  }
  TDF_Label aLabel;
  switch (theLevel) {
  case DFBrowser_SHOWDATA:
    myTree->Root()->Opened(Standard_True);
    aLabel = myTree->Root()->Label();
    aLabel = aLabel.FindChild(1,Standard_False);
    if (!aLabel.IsNull()) {
      aNode = myTree->Node(aLabel);
      if (!aNode.IsNull()) aNode->Opened(Standard_True);
      aLabel = aLabel.FindChild(3,Standard_False);
      if (!aLabel.IsNull()) {
	aNode = myTree->Node(aLabel);
	if (!aNode.IsNull()) aNode->Opened(Standard_True);
	aLabel = aLabel.FindChild(2,Standard_False);
	if (!aLabel.IsNull()) {
	  aNode = myTree->Node(aLabel);
	  if (!aNode.IsNull()) aNode->Opened(Standard_True);
	}
      }
    }
    break;
  case DFBrowser_SHOWFIRST:
    myTree->Root()->Opened(Standard_True);
    for(aNode=myTree->Root()->FirstLabel();!aNode.IsNull();aNode=aNode->Next())
      if (aNode->CanOpen()) aNode->Opened(Standard_True);

  default: break; //SHOWALL <<== to be checked
  }
  Update();
}

void DFBrowser_GUI::SetAttrPrs (const DFBrowser_AttrPrs thePrs)
{
  Handle(DFBrowser_DFNode) aNode = myTree->Root();
  for(;!aNode.IsNull();aNode = Next(aNode)) {
    if (aNode->GetType() == DFBrowser_ATTRIBUTE) aNode->Visible(thePrs!=DFBrowser_VANISH);
    if (aNode->GetType() == DFBrowser_LABEL) {
      Handle(DFBrowser_LabelNode) aLab = Handle(DFBrowser_LabelNode)::DownCast(aNode);
      if (thePrs == DFBrowser_ASLIST && aLab->List().IsNull()) aLab->AddList(Standard_True);
      else if (thePrs != DFBrowser_ASLIST && !aLab->List().IsNull()) aLab->AddList(Standard_False);
    }
  }
  const TCollection_AsciiString &aFilter = ((DFBrowser_PtrQTGUI)myQTGUI)->Filter();
  const Handle(DFBrowser_DFNode) &aFirst = myTree->Root();
  if (thePrs!=DFBrowser_VANISH)
    if (!aFilter.IsEmpty())
      DoFilter(aFirst);
  Update();
}

Standard_Boolean DFBrowser_GUI::DoFilter (const Handle(DFBrowser_DFNode)& theFrom)
{
  Standard_Boolean aRes = Standard_False;
  Handle(DFBrowser_DFNode) aNode = theFrom->Sub();
  while(!aNode.IsNull()) {
    if (DoFilter(aNode)) { aRes = Standard_True; break; }
    aNode = aNode->Next();
  }
  if (!aRes) {
    const TCollection_AsciiString &aFilter = ((DFBrowser_PtrQTGUI)myQTGUI)->Filter();
    if (theFrom->Name().Search(aFilter) >= 0) aRes = Standard_True;
  }
  theFrom->Visible(aRes);
  return aRes;
}

void DFBrowser_GUI::ShowNS (const Standard_Boolean theShow)
{
  Handle(DFBrowser_DFNode) aNode;
  for(aNode=myTree->Root() ; !aNode.IsNull() ; aNode = Next(aNode)) {
    Handle(DFBrowser_NSNode) aNS = Handle(DFBrowser_NSNode)::DownCast(aNode);
    if (!aNS.IsNull()) {
      if ((aNS->Structure() ^ theShow) != 0) {
	aNS->Structure(theShow);
	aNS->Update();
      }
    } 
  }
  Update();
}

void DFBrowser_GUI::Close ()
{
  ((DFBrowser_PtrQTGUI)myQTGUI)->Close();
  myTree->Close();
}
