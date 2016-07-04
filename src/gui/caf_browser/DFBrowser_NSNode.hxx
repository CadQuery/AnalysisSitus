//-----------------------------------------------------------------------------
// Created on: 18 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef _DFBrowser_NSNode_h
#define _DFBrowser_NSNode_h

#include <Handle_DFBrowser_NSNode.hxx>
#include <DFBrowser_AttrNode.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>
#include <Handle_DFBrowser_DFNode.hxx>

class DFBrowser_NSNode : public DFBrowser_AttrNode
{
 public:

  Standard_EXPORT DFBrowser_NSNode();

  Standard_EXPORT virtual void AddSub(Handle(DFBrowser_DFNode)& theNode);

  Standard_EXPORT virtual Handle(DFBrowser_DFNode) Sub() const;

  Standard_EXPORT virtual void Update();

  Standard_EXPORT Handle(TNaming_NamedShape) NamedShape() const;

  Standard_EXPORT void NamedShape(const Handle(TNaming_NamedShape)& theNS);

  inline Standard_Boolean Structure() const
  {
    return myStructure;
  }

  inline void Structure(const Standard_Boolean theSet)
  {
    if (!myIsShape) myStructure = theSet;
  }

  Standard_EXPORT void SetShape(const TopoDS_Shape& theNewShape,
                                const TopoDS_Shape& theOldShape,
                                const Standard_Boolean theVertices,
                                const Standard_Boolean theNew);

  inline const TopoDS_Shape & GetShape() const
  {
    return myShape;
  }

  Standard_EXPORT virtual void Del();

  DEFINE_STANDARD_RTTI_INLINE(DFBrowser_NSNode, DFBrowser_AttrNode)

 private:

  Standard_Boolean myStructure;
  Handle(DFBrowser_DFNode) myFirst;
  Standard_Boolean myIsShape;
  TopoDS_Shape myShape;
};

#endif
