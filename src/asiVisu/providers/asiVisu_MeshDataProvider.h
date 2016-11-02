//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshDataProvider_h
#define asiVisu_MeshDataProvider_h

// Visualization includes
#include <asiVisu_DataProvider.h>

// Mesh (Active Data) includes
#include <Mesh.h>
#include <Mesh_Group.h>

DEFINE_STANDARD_HANDLE(asiVisu_MeshDataProvider, asiVisu_DataProvider)

//! Provides data necessary for visualization of mesh structures.
class asiVisu_MeshDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_MeshDataProvider();

  asiVisu_EXPORT
    asiVisu_MeshDataProvider(const ActAPI_DataObjectId&           theNodeId,
                             const Handle(ActAPI_HParameterList)& theParamList);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

  asiVisu_EXPORT virtual Handle(Mesh)
    GetMeshDS() const;

  asiVisu_EXPORT virtual Handle(ActAPI_HParameterList)
    SourceParameters() const;

public:

  //! Creates copy of Data Provider.
  //! \return copy.
  Handle(asiVisu_MeshDataProvider) Clone() const
  {
    return new asiVisu_MeshDataProvider(m_nodeID, m_params);
  }

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  ActAPI_DataObjectId           m_nodeID; //!< Source Node ID.
  Handle(ActAPI_HParameterList) m_params; //!< Source Parameters.

};

#endif
