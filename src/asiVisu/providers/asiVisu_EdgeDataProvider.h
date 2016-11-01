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

  asiVisu_EdgeDataProvider(const ActAPI_DataObjectId&           theNodeId,
                           const Handle(ActAPI_HParameterList)& theParamList);

public:

  ActAPI_DataObjectId GetNodeID                  () const;
  int                 GetEdgeIndexAmongSubshapes () const;
  int                 GetEdgeIndexAmongEdges     () const;
  TopoDS_Edge         ExtractEdge                () const;

public:

  virtual Handle(Standard_Type) GetCurveType ()                     const;
  virtual Handle(Geom2d_Curve)  GetCurve2d   (double& f, double& l) const;
  virtual Handle(Geom_Curve)    GetCurve     (double& f, double& l) const;

public:

  Handle(asiVisu_EdgeDataProvider)
    Clone() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  //! Source Node ID.
  ActAPI_DataObjectId m_nodeID;

  //! Source Parameters.
  Handle(ActAPI_HParameterList) m_params;

  //! Map of sub-shapes.
  TopTools_IndexedMapOfShape m_subShapes;

  //! Map of edges.
  TopTools_IndexedMapOfShape m_edges;

};

#endif
