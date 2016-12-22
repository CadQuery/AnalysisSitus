//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshEScalarPipeline_h
#define asiVisu_MeshEScalarPipeline_h

// Visualization includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshEScalarDataProvider, asiVisu_MeshDataProvider)

//! Provides data necessary for visualization of mesh with elemental scalars.
class asiVisu_MeshEScalarDataProvider : public asiVisu_MeshDataProvider
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshEScalarDataProvider, asiVisu_MeshDataProvider)

public:

  virtual Handle(HIntArray)
    GetElementIDs() const = 0;

  virtual Handle(HRealArray)
    GetElementScalars() const = 0;

};

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshEScalarPipeline, asiVisu_Pipeline)

//! Visualization pipeline for meshes with elemental scalars.
class asiVisu_MeshEScalarPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshEScalarPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_MeshEScalarPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* renderer);
  virtual void removeFromRendererCallback (vtkRenderer* renderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  asiVisu_MeshEScalarPipeline(const asiVisu_MeshEScalarPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshEScalarPipeline& operator=(const asiVisu_MeshEScalarPipeline&);

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
