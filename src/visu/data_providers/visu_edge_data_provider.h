//-----------------------------------------------------------------------------
// Created on: 18 August 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_edge_data_provider_h
#define visu_edge_data_provider_h

// A-Situs visualization includes
#include <visu_data_provider.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

DEFINE_STANDARD_HANDLE(visu_edge_data_provider, visu_data_provider)

//! Data provider for edge geometry.
class visu_edge_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_edge_data_provider, visu_data_provider)

public:

  visu_edge_data_provider(const ActAPI_DataObjectId&           theNodeId,
                          const Handle(ActAPI_HParameterList)& theParamList);

public:

  ActAPI_DataObjectId GetNodeID                  () const;
  int                 GetEdgeIndexAmongSubshapes () const;
  int                 GetEdgeIndexAmongEdges     () const;
  TopoDS_Edge         ExtractEdge                () const;

public:

  Handle(visu_edge_data_provider)
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
