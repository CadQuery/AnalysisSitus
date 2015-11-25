//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_N_scalar_pipeline_h
#define visu_mesh_N_scalar_pipeline_h

// Visualization includes
#include <visu_mesh_data_provider.h>
#include <visu_mesh_pipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_mesh_N_scalar_data_provider, visu_mesh_data_provider)

//! Data source for the corresponding pipeline. Specifies all data necessary
//! for visualization of mesh with nodal scalars.
class visu_mesh_N_scalar_data_provider : public visu_mesh_data_provider
{
public:

  DEFINE_STANDARD_RTTI(visu_mesh_N_scalar_data_provider, visu_mesh_data_provider)

public:

  virtual Handle(HIntArray)
    GetNodeIDs() const = 0;

  virtual Handle(HRealArray)
    GetNodeScalars() const = 0;

};

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_mesh_N_scalar_pipeline, visu_pipeline)

//! Visualization pipeline for meshes with nodal scalars.
class visu_mesh_N_scalar_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(visu_mesh_N_scalar_pipeline, visu_pipeline)

public:

  ASitus_EXPORT
    visu_mesh_N_scalar_pipeline();

public:

  ASitus_EXPORT virtual void
    SetInput(const Handle(visu_data_provider)& theDataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* theRenderer);
  virtual void removeFromRendererCallback (vtkRenderer* theRenderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  visu_mesh_N_scalar_pipeline(const visu_mesh_N_scalar_pipeline&);

  //! Assignment prohibited.
  visu_mesh_N_scalar_pipeline& operator=(const visu_mesh_N_scalar_pipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_NScalar = 1, //!< Filter for populating point scalar array.
    Filter_Normals,     //!< Filter for calculation of normals.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

};

#endif
