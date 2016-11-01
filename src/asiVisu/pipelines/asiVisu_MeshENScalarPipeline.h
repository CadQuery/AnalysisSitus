//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshENScalarPipeline_h
#define asiVisu_MeshENScalarPipeline_h

// Visualization includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshENScalarDataProvider, asiVisu_MeshDataProvider)

//! Data source for the corresponding pipeline. Specifies all data necessary
//! for visualization of mesh with element nodal scalars.
class asiVisu_MeshENScalarDataProvider : public asiVisu_MeshDataProvider
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshENScalarDataProvider, asiVisu_MeshDataProvider)

public:

  virtual Handle(HIntArray)
    GetTriIDs() const = 0;

  virtual Handle(HRealArray)
    GetTriScalars() const = 0;

  virtual Handle(HIntArray)
    GetQuadIDs() const = 0;

  virtual Handle(HRealArray)
    GetQuadScalars() const = 0;

};

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshENScalarPipeline, asiVisu_Pipeline)

//! Visualization pipeline for meshes with element nodal scalars.
class asiVisu_MeshENScalarPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshENScalarPipeline, asiVisu_Pipeline)

public:

  asiVisu_MeshENScalarPipeline();

public:

  virtual void
    SetInput(const Handle(asiVisu_DataProvider)& theDataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* theRenderer);
  virtual void removeFromRendererCallback (vtkRenderer* theRenderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  asiVisu_MeshENScalarPipeline(const asiVisu_MeshENScalarPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshENScalarPipeline& operator=(const asiVisu_MeshENScalarPipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_ENScalar = 1,  //!< Filter for populating point scalar array.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

};

#endif
