//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_common_h
#define visu_common_h

// A-Situs includes
#include <analysis_situs.h>

// VTK includes
#include <vtkCommand.h>

//-----------------------------------------------------------------------------

#define VTK_BAD_ID -1

//-----------------------------------------------------------------------------

// From VIS component (!)
#define VIS_SUBSHAPES_ARRNAME "SUBSHAPE_IDS"

//-----------------------------------------------------------------------------

#define ARRNAME_MESH_ITEM_TYPE   "arrname_tess_item_type"
#define ARRNAME_MESH_NODE_IDS    "arrname_tess_node_ids"
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

//-----------------------------------------------------------------------------

#endif
