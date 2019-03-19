//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2016-2018 OPEN CASCADE SAS
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_Attr_h
#define _DFBrowser_Attr_h

#include <Handle_DFBrowser_Attr.hxx>
#include <Standard_Transient.hxx>

#include <Standard_CString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <DFBrowser_Colors.hxx>

class TDF_Label;
class TopoDS_Shape;
class TCollection_ExtendedString;

class DFBrowser_Attr : public Standard_Transient
{
 public:

  Standard_EXPORT DFBrowser_Attr();

  Standard_EXPORT const TCollection_AsciiString & LoadLabel(const TDF_Label& theLabel,const Standard_Boolean theName = Standard_True);

  Standard_EXPORT const TCollection_AsciiString & LoadAttribute(const Handle(TDF_Attribute)& theAttribute);

  Standard_EXPORT const TCollection_AsciiString & LoadAttribute(const Handle(TDF_Attribute)& theAttribute,const Standard_Boolean theFull);

  inline DFBrowser_Colors GetColor() const
  {
    return myColor;
  }

  inline const TCollection_AsciiString & GetName() const
  {
    return myName;
  }
  
  inline Standard_Boolean HasError() const
  {
    return myError;
  }

  inline void Detailed(const Standard_Boolean theDetailed)
  {
    myDetailed = theDetailed;
  }

  Standard_EXPORT void NamedShape(const Standard_Address theTree, const Standard_Address theNode);

  Standard_EXPORT const TCollection_AsciiString & LoadShape(const TopoDS_Shape& theShape,const TDF_Label& theAccess);

  Standard_EXPORT const TCollection_AsciiString & LoadVertices(const TopoDS_Shape& theShape);

  Standard_EXPORT void Description(const Handle(TDF_Attribute) &theAttr,TCollection_AsciiString &theName);

  Standard_EXPORT void Description(const TDF_Label &theLabel,TCollection_AsciiString &theName);

  Standard_EXPORT void AddToName(const TCollection_ExtendedString& theString);

  Standard_EXPORT void AddToName(const TCollection_AsciiString& theString);

  Standard_EXPORT void AddToName(const Standard_CString theString);

  Standard_EXPORT void AddToName(const Handle(TDF_Attribute)& theAttribute);

  Standard_EXPORT void AddToName(const Standard_Integer theType,const Standard_Integer theCount);

  DEFINE_STANDARD_RTTI_INLINE(DFBrowser_Attr, Standard_Transient)

 private:

  Standard_EXPORT void AddList(const Standard_CString theString);

  Standard_EXPORT void AddList(const Handle(TDF_Attribute)& theAttribute);

  TCollection_AsciiString myName;
  DFBrowser_Colors myColor;
  Standard_Integer myBrackets;
  Standard_Boolean myError;
  Standard_Boolean myDetailed;
};

#endif
