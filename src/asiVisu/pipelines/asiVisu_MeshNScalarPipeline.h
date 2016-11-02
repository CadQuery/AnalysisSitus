//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshNScalarPipeline_h
#define asiVisu_MeshNScalarPipeline_h

// Visualization includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshNScalarDataProvider, asiVisu_MeshDataProvider)

//! Data source for the corresponding pipeline. Specifies all data necessary
//! for visualization of mesh with nodal scalars.
class asiVisu_MeshNScalarDataProvider : public asiVisu_MeshDataProvider
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshNScalarDataProvider, asiVisu_MeshDataProvider)

public:

  virtual Handle(HIntArray)
    GetNodeIDs() const = 0;

  virtual Handle(HRealArray)
    GetNodeScalars() const = 0;

};

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshNScalarPipeline, asiVisu_Pipeline)

//! Visualization pipeline for meshes with nodal scalars.
class asiVisu_MeshNScalarPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshNScalarPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_MeshNScalarPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& theDataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* theRenderer);
  virtual void removeFromRendererCallback (vtkRenderer* theRenderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  asiVisu_MeshNScalarPipeline(const asiVisu_MeshNScalarPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshNScalarPipeline& operator=(const asiVisu_MeshNScalarPipeline&);

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
