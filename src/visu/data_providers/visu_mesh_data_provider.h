//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_data_provider_h
#define visu_mesh_data_provider_h

// Visualization includes
#include <visu_data_provider.h>

// Mesh (Active Data) includes
#include <Mesh.h>
#include <Mesh_Group.h>

DEFINE_STANDARD_HANDLE(visu_mesh_data_provider, visu_data_provider)

//! Provides data necessary for visualization of mesh structures.
class visu_mesh_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_mesh_data_provider, visu_data_provider)

public:

  visu_mesh_data_provider();

  visu_mesh_data_provider(const ActAPI_DataObjectId&           theNodeId,
                          const Handle(ActAPI_HParameterList)& theParamList);

public:

  virtual ActAPI_DataObjectId
    GetNodeID() const;

  virtual Handle(Mesh)
    GetMeshDS() const;

  virtual Handle(ActAPI_HParameterList)
    SourceParameters() const;

public:

  //! Creates copy of Data Provider.
  //! \return copy.
  inline Handle(visu_mesh_data_provider) Clone() const
  {
    return new visu_mesh_data_provider(m_nodeID, m_params);
  }

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  ActAPI_DataObjectId           m_nodeID; //!< Source Node ID.
  Handle(ActAPI_HParameterList) m_params; //!< Source Parameters.

};

#endif
