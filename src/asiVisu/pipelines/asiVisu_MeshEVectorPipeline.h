//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshEVectorPipeline_h
#define asiVisu_MeshEVectorPipeline_h

// Visualization includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshEVectorDataProvider, asiVisu_MeshDataProvider)

//! Data source for the corresponding pipeline. Specifies all data necessary
//! for visualization of mesh with elemental vectors.
class asiVisu_MeshEVectorDataProvider : public asiVisu_MeshDataProvider
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshEVectorDataProvider,asiVisu_MeshDataProvider)

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

DEFINE_STANDARD_HANDLE(asiVisu_MeshEVectorPipeline, asiVisu_Pipeline)

//! Visualization pipeline for Mesh meshes with elemental vectors.
class asiVisu_MeshEVectorPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshEVectorPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_MeshEVectorPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& theDataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* theRenderer);
  virtual void removeFromRendererCallback (vtkRenderer* theRenderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  asiVisu_MeshEVectorPipeline(const asiVisu_MeshEVectorPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshEVectorPipeline& operator=(const asiVisu_MeshEVectorPipeline&);

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
