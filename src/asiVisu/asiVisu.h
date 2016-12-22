//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_h
#define asiVisu_h

#define asiVisu_NotUsed(x)

#ifdef asiVisu_EXPORTS
  #define asiVisu_EXPORT __declspec(dllexport)
#else
  #define asiVisu_EXPORT __declspec(dllimport)
#endif

// VTK includes
#include <vtkCommand.h>

//-----------------------------------------------------------------------------

#define VTK_BAD_ID -1

//-----------------------------------------------------------------------------

// From VIS component (!)
#define VIS_SUBSHAPES_ARRNAME "SUBSHAPE_IDS"

//-----------------------------------------------------------------------------

#define ARRNAME_MESH_ITEM_TYPE   "arrname_tess_item_type"
#define ARRNAME_MESH_NODE_IDS    "arrname_asiData_TessNode_ids"
#define ARRNAME_MESH_ELEM_IDS    "arrname_tess_elem_ids"
#define ARRNAME_MESH_EN_SCALARS  "arrname_tess_en_scalars"
#define ARRNAME_MESH_E_SCALARS   "arrname_tess_e_scalars"
#define ARRNAME_MESH_E_VECTORS   "arrname_tess_e_vectors"
#define ARRNAME_MESH_N_SCALARS   "arrname_tess_n_scalars"
#define ARRNAME_ORIENT_SCALARS   "arrname_domain_scalars"
//
#define ARRNAME_SURF_CURVATURE "arrname_surf_curvature"

//-----------------------------------------------------------------------------

#define EVENT_ZOOM_RUBBER_RELEASE (vtkCommand::UserEvent + 1000)
#define EVENT_PICK_DEFAULT        (vtkCommand::UserEvent + 1001)
#define EVENT_DETECT_DEFAULT      (vtkCommand::UserEvent + 1002)
#define EVENT_ROTATION_START      (vtkCommand::UserEvent + 1003)
#define EVENT_ROTATION_END        (vtkCommand::UserEvent + 1004)
#define EVENT_DELETE              (vtkCommand::UserEvent + 1005)
#define EVENT_JOIN                (vtkCommand::UserEvent + 1006)
#define EVENT_FIND_FACE           (vtkCommand::UserEvent + 1007)
#define EVENT_FIND_EDGE           (vtkCommand::UserEvent + 1008)
#define EVENT_PICK_WORLD_POINT    (vtkCommand::UserEvent + 1008)
#define EVENT_TRACE_WORLD_POINTS  (vtkCommand::UserEvent + 1010)

//-----------------------------------------------------------------------------

#endif
