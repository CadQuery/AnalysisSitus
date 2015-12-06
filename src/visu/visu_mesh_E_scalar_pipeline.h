//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_E_scalar_pipeline_h
#define visu_mesh_E_scalar_pipeline_h

// Visualization includes
#include <visu_mesh_data_provider.h>
#include <visu_mesh_pipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_mesh_E_scalar_data_provider, visu_mesh_data_provider)

//! Provides data necessary for visualization of mesh with elemental scalars.
class visu_mesh_E_scalar_data_provider : public visu_mesh_data_provider
{
public:

  DEFINE_STANDARD_RTTI(visu_mesh_E_scalar_data_provider, visu_mesh_data_provider)

public:

  virtual Handle(HIntArray)
    GetElementIDs() const = 0;

  virtual Handle(HRealArray)
    GetElementScalars() const = 0;

};

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_mesh_E_scalar_pipeline, visu_pipeline)

//! Visualization pipeline for meshes with elemental scalars.
class visu_mesh_E_scalar_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(visu_mesh_E_scalar_pipeline, visu_pipeline)

public:

  visu_mesh_E_scalar_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& dataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* renderer);
  virtual void removeFromRendererCallback (vtkRenderer* renderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  visu_mesh_E_scalar_pipeline(const visu_mesh_E_scalar_pipeline&);

  //! Assignment prohibited.
  visu_mesh_E_scalar_pipeline& operator=(const visu_mesh_E_scalar_pipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_EScalar = 1, //!< Filter for populating cell scalar array.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

};

#endif
