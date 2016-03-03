//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------


#include <DFBrowser_Attr.hxx>

#include <TCollection_ExtendedString.hxx>
#include <TDF.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Reference.hxx>
#include <TDF_AttributeList.hxx>
#include <TDataStd.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Current.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_Variable.hxx>
#include <TDataStd_Expression.hxx>
#include <TDataStd_Relation.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Position.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_PatternStd.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TDocStd_Owner.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkRoot.hxx>
#include <TFunction_Function.hxx>
#include <TNaming.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs.hxx>
#include <gp_Pnt.hxx>
#include <BRep_Tool.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <stdio.h>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Quantity_Color.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_InteractiveContext.hxx>

//#define DBROWSER_DEBUG
#define MAX_STR_LENGTH 511 

#define DB_CONSTRAINT_TYPE 0
#define DB_NAMING_TYPE 1
#define DB_SHAPE_TYPE 2
#define DB_NS_TYPE 3
#define DB_GEOM_TYPE 4
#define DB_DIMENSION_TYPE 5
#define DB_MATERIAL_TYPE 6
#define DB_DISPLAY_MODE 7
#define DB_ORIENTATION_TYPE 8

DFBrowser_Attr::DFBrowser_Attr()
: myColor(DFBrowser_BLACK),
  myError(Standard_False),
  myBrackets(0),
  myDetailed(Standard_True)
{
}

const TCollection_AsciiString & DFBrowser_Attr::LoadLabel(const TDF_Label& theLabel, const Standard_Boolean theName)
{
  TDF_Tool::Entry(theLabel,myName); // given label path string representation

  Handle(TDataStd_Name) aName;
  if (theName && theLabel.FindAttribute(TDataStd_Name::GetID(),aName))
  {
    myColor = DFBrowser_GREEN;
    myName += " ";
    myName += TCollection_AsciiString(aName->Get(),'?');
  }
  else
    myColor = DFBrowser_BLACK;

  return myName;
}

const TCollection_AsciiString & DFBrowser_Attr::LoadAttribute(const Handle(TDF_Attribute)& theAttribute, const Standard_Boolean theFull)
{
  if (theFull) return LoadAttribute(theAttribute);

  myColor = DFBrowser_BLACK;
  myName = theAttribute->DynamicType()->Name();
  return myName;
}

const TCollection_AsciiString & DFBrowser_Attr::LoadAttribute(const Handle(TDF_Attribute)& theAttribute)
{
  myColor = DFBrowser_BLACK;
  myName = theAttribute->DynamicType()->Name();
  if (!myDetailed) return myName;

  AddToName(" ");
  TCollection_AsciiString aString;
  if (theAttribute->IsInstance(STANDARD_TYPE(TDataXtd_Constraint))) {
    Handle(TDataXtd_Constraint) data = Handle(TDataXtd_Constraint)::DownCast(theAttribute);
    AddToName(DB_CONSTRAINT_TYPE,data->GetType());
    if (data->IsDimension())
      AddList(TCollection_AsciiString(data->GetValue()->Get()).ToCString());
    for(Standard_Integer i=0;i<data->NbGeometries();i++) AddList(data->GetGeometry(i+1));
    AddList(NULL);
  } else if (theAttribute->IsInstance(STANDARD_TYPE(TDataStd_TreeNode))) {
    Handle(TDataStd_TreeNode) data = Handle(TDataStd_TreeNode)::DownCast(theAttribute);
    if(data->HasFather()) AddList(data->Father()); else AddList("NULL ");
    if(data->HasPrevious()) AddList(data->Previous()); else AddList("NULL ");
    if(data->HasNext()) AddList(data->Next()); else AddList("NULL ");
    if(data->HasFirst()) AddList(data->First()); else AddList("NULL ");
    AddList(NULL);
  } else if (theAttribute->IsInstance(STANDARD_TYPE(TDataStd_Name))) {
    AddToName("= ");
    AddToName(Handle(TDataStd_Name)::DownCast(theAttribute)->Get());
  } else if (theAttribute->IsInstance(STANDARD_TYPE(TDataStd_Comment))) {
    AddToName("= ");
    AddToName(Handle(TDataStd_Comment)::DownCast(theAttribute)->Get());
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDataStd_UAttribute)) || theAttribute->IsInstance(STANDARD_TYPE(TDataXtd_PatternStd))) {
    Standard_GUID gui=(theAttribute->IsInstance(STANDARD_TYPE(TDataStd_UAttribute)))
      ?Handle(TDataStd_UAttribute)::DownCast(theAttribute)->ID()
      :Handle(TDataXtd_PatternStd)::DownCast(theAttribute)->GetPatternID();
    char s[256];
    gui.ToCString(s);
    AddToName("= ");
    AddToName(s);
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDataStd_Integer))){
    aString = "= ";
    aString += Handle(TDataStd_Integer)::DownCast(theAttribute)->Get();
    AddToName(aString);
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDataStd_Real))){
    aString = "= ";
    aString += Handle(TDataStd_Real)::DownCast(theAttribute)->Get();
    AddToName(aString);
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDataStd_IntegerArray))) {
    Handle(TDataStd_IntegerArray) data = Handle(TDataStd_IntegerArray)::DownCast(theAttribute);
    aString = "= [ ";
    aString += data->Lower();
    aString += "..";
    aString += data->Upper();
    aString += " ]";
    AddToName(aString);
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDataStd_RealArray))){
    Handle(TDataStd_RealArray) data = Handle(TDataStd_RealArray)::DownCast(theAttribute);
    aString = "= [ ";
    aString += data->Lower();
    aString += "..";
    aString += data->Upper();
    aString += " ]";
    AddToName(aString);
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDataStd_Expression))){
    AddToName("= ");
    AddToName(Handle(TDataStd_Expression)::DownCast(theAttribute)->GetExpression());
  }else if(theAttribute->IsInstance(STANDARD_TYPE(TDataStd_Relation))){
    AddToName("= ");
    AddToName(Handle(TDataStd_Relation)::DownCast(theAttribute)->GetRelation());
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDataXtd_Position))){
    Handle(TDataXtd_Position) data = Handle(TDataXtd_Position)::DownCast(theAttribute);
    gp_Pnt p=data->GetPosition();
	aString = "= ( ";
    aString += p.X();
    aString += " , ";
    aString += p.Y();
    aString += " , ";
    aString += p.Z();
    aString += " )";
    AddToName(aString);
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TPrsStd_AISPresentation))){
    Handle(TPrsStd_AISPresentation) data = Handle(TPrsStd_AISPresentation)::DownCast(theAttribute);
    Standard_GUID gui=data->GetDriverGUID();
    char s[256];
    gui.ToCString(s);
    AddToName("= ");
    AddToName(s);
    TCollection_ExtendedString aName;
    Standard_Boolean res=TDF::ProgIDFromGUID(gui,aName);
    if(res){
      AddToName(aName);
      AddList(NULL);
    }
    if (!data->IsDisplayed()) myColor = DFBrowser_LIGHTGRAY;
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TPrsStd_AISViewer))){
    if (!(Handle(TPrsStd_AISViewer)::DownCast(theAttribute)->GetInteractiveContext()).IsNull())
      AddToName(" ( Attached )");
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDocStd_Owner))){
    Handle(TDocStd_Document) aD=Handle(TDocStd_Owner)::DownCast(theAttribute)->GetDocument();
    AddToName("= ");
    AddToName(aD->StorageFormat());
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDocStd_XLink))){
    AddToName(" = ( ");
    AddToName(Handle(TDocStd_XLink)::DownCast(theAttribute)->LabelEntry());
    AddToName(" , ");
    AddToName(Handle(TDocStd_XLink)::DownCast(theAttribute)->DocumentEntry());
    AddToName(" )");
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TNaming_NamedShape))){
    Handle(TNaming_NamedShape) data = Handle(TNaming_NamedShape)::DownCast(theAttribute);
    TopoDS_Shape aShape=data->Get();
    if(aShape.IsNull()) AddToName("Empty shape"); else {
      AddToName(DB_SHAPE_TYPE,aShape.ShapeType());
      if (aShape.Orientation() == TopAbs_FORWARD) myColor = DFBrowser_DARKGRAY;
      else if (aShape.Orientation() == TopAbs_REVERSED) myColor = DFBrowser_GRAY;
    }
    if (!data->IsEmpty()) {
      AddToName(" ");
      AddToName(DB_NS_TYPE,data->Evolution());
    }

    if (data->Evolution() != TNaming_PRIMITIVE && data->Evolution() != TNaming_SELECTED) {
      for(TNaming_Iterator anIterator(data);anIterator.More();anIterator.Next()) {
	if (!anIterator.OldShape().IsNull()) {
	  data = TNaming_Tool::NamedShape(anIterator.OldShape(),theAttribute->Label());
	  if (!data.IsNull()) AddList(data);
	  else AddList("SHAPE");
	} else AddList("NULL");
      }
      AddList(NULL);
    }
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TNaming_Naming))){
    Handle(TNaming_Naming) data = Handle(TNaming_Naming)::DownCast(theAttribute);
    AddToName("= ");
    AddToName(DB_NAMING_TYPE,data->GetName().Type());
    for(TNaming_ListIteratorOfListOfNamedShape anIter(data->GetName().Arguments());anIter.More();anIter.Next())
      AddList(anIter.Value());
    AddList(NULL);
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TFunction_Function))){
    Standard_GUID gui=Handle(TFunction_Function)::DownCast(theAttribute)->GetDriverGUID();
    char s[256];
    gui.ToCString(s);
    AddToName("= ");
    AddToName(s);
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDF_TagSource))){
    aString = "= ";
    aString += (int)(Handle(TDF_TagSource)::DownCast(theAttribute)->Get());
    AddToName(aString);
  } else if(theAttribute->IsInstance(STANDARD_TYPE(TDF_Reference))){
    TCollection_AsciiString aRLabelName;
    TDF_Tool::Entry(Handle(TDF_Reference)::DownCast(theAttribute)->Get(),aRLabelName);
    AddToName("= ");
    AddToName(aRLabelName);
  }
  return myName;
}

void DFBrowser_Attr::AddToName(const TCollection_ExtendedString& theString)
{
  myName += TCollection_AsciiString(theString,'?');
}

void DFBrowser_Attr::AddToName(const TCollection_AsciiString& theString)
{
  myName += theString;
}

void DFBrowser_Attr::AddToName(const Standard_CString theString)
{
  myName += theString;
}

void DFBrowser_Attr::AddToName(const Handle(TDF_Attribute)& theAttribute)
{
  if (theAttribute.IsNull()) {
    AddToName("null\n");
  } else {
    AddToName(theAttribute->DynamicType()->Name());
    if (!theAttribute->Label().IsNull()) {
      AddToName(" at label ");
      TCollection_AsciiString aLabelName;
      TDF_Tool::Entry(theAttribute->Label(),aLabelName);
      AddToName(aLabelName);
    } else AddToName(" with null label");
    AddToName("\n");
  }
}

void DFBrowser_Attr::AddList(const Standard_CString theString)
{
  if (theString == NULL) {
    if (myBrackets==0) AddToName("(");
    else myBrackets=0;
    AddToName(" )");
  } else {
    if (myBrackets==0) AddToName("( ");
    else AddToName(", ");
    AddToName(theString);
    myBrackets++;
  }
}

void DFBrowser_Attr::AddList(const Handle(TDF_Attribute)& theAttribute)
{
  if (theAttribute.IsNull()) {
    myError = Standard_True;
    AddList("NULL ATTRIBUTE");
    myColor = DFBrowser_RED;
  } else if (theAttribute->Label().IsNull()) {
    myError = Standard_True;
    AddList("NULL LABEL");
    myColor = DFBrowser_RED;
  } else {
    TCollection_AsciiString aLabelName;
    TDF_Tool::Entry(theAttribute->Label(),aLabelName);
    AddList(aLabelName.ToCString());
  }
}

const TCollection_AsciiString & DFBrowser_Attr::LoadShape(const TopoDS_Shape& theShape, const TDF_Label& theAccess)
{
  myName.Clear();
  if (!theShape.IsNull()) {
    AddToName(DB_SHAPE_TYPE,theShape.ShapeType());
    AddToName(" ");
    if (!TNaming_Tool::NamedShape(theShape,theAccess).IsNull()) {
      AddList(TNaming_Tool::NamedShape(theShape,theAccess));
      AddList(NULL);
    }
    AddToName(" TShape");
    char s[256];
    sprintf(s,"%p",(void*)&(*(theShape.TShape())));
    AddList(s);
    AddList(NULL);
  }
  else AddToName("Null shape ");
  return myName;
}

const TCollection_AsciiString & DFBrowser_Attr::LoadVertices(const TopoDS_Shape& theShape)
{
  myName = "Vertices:";
  TopTools_MapOfShape aVertices;
  char s[256];
  for(TopExp_Explorer anExp(theShape,TopAbs_VERTEX);anExp.More();anExp.Next()) {
    if (aVertices.Contains(anExp.Current())) continue;
    else aVertices.Add(anExp.Current());
    gp_Pnt aPoint = BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current()));
    sprintf(s,"(%5.f, %5.f, %5.f)",(float)aPoint.X(),(float)aPoint.Y(),(float)aPoint.Z());
    AddList(s);
  }
  AddList(NULL);
  return myName;
}

void DFBrowser_Attr::Description (const Handle(TDF_Attribute) &theAttr,
                                  TCollection_AsciiString &theName)
{
  myName.Clear();
  char *aString = new char[MAX_STR_LENGTH+1];
  TCollection_AsciiString aLabelName;
  /******************************************************************************\
  |**                        TDataStd package attributes                       **|
  \******************************************************************************/
  if (theAttr->ID() == TDataStd_Comment::GetID()) {
    Handle(TDataStd_Comment) aComment = Handle(TDataStd_Comment)::DownCast(theAttr);
    AddToName("text:\n");
    AddToName(aComment->Get());
  } else if (theAttr->ID() == TDataXtd_Constraint::GetID()) {
    Handle(TDataXtd_Constraint) aConstraint = Handle(TDataXtd_Constraint)::DownCast(theAttr);
    AddToName(DB_CONSTRAINT_TYPE,aConstraint->GetType());
    if (aConstraint->IsDimension()) {
      sprintf(aString," = %f",aConstraint->GetValue()->Get());
      AddToName(aString);
    }
    AddToName("\n");
    if (aConstraint->Reversed()) AddToName("reversed "); else AddToName("not reversed ");
    if (aConstraint->Inverted()) AddToName("inverted "); else AddToName("not inverted ");
    if (aConstraint->Verified()) AddToName("verified\n"); else AddToName("not verified\n");
    AddToName("Value is ");
    AddToName(aConstraint->GetValue());
    AddToName("Plane is ");
    AddToName(aConstraint->GetPlane());
    AddToName("Arguments:\n");
    for(Standard_Integer i=0;i<aConstraint->NbGeometries();i++) {
      AddToName(" ");
      AddToName(aConstraint->GetGeometry(i+1));
    }
  } else if (theAttr->ID() == TDataStd_Current::GetID()) {
    Handle(TDataStd_Current) aCurrent = Handle(TDataStd_Current)::DownCast(theAttr);
    AddToName("current: ");
    TDF_Tool::Entry(aCurrent->GetLabel(),aLabelName);
    AddToName(aLabelName);
  } else if (theAttr->ID() == TDataStd_Expression::GetID()) {
    Handle(TDataStd_Expression) anExpr = Handle(TDataStd_Expression)::DownCast(theAttr); 
    AddToName("Expression string:\n");
    AddToName(anExpr->GetExpression());
    AddToName("Variables:\n");
    TDF_ListIteratorOfAttributeList anIter1(anExpr->GetVariables());
    for(;anIter1.More();anIter1.Next()) {
      AddToName(" ");
      AddToName(anIter1.Value());
    }
  } else if (theAttr->ID() == TDataXtd_Geometry::GetID()) {
    Handle(TDataXtd_Geometry) aGeom = Handle(TDataXtd_Geometry)::DownCast(theAttr); 
    AddToName(DB_GEOM_TYPE,aGeom->GetType());
  } else if (theAttr->ID() == TDataStd_Integer::GetID()) {
    Handle(TDataStd_Integer) anInt = Handle(TDataStd_Integer)::DownCast(theAttr); 
    AddToName("value: ");
    sprintf(aString,"%d",anInt->Get());
    AddToName(aString);
  } else if (theAttr->ID() == TDataStd_IntegerArray::GetID()) {
    Handle(TDataStd_IntegerArray) array = Handle(TDataStd_IntegerArray)::DownCast(theAttr);
    sprintf(aString," Values [%d..%d]:\n",array->Lower(),array->Upper());
    AddToName(aString);
    for(Standard_Integer a = array->Lower();a<=array->Upper();a++) {
      sprintf(aString,"%d",array->Value(a));
      AddToName(aString);
      if ((a-array->Lower()+1)%10==0) AddToName("\n");
      else AddToName(" ");
    }
  } else if (theAttr->ID() == TDataStd_Name::GetID()) {
    Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(theAttr);
    AddToName("text:\n");
    AddToName(aName->Get());
  } else if (theAttr->ID() == TDataStd_Real::GetID()) {
    Handle(TDataStd_Real) aReal = Handle(TDataStd_Real)::DownCast(theAttr);
    AddToName("value: ");
    sprintf(aString,"%f\n",aReal->Get());
    AddToName(aString);
    AddToName(DB_DIMENSION_TYPE,aReal->GetDimension());
    AddToName(" dimension");
  } else if (theAttr->ID() == TDataStd_RealArray::GetID()) {
    Handle(TDataStd_RealArray) array = Handle(TDataStd_RealArray)::DownCast(theAttr);
    sprintf(aString," Values [%d..%d]:\n",array->Lower(),array->Upper());
    AddToName(aString);
    for(Standard_Integer a = array->Lower();a<=array->Upper();a++) {
      sprintf(aString,"%f",array->Value(a));
      AddToName(aString);
      if ((a-array->Lower()+1)%5==0) AddToName("\n");
      else AddToName(" ");
    }
  } else if (theAttr->ID() == TDataStd_Relation::GetID()) {
    Handle(TDataStd_Relation) aRelation = Handle(TDataStd_Relation)::DownCast(theAttr); 
    AddToName("Relation string:\n");
    AddToName(aRelation->GetRelation());
    AddToName("Variables:\n");
    TDF_ListIteratorOfAttributeList anIter1(aRelation->GetVariables());
    for(;anIter1.More();anIter1.Next()) {
      AddToName(" ");
      AddToName(anIter1.Value());
    }
  } else if (strcmp(theAttr->DynamicType()->Name(),"TDataStd_TreeNode")==0) {
    Handle(TDataStd_TreeNode) aNode = Handle(TDataStd_TreeNode)::DownCast(theAttr);
    AddToName("Tree ID:\n");
    Standard_GUID gui=aNode->ID();
    gui.ToCString(aString);
    AddToName(aString);
    if (aNode->HasFather()) {
      AddToName("\nFather:");
      AddToName(aNode->Father());
    } else AddToName("\nNo father\n");
    if (aNode->HasPrevious()) {
      AddToName("Previous:");
      AddToName(aNode->Previous());
    } else AddToName("No previous\n");
    if (aNode->HasNext()) {
      AddToName("Next:");
      AddToName(aNode->Next());
    } else AddToName("No next\n");
    if (aNode->HasFirst()) {
      AddToName("First:");
      AddToName(aNode->First());
    } else AddToName("No first\n");
  } else if (strcmp(theAttr->DynamicType()->Name(),"TDataStd_UAttribute")==0) {
    Handle(TDataStd_UAttribute) anUAttr = Handle(TDataStd_UAttribute)::DownCast(theAttr);
    AddToName("ID:\n");
    Standard_GUID gui=anUAttr->ID();
    gui.ToCString(aString);
    AddToName(aString);
  } else if (theAttr->ID() == TDataStd_Variable::GetID()) {
    Handle(TDataStd_Variable) aVariable = Handle(TDataStd_Variable)::DownCast(theAttr);
    AddToName("Name: ");
    AddToName(aVariable->Name());
    AddToName("\nUnit: ");
    AddToName(aVariable->Unit());
    if (aVariable->IsConstant()) AddToName("\nConstant");
  /******************************************************************************\
  |**                          TDF package attributes                          **|
  \******************************************************************************/
  } else if (theAttr->ID() == TDF_Reference::GetID()) {
    Handle(TDF_Reference) aReference = Handle(TDF_Reference)::DownCast(theAttr);
    AddToName("reference to: ");
    TDF_Tool::Entry(aReference->Get(),aLabelName);
    AddToName(aLabelName);
  } else if (theAttr->ID() == TDF_TagSource::GetID()) {
    Handle(TDF_TagSource) aTag = Handle(TDF_TagSource)::DownCast(theAttr);
    AddToName("Tag: ");
    sprintf(aString,"%d",aTag->Get());
    AddToName(aString);
  /******************************************************************************\
  |**                        TDocStd package attributes                        **|
  \******************************************************************************/
  } else if (theAttr->ID() == TDocStd_Owner::GetID()) {
    Handle(TDocStd_Document) aD=Handle(TDocStd_Owner)::DownCast(theAttr)->GetDocument();
    if (aD.IsNull()) {
      AddToName("Has no document");
    } else {
      AddToName("Document:\nname: ");
      if (aD->IsSaved()) AddToName(aD->GetName()); else AddToName("document is not saved");
      AddToName("\npath: ");
      if (aD->IsSaved()) AddToName(aD->GetPath()); else AddToName("document is not saved");
      AddToName("\nformat: ");
      AddToName(aD->StorageFormat());
      AddToName("\nmain label: ");
      TDF_Tool::Entry(aD->Main(),aLabelName);
      AddToName(aLabelName);
      sprintf(aString,"\nundo limit = %d\nundos = %d\nredos = %d\n",
	      aD->GetUndoLimit(),aD->GetAvailableUndos(),aD->GetAvailableRedos());
      AddToName(aString);
      if (aD->HasOpenCommand()) AddToName("Has open command"); else AddToName("Has no open commands");
    }
  } else if (theAttr->ID() == TDocStd_XLink::GetID()) {
    Handle(TDocStd_XLink) aXLink = Handle(TDocStd_XLink)::DownCast(theAttr);
    AddToName("Document entry: ");
    AddToName(aXLink->DocumentEntry());
    AddToName("\nLabel entry: ");
    AddToName(aXLink->LabelEntry());
//     AddToName("\nNext: ");
//     AddToName(aXLink->Next());
//   } else if (theAttr->ID() == TDocStd_XLinkRoot::GetID()) {
//     Handle(TDocStd_XLinkRoot) aXLinkRoot = Handle(TDocStd_XLinkRoot)::DownCast(theAttr);
//     AddToName("First: ");
//     AddToName(aXLinkRoot->First());
  /******************************************************************************\
  |**                        TPrsStd package attributes                        **|
  \******************************************************************************/
  } else if (theAttr->ID() == TPrsStd_AISPresentation::GetID()) {
    Handle(TPrsStd_AISPresentation) aPrs = Handle(TPrsStd_AISPresentation)::DownCast(theAttr);
    AddToName("Presentation driver ID:\n");
    Standard_GUID gui=aPrs->GetDriverGUID();
    gui.ToCString(aString);
    AddToName(aString);
    if (aPrs->IsDisplayed()) AddToName("\nIs displayed\n"); else AddToName("\nNot displayed\n");
    if (aPrs->HasOwnMaterial()) {
      AddToName("Own material: ");
      AddToName(DB_MATERIAL_TYPE,aPrs->Material());
    } else AddToName("Has no own material");
    if (aPrs->HasOwnTransparency()) {
      sprintf(aString,"\nOwn transparency: %f\n",(float)(aPrs->Transparency()));
      AddToName(aString);
    } else AddToName("\nHas no own transparency\n");
    if (aPrs->HasOwnColor()) {
      Quantity_Color aColor(aPrs->Color());
      sprintf(aString,"Own color: (r=%f,g=%f,b=%f)\n",(float)(aColor.Red()),(float)(aColor.Green()),(float)(aColor.Blue()));
      AddToName(aString);
    } else AddToName("Has no own color\n");
    if (aPrs->HasOwnWidth()) {
      sprintf(aString,"Own width: %d\n",aPrs->Width());
      AddToName(aString);
    } else AddToName("Has no own width\n");
    if (aPrs->HasOwnMode()) {
      sprintf(aString,"Own mode: %d (",aPrs->Mode());
      AddToName(aString);
      AddToName(DB_DISPLAY_MODE,aPrs->Mode());
      AddToName(")\n");
    } else AddToName("Has no own mode\n");
    if (aPrs->HasOwnSelectionMode()) {
      sprintf(aString,"Own selection mode: %d\n",aPrs->SelectionMode());
      AddToName(aString);
    } else AddToName("Has no own selection mode\n");
    if (!aPrs->GetAIS().IsNull()) {
      sprintf(aString,"Has AIS (0x%p)\n",(void*)&(*(aPrs->GetAIS())));
      AddToName(aString);
    } else AddToName("Has no AIS\n");
  } else if (theAttr->ID() == TPrsStd_AISViewer::GetID()) {
    Handle(TPrsStd_AISViewer) aViewer = Handle(TPrsStd_AISViewer)::DownCast(theAttr);
    if (aViewer->Has(aViewer->Label())) AddToName("Has access viewer in the dataframework\n");
    else AddToName("Has no access viewer in the dataframework\n");
    Handle(AIS_InteractiveContext) aContext = aViewer->GetInteractiveContext();
    sprintf(aString,"Interactive context (0x%p):\n",(void*)&(*aContext));
    AddToName(aString);
    if (!aContext.IsNull()) {
     // if (aContext->IsCollectorClosed()) AddToName("Collector is closed\n"); else AddToName("Collector is opened\n");
      sprintf(aString,"Size of trihedron: %f\n",(float)(aContext->TrihedronSize()));
      AddToName(aString);
      Standard_Real aXSize,aYSize;
      aContext->PlaneSize(aXSize,aYSize);
      sprintf(aString,"Plane size: (Xdirection:%f,Ydirection:%f)\n",(float)aXSize,(float)aYSize);
      AddToName(aString);
      Quantity_Color aColor(aContext->HilightColor());
      sprintf(aString,"Hilight color: (r=%f,g=%f,b=%f)\n",(float)(aColor.Red()),(float)(aColor.Green()),(float)(aColor.Blue()));
      AddToName(aString);
      aColor = aContext->SelectionColor();
      sprintf(aString,"Selection color: (r=%f,g=%f,b=%f)\n",(float)(aColor.Red()),(float)(aColor.Green()),(float)(aColor.Blue()));
      AddToName(aString);
      aColor = aContext->PreSelectionColor();
      sprintf(aString,"Preselection color: (r=%f,g=%f,b=%f)\n",(float)(aColor.Red()),(float)(aColor.Green()),(float)(aColor.Blue()));
      AddToName(aString);
      aColor = aContext->DefaultColor();
      sprintf(aString,"Default color: (r=%f,g=%f,b=%f)\n",(float)(aColor.Red()),(float)(aColor.Green()),(float)(aColor.Blue()));
      AddToName(aString);
      aColor = aContext->SubIntensityColor();
      sprintf(aString,"SubintensityColor color: (r=%f,g=%f,b=%f)\n",(float)(aColor.Red()),(float)(aColor.Green()),(float)(aColor.Blue()));
      AddToName(aString);
      sprintf(aString,"Deviation cefficient: %f\n",(float)(aContext->DeviationCoefficient()));
      AddToName(aString);
      sprintf(aString,"Deviation angle: %f\n",(float)(aContext->DeviationAngle()));
      AddToName(aString);
      sprintf(aString,"Hidden line remove deviation cefficient: %f\n",(float)(aContext->HLRDeviationCoefficient()));
      AddToName(aString);
      sprintf(aString,"Hidden line remove angle: %f\n",(float)(aContext->HLRAngle()));
      AddToName(aString);
      sprintf(aString,"Iso number: (U:%d,V:%d)\n",aContext->IsoNumber(AIS_TOI_IsoU),aContext->IsoNumber(AIS_TOI_IsoV));
      AddToName(aString);
      if (aContext->IsoOnPlane()) AddToName("Iso is on plane\n"); else AddToName("Iso is not on plane\n");
      if (aContext->HasOpenedContext()) sprintf(aString,"Current local context index: %d\n",aContext->IndexOfCurrentLocal());
      else sprintf(aString,"Has no opened context\n");
      AddToName(aString);
      if (aContext->AutomaticHilight()) AddToName("Automatic hilight is on\n"); else AddToName("Automatic hilight is off\n");
      sprintf(aString,"Current name: %s\n",aContext->CurrentName().ToCString());
      AddToName(aString);
      sprintf(aString,"Selection name: %s\n",aContext->SelectionName().ToCString());
      AddToName(aString);
      sprintf(aString,"Domain of main viewer: %s\n",aContext->DomainOfMainViewer());
      AddToName(aString);
      //sprintf(aString,"Domain of collector: %s",aContext->DomainOfCollector());
      //AddToName(aString);
    } else {AddToName("interactive context is null");}
  } else if (theAttr->ID() == TDataXtd_Position::GetID()) {
    Handle(TDataXtd_Position) aPosition = Handle(TDataXtd_Position)::DownCast(theAttr);
    gp_Pnt aPos;
    aPosition->Get(aPosition->Label(),aPos);
    sprintf(aString,"Position(%f,%f,%f)",(float)(aPos.X()),(float)(aPos.Y()),(float)(aPos.Z())); 
  /******************************************************************************\
  |**                        TNaming package attributes                        **|
  \******************************************************************************/
  } else if (theAttr->ID() == TNaming_NamedShape::GetID()) {
    Handle(TNaming_NamedShape) aNS = Handle(TNaming_NamedShape)::DownCast(theAttr);
    AddToName("Evolution: ");
    AddToName(DB_NS_TYPE,aNS->Evolution());
    sprintf(aString,"\nVersion: %d\n",aNS->Version());
    AddToName(aString);
    AddToName("New shape <- old shape:\n");
    TNaming_Iterator anIter2(aNS);
    for(;anIter2.More();anIter2.Next()) {
      TopoDS_Shape aShape = anIter2.NewShape();
      if (!aShape.IsNull()) {
	AddToName(DB_SHAPE_TYPE,aShape.ShapeType());
	AddToName("(");
	AddToName(DB_ORIENTATION_TYPE,aShape.Orientation());
	sprintf(aString," Ox%p) <- ",(void*)&(*(aShape.TShape())));
	AddToName(aString);
      } else AddToName("Empty shape <- ");
      aShape = anIter2.OldShape();
      if (!aShape.IsNull()) {
	AddToName(DB_SHAPE_TYPE,aShape.ShapeType());
	sprintf(aString,"(Ox%p)\n",(void*)&(*(aShape.TShape())));
	AddToName(aString);
      } else AddToName("Empty shape\n");
    }
  } else if (theAttr->ID() == TNaming_Naming::GetID()) {
    Handle(TNaming_Naming) aNaming = Handle(TNaming_Naming)::DownCast(theAttr);
    AddToName("Name type: ");
    AddToName(DB_NAMING_TYPE,aNaming->GetName().Type());
    AddToName("\nShape type: ");
    AddToName(DB_SHAPE_TYPE,aNaming->GetName().Type());
    sprintf(aString,"\nIndex: %d\n",aNaming->GetName().Index());
    AddToName(aString);
    AddToName("StopShape: ");
    AddList(aNaming->GetName().StopNamedShape());
    AddList(NULL);
    AddToName("\nArguments:\n");
    TNaming_ListIteratorOfListOfNamedShape aList(aNaming->GetName().Arguments());
    for(;aList.More();aList.Next()) {
      AddToName(aList.Value());
    }
  }
  theName += myName;
}

void DFBrowser_Attr::Description(const TDF_Label& theLabel,
                                 TCollection_AsciiString& theName)
{
  myName.Clear();
  TCollection_AsciiString aString("Label ");
  char s[256];
  sprintf(s,"0x%p",(void*)(&theLabel));
  aString += s;
  aString += "\nName: ";
  aString += LoadLabel(theLabel,Standard_False);
  aString += "\n";
  myName.Clear();
  AddToName(aString);
  if (theLabel.IsRoot()) AddToName("Is root\n"); else AddToName("Is not root\n");
  if (theLabel.IsImported()) AddToName("Is imported\n"); else AddToName("Is not imported\n");
  if (theLabel.AttributesModified()) AddToName("Has modified attributes\n");
  else AddToName("Have no modified attributes\n");
  aString = "Has ";
  aString += theLabel.NbAttributes();
  aString += " attributes, depth is ";
  aString += theLabel.Depth();
  aString += "\n";
  AddToName(aString);
  aString = "Transaction index ";
  aString += theLabel.Transaction();
  AddToName(aString);
  theName += myName;
}

void DFBrowser_Attr::AddToName(const Standard_Integer theType,const Standard_Integer theCount)
{
  if (theType == DB_CONSTRAINT_TYPE) {
    switch(theCount) {
    case TDataXtd_RADIUS: AddToName("RADIUS");break;
    case TDataXtd_DIAMETER: AddToName("DIAMETER");break;
    case TDataXtd_MINOR_RADIUS: AddToName("MINOR RADIUS");break;
    case TDataXtd_MAJOR_RADIUS: AddToName("MAJOR RADIUS");break;
    case TDataXtd_TANGENT: AddToName("TANGENT");break;
    case TDataXtd_PARALLEL: AddToName("PARALLEL");break;
    case TDataXtd_PERPENDICULAR: AddToName("PERPENDICULAR");break;
    case TDataXtd_CONCENTRIC: AddToName("CONCENTRIC");break;
    case TDataXtd_COINCIDENT: AddToName("COINCIDENT");break;
    case TDataXtd_DISTANCE: AddToName("DISTANCE");break;
    case TDataXtd_ANGLE: AddToName("ANGLE");break;
    case TDataXtd_EQUAL_RADIUS: AddToName("EQUAL RADIUS");break;
    case TDataXtd_SYMMETRY: AddToName("SYMMETRY");break;
    case TDataXtd_MIDPOINT: AddToName("MIDPOINT");break;
    case TDataXtd_EQUAL_DISTANCE: AddToName("EQUAL DISTANCE");break;
    case TDataXtd_FIX: AddToName("FIX");break;
    case TDataXtd_RIGID: AddToName("RIGID");break;
    case TDataXtd_FROM: AddToName("FROM");break;
    case TDataXtd_AXIS: AddToName("AXIS");break;
    case TDataXtd_MATE: AddToName("MATE");break;
    case TDataXtd_ALIGN_FACES: AddToName("ALIGN FACES");break;
    case TDataXtd_ALIGN_AXES: AddToName("ALIGN AXES");break;
    case TDataXtd_AXES_ANGLE: AddToName("AXES ANGLE");break;
    case TDataXtd_FACES_ANGLE: AddToName("FACES ANGLE");break;
    case TDataXtd_ROUND: AddToName("ROUND");break;
    case TDataXtd_OFFSET: AddToName("OFFSET");break;
    default: AddToName("UNKNOWN CONSTRAINT TYPE");
    }
  } else if (theType == DB_NAMING_TYPE) {
    switch(theCount) {
    case TNaming_UNKNOWN: AddToName("UNKNOWN");break;
    case TNaming_IDENTITY: AddToName("IDENTITY");break;
    case TNaming_MODIFUNTIL: AddToName("MODIFUNTIL");break;
    case TNaming_GENERATION: AddToName("GENERATION");break;
    case TNaming_INTERSECTION: AddToName("INTERSECTION");break;
    case TNaming_UNION: AddToName("UNION");break;
    case TNaming_SUBSTRACTION: AddToName("SUBSTRACTION");break;
    case TNaming_CONSTSHAPE: AddToName("CONSTSHAPE");break;
    case TNaming_FILTERBYNEIGHBOURGS: AddToName("FILTERBYNEIGHBOURGS");break;
    case TNaming_ORIENTATION: AddToName("ORIENTATION");break;
    case TNaming_WIREIN: AddToName("WIREIN");break;
    default: AddToName("UNKNOWN NAMING TYPE");
    }    
  } else if (theType == DB_SHAPE_TYPE) {
    switch(theCount) {
    case TopAbs_COMPOUND: AddToName("COMPOUND");break;
    case TopAbs_COMPSOLID: AddToName("COMPSOLID");break;
    case TopAbs_SOLID: AddToName("SOLID");break;
    case TopAbs_SHELL: AddToName("SHELL");break;
    case TopAbs_FACE: AddToName("FACE");break;
    case TopAbs_WIRE: AddToName("WIRE");break;
    case TopAbs_EDGE: AddToName("EDGE");break;
    case TopAbs_VERTEX: AddToName("VERTEX");break;
    case TopAbs_SHAPE: AddToName("SHAPE");break;
    default: AddToName("UNKNOWN SHAPE TYPE");
    }
  } else if (theType == DB_NS_TYPE) {
    switch(theCount) {
    case TNaming_PRIMITIVE: AddToName("PRIMITIVE");break;
    case TNaming_GENERATED: AddToName("GENERATED");break;
    case TNaming_MODIFY: AddToName("MODIFY");break;
    case TNaming_DELETE: AddToName("DELETE");break;
    case TNaming_REPLACE: AddToName("REPLACE");break;
    case TNaming_SELECTED: AddToName("SELECTED");break;
    default: AddToName("UNKNOWN EVOLUTION TYPE");
    }
  } else if (theType == DB_GEOM_TYPE) {
    switch(theCount) {
    case TDataXtd_ANY_GEOM: AddToName("ANY_GEOM");break;
    case TDataXtd_POINT: AddToName("POINT");break;
    case TDataXtd_LINE: AddToName("LINE");break;
    case TDataXtd_CIRCLE: AddToName("CIRCLE");break;
    case TDataXtd_ELLIPSE: AddToName("ELLIPSE");break;
    case TDataXtd_SPLINE: AddToName("SPLINE");break;
    case TDataXtd_PLANE: AddToName("PLANE");break;
    case TDataXtd_CYLINDER: AddToName("CYLINDER");break;
    default: AddToName("UNKNOWN GEOMETRY TYPE");
    }
  } else if (theType == DB_DIMENSION_TYPE) {
    switch(theCount) {
    case TDataStd_SCALAR: AddToName("SCALAR");break;
    case TDataStd_LENGTH: AddToName("LENGTH");break;
    case TDataStd_ANGULAR: AddToName("ANGULAR");break;
    default: AddToName("UNKNOWN DIMENSION TYPE");
    }
  } else if (theType == DB_MATERIAL_TYPE) {
    switch(theCount) {
    case Graphic3d_NOM_BRASS: AddToName("BRASS");break;
    case Graphic3d_NOM_BRONZE: AddToName("BRONZE");break;
    case Graphic3d_NOM_COPPER: AddToName("COPPER");break;
    case Graphic3d_NOM_GOLD: AddToName("GOLD");break;
    case Graphic3d_NOM_PEWTER: AddToName("PEWTER");break;
    case Graphic3d_NOM_PLASTER: AddToName("PLASTER");break;
    case Graphic3d_NOM_PLASTIC: AddToName("PLASTIC");break;
    case Graphic3d_NOM_SILVER: AddToName("SILVER");break;
    case Graphic3d_NOM_STEEL: AddToName("STEEL");break;
    case Graphic3d_NOM_STONE: AddToName("STONE");break;
    case Graphic3d_NOM_SHINY_PLASTIC: AddToName("SHINY PLASTIC");break;
    case Graphic3d_NOM_SATIN: AddToName("SATIN");break;
    case Graphic3d_NOM_METALIZED: AddToName("METALIZED");break;
    case Graphic3d_NOM_NEON_GNC: AddToName("NEON (generic)");break;
    case Graphic3d_NOM_CHROME: AddToName("CHROME");break;
    case Graphic3d_NOM_ALUMINIUM: AddToName("ALUMINIUM");break;
    case Graphic3d_NOM_OBSIDIAN: AddToName("OBSIDIAN");break;
    case Graphic3d_NOM_NEON_PHC: AddToName("NEON (physic)");break;
    case Graphic3d_NOM_JADE: AddToName("JADE");break;
    case Graphic3d_NOM_DEFAULT: AddToName("DEFAULT");break;
    default: AddToName("UNKNOWN MATERIAL");
    }
  } else if (theType == DB_DISPLAY_MODE) {
    switch(theCount) {
    case AIS_WireFrame: AddToName("WireFrame");break;
    case AIS_Shaded: AddToName("Shaded");break;
    default: AddToName("UNKNOWN DISPLAY MODE");
    }
  } else if (theType == DB_ORIENTATION_TYPE) {
    switch(theCount) {
    case TopAbs_FORWARD: AddToName("FORWARD");break;
    case TopAbs_REVERSED: AddToName("REVERSED");break;
    case TopAbs_INTERNAL: AddToName("INTERNAL");break;
    case TopAbs_EXTERNAL: AddToName("EXTERNAL");break;
    default: AddToName("UNKNOWN ORIENTATION TYPE");
    }
  } else AddToName("UNKNOWN PARAMETER");
}
