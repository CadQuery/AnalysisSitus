//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#include <DFBrowser_NSNode.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Iterator.hxx>
#include <DFBrowser_DFNode.hxx>
#include <DFBrowser_DFTree.hxx>
#include <DFBrowser_Attr.hxx>
#include <DFBrowser_LabelNode.hxx>

DFBrowser_NSNode::DFBrowser_NSNode ()
{
  myPixmap = DFBrowser_NS_ATTRIBUTE;
  myIsShape = Standard_False;
  myStructure = Standard_False;
}

void DFBrowser_NSNode::AddSub (Handle(DFBrowser_DFNode)& theNode)
{
  theNode->Parent(this);
  theNode->Next(myFirst);
  Handle(DFBrowser_AttrNode)::DownCast(theNode)->Attribute(Attribute());
  myFirst = theNode;
}

Handle(DFBrowser_DFNode) DFBrowser_NSNode::Sub () const
{
  return myFirst;
}

void DFBrowser_NSNode::Update ()
{
  if (myIsShape) {
    myPixmap = DFBrowser_NODE;
    Changed(Standard_True);
    return;
  }
  if (!Attribute().IsNull()) DFBrowser_AttrNode::Update();
      myPixmap = DFBrowser_NS_ATTRIBUTE;
  if (myStructure) {
    while(!myFirst.IsNull()) { // remove old structure
      myFirst->Del();
      myFirst = myFirst->Next();
    }
    CanOpen(Standard_True);

    TopTools_MapOfShape aShapes;
    Standard_Boolean aWasNull = Standard_False;
    TNaming_Iterator anIter(NamedShape());
    for(;anIter.More();anIter.Next()) {
      TopoDS_Shape aShape = anIter.NewShape();
      if (aShape.IsNull()) {
	if (aWasNull) continue;
	aWasNull = Standard_True;
      } else if (aShapes.Contains(aShape)) continue; else aShapes.Add(aShape);
      Handle(DFBrowser_NSNode) aNewNode = new DFBrowser_NSNode;
      AddSub(aNewNode);
      aNewNode->Tree(Tree());
      aNewNode->SetShape(anIter.NewShape(),anIter.OldShape(),Standard_False,Standard_True);
    }
  } else {
    Opened(Standard_False);
    CanOpen(Standard_False);
  }
  Changed(Standard_True);
}

Handle(TNaming_NamedShape) DFBrowser_NSNode::NamedShape () const
{
  return Handle(TNaming_NamedShape)::DownCast(Attribute());
}

void DFBrowser_NSNode::NamedShape (const Handle(TNaming_NamedShape)& theNS)
{
  Attribute(theNS);
}

void DFBrowser_NSNode::Del ()
{
  while(!myFirst.IsNull()) {
    myFirst->Del();
    myFirst = myFirst->Next();
  }
  DFBrowser_AttrNode::Del();
}

void DFBrowser_NSNode::SetShape (const TopoDS_Shape& theNewShape,
                                 const TopoDS_Shape& theOldShape,
                                 const Standard_Boolean theVertices,
                                 const Standard_Boolean theNew)
{
  myIsShape = Standard_True;
  if (!theVertices) myShape = theNew?theNewShape:theOldShape;
  myPixmap = DFBrowser_NODE;
  if (theVertices) {
    myName = Tree()->Attr()->LoadVertices(theNew? theNewShape : theOldShape);
  }
  else {
    CanOpen(Standard_True);
    Opened(Standard_True);
	myName = Tree()->Attr()->LoadShape((theNew? theNewShape : theOldShape),Tree()->Root()->Label());
    if (theNew) {
      Handle(DFBrowser_NSNode) anOld = new DFBrowser_NSNode();
      AddSub(anOld);
      anOld->Tree(Tree());
      anOld->SetShape(theNewShape,theOldShape,Standard_False,Standard_False);
    }
    Handle(DFBrowser_NSNode) aVertices = new DFBrowser_NSNode();
    AddSub(aVertices);
    aVertices->Tree(Tree());
    aVertices->SetShape(theNewShape,theOldShape,Standard_True,theNew);
  }
}
