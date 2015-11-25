//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_data_provider_h
#define visu_mesh_data_provider_h

// Visualization includes
#include <visu_data_provider.h>

// OMFDS includes
#include <OMFDS_Mesh.hxx>
#include <OMFDS_MeshGroup.hxx>

DEFINE_STANDARD_HANDLE(visu_mesh_data_provider, visu_data_provider)

//! Provides data necessary for visualization of mesh structures.
class visu_mesh_data_provider : public visu_data_provider
{
public:

  DEFINE_STANDARD_RTTI(visu_mesh_data_provider, visu_data_provider)

public:

  virtual Handle(OMFDS_Mesh)
    GetMeshDS() const = 0;

  virtual bool
    HasMeshGroup() const = 0;

  virtual Handle(OMFDS_MeshGroup)
    GetMeshGroup() const = 0;

};

#endif
