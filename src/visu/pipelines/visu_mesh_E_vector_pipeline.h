//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_E_vector_pipeline_h
#define visu_mesh_E_vector_pipeline_h

// Visualization includes
#include <visu_mesh_data_provider.h>
#include <visu_mesh_pipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_mesh_E_vector_data_provider, visu_mesh_data_provider)

//! Data source for the corresponding pipeline. Specifies all data necessary
//! for visualization of mesh with elemental vectors.
class visu_mesh_E_vector_data_provider : public visu_mesh_data_provider
{
public:

  DEFINE_STANDARD_RTTI_INLINE(visu_mesh_E_vector_data_provider,visu_mesh_data_provider)

public:

  virtual Handle(HIntArray)
    GetElementIDs() const = 0;

  virtual Handle(HRealArray)
    GetElementVectors() const = 0;

  virtual double
    GetMaxModulus() const = 0;

};

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_mesh_E_vector_pipeline, visu_pipeline)

//! Visualization pipeline for OMFDS meshes with elemental vectors.
class visu_mesh_E_vector_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_mesh_E_vector_pipeline, visu_pipeline)

public:

  visu_mesh_E_vector_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& theDataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* theRenderer);
  virtual void removeFromRendererCallback (vtkRenderer* theRenderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  visu_mesh_E_vector_pipeline(const visu_mesh_E_vector_pipeline&);

  //! Assignment prohibited.
  visu_mesh_E_vector_pipeline& operator=(const visu_mesh_E_vector_pipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_EVector = 1, //!< Filter for populating cell vector array.
    Filter_Trsf,        //!< Transformation filter.
    Filter_Glyph3D,     //!< Filter for glyphing.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

};

#endif
