//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_EdgeDataProvider_h
#define asiVisu_EdgeDataProvider_h

// A-Situs visualization includes
#include <asiVisu_CurveDataProvider.h>

// asiData includes
#include <asiData_CurveNode.h>
#include <asiData_EdgeNode.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_EdgeDataProvider, asiVisu_CurveDataProvider)

//! Data provider for edge geometry.
class asiVisu_EdgeDataProvider : public asiVisu_CurveDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_EdgeDataProvider, asiVisu_CurveDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_EdgeDataProvider(const Handle(asiData_EdgeNode)& edge_n);

  asiVisu_EXPORT
    asiVisu_EdgeDataProvider(const Handle(asiData_CurveNode)& curve_n);

public:

  asiVisu_EXPORT ActAPI_DataObjectId
    GetNodeID() const;

  asiVisu_EXPORT int
    GetEdgeIndexAmongSubshapes() const;

  asiVisu_EXPORT int
    GetEdgeIndexAmongEdges() const;

  asiVisu_EXPORT TopoDS_Edge
    ExtractEdge() const;

public:

  asiVisu_EXPORT virtual Handle(Standard_Type)
    GetCurveType() const;

  asiVisu_EXPORT virtual Handle(Geom2d_Curve)
    GetCurve2d(double& f, double& l) const;

  asiVisu_EXPORT virtual Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

public:

  asiVisu_EXPORT Handle(asiVisu_EdgeDataProvider)
    Clone() const;

protected:

  asiVisu_EXPORT void
    init(const Handle(ActAPI_INode)& subNode);

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  //! Source Node.
  Handle(ActAPI_INode) m_node;

  //! Map of sub-shapes.
  TopTools_IndexedMapOfShape m_subShapes;

  //! Map of edges.
  TopTools_IndexedMapOfShape m_edges;

};

#endif
