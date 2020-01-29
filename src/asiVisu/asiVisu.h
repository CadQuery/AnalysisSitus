//-----------------------------------------------------------------------------
// Created on: 25 September 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_h
#define asiVisu_h

#define asiVisu_NotUsed(x)

#ifdef _WIN32
  #ifdef asiVisu_EXPORTS
    #define asiVisu_EXPORT __declspec(dllexport)
  #else
    #define asiVisu_EXPORT __declspec(dllimport)
  #endif
#else
  #define asiVisu_EXPORT
#endif

// VTK includes
#include <vtkCommand.h>

//-----------------------------------------------------------------------------

#define VTK_BAD_ID -1

//-----------------------------------------------------------------------------

// From VIS component (!)
#define VIS_SUBSHAPES_ARRNAME "SUBSHAPE_IDS"

//-----------------------------------------------------------------------------

#define ARRNAME_MESH_ITEM_TYPE     "arrname_tess_item_type"
#define ARRNAME_MESH_NODE_IDS      "arrname_tess_node_ids"
#define ARRNAME_MESH_ELEM_IDS      "arrname_tess_elem_ids"
#define ARRNAME_MESH_EN_SCALARS    "arrname_tess_en_scalars"
#define ARRNAME_MESH_E_SCALARS     "arrname_tess_e_scalars"
#define ARRNAME_MESH_E_VECTORS     "arrname_tess_e_vectors"
#define ARRNAME_MESH_N_SCALARS     "arrname_tess_n_scalars"
#define ARRNAME_ORIENT_SCALARS     "arrname_domain_scalars"
#define ARRNAME_CURVCOMBS_SCALARS  "arrname_curvature_combs_scalars"
#define ARRNAME_ISOSMULTS_SCALARS  "arrname_isos_mults_scalars"
#define ARRNAME_CURVIAXES_SCALARS  "arrname_curviaxes_scalars"
#define ARRNAME_POINTCLOUD_VECTORS "arrname_pointcloud_vectors"
#define ARRNAME_VOXEL_N_SCALARS    "arrname_voxel_n_scalars"
//
#define ARRNAME_SURF_SCALARS       "arrname_surf_scalars"
//
#define ARRNAME_PART_SUBSHAPE_IDS  "arrname_part_subshape_ids"
#define ARRNAME_PART_CELL_TYPES    "arrname_part_cell_types"

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
#define EVENT_REFINE_TESSELLATION (vtkCommand::UserEvent + 1009)

// For interactive picking:
#define EVENT_DETECT_WORLD_POINT  (vtkCommand::UserEvent + 1010) // World picker.
#define EVENT_SELECT_WORLD_POINT  (vtkCommand::UserEvent + 1011) // World picker.
#define EVENT_DETECT_POINT        (vtkCommand::UserEvent + 1012) // Point picker.
#define EVENT_SELECT_POINT        (vtkCommand::UserEvent + 1013) // Point picker.
#define EVENT_DETECT_CELL         (vtkCommand::UserEvent + 1014) // Cell picker.
#define EVENT_SELECT_CELL         (vtkCommand::UserEvent + 1015) // Cell picker.

//-----------------------------------------------------------------------------

#endif
