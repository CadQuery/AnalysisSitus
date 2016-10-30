//-----------------------------------------------------------------------------
// Created on: 13 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef engine_volume_h
#define engine_volume_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_volume_node.h>

//! API for working with Data Nodes representing volumetric parts and features.
namespace engine_volume
{
  Handle(geom_volume_node)
    Create_Volume(const TopoDS_Shape&            shape,
                  const TCollection_AsciiString& name,
                  const Handle(ActAPI_INode)&    parent = NULL);

  void
    Clean_Volumes();

  Handle(ActAPI_INode)
    Get_RootVolume();

  Handle(ActAPI_HDataCursorList)
    Get_LeafVolumes();

  Handle(ActAPI_HDataCursorList)
    Get_LeafVolumesExcept(const Handle(ActAPI_INode)& excludedItem);

  void
    Move_Feature2Excluded(const Handle(ActAPI_INode)& owner_n,
                          const Handle(ActAPI_INode)& feature_n);

  void
    Move_Feature2Kept(const Handle(ActAPI_INode)& owner_n,
                      const Handle(ActAPI_INode)& feature_n);

};

#endif
