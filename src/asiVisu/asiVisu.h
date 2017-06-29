//-----------------------------------------------------------------------------
// Created on: 25 September 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_h
#define asiVisu_h

#define asiVisu_NotUsed(x) x

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

#define ARRNAME_MESH_ITEM_TYPE     "arrname_tess_item_type"
#define ARRNAME_MESH_NODE_IDS      "arrname_asiData_TessNode_ids"
#define ARRNAME_MESH_ELEM_IDS      "arrname_tess_elem_ids"
#define ARRNAME_MESH_EN_SCALARS    "arrname_tess_en_scalars"
#define ARRNAME_MESH_E_SCALARS     "arrname_tess_e_scalars"
#define ARRNAME_MESH_E_VECTORS     "arrname_tess_e_vectors"
#define ARRNAME_MESH_N_SCALARS     "arrname_tess_n_scalars"
#define ARRNAME_ORIENT_SCALARS     "arrname_domain_scalars"
#define ARRNAME_POINTCLOUD_VECTORS "arrname_pointcloud_vectors"
//
#define ARRNAME_SURF_CURVATURE     "arrname_surf_curvature"
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
#define EVENT_DETECT_WORLD_POINT  (vtkCommand::UserEvent + 1009)
#define EVENT_SELECT_WORLD_POINT  (vtkCommand::UserEvent + 1010)
#define EVENT_TRACE_WORLD_POINTS  (vtkCommand::UserEvent + 1011)
#define EVENT_REFINE_TESSELLATION (vtkCommand::UserEvent + 1012)

//-----------------------------------------------------------------------------

#endif
