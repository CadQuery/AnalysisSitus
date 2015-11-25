//-----------------------------------------------------------------------------
// Created on: 13 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_pipeline_h
#define visu_mesh_pipeline_h

// Visualization includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>

DEFINE_STANDARD_HANDLE(visu_mesh_pipeline, visu_pipeline)

//! Visualization pipeline for facets.
class visu_mesh_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(visu_mesh_pipeline, visu_pipeline)

public:

  ASitus_EXPORT
    visu_mesh_pipeline();

public:

  ASitus_EXPORT virtual void
    SetInput(const Handle(visu_data_provider)& dataProvider);

public:

  ASitus_EXPORT void
    EmptyGroupForAllModeOn();

  ASitus_EXPORT void
    EmptyGroupForAllModeOff();

  ASitus_EXPORT bool
    IsShrinkMode() const;

  ASitus_EXPORT void
    ShrinkModeOn();

  ASitus_EXPORT void
    ShrinkModeOff();

private:

  virtual void addToRendererCallback      (vtkRenderer* theRenderer);
  virtual void removeFromRendererCallback (vtkRenderer* theRenderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  visu_mesh_pipeline(const visu_mesh_pipeline&);

  //! Assignment prohibited.
  visu_mesh_pipeline& operator=(const visu_mesh_pipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_Shrink = 1, //!< VTK built-in filter for shrink mode.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

  //! Technical flag indicating whether a GL-mapper is initialized with the
  //! custom color scheme or not.
  bool m_bMapperColorsSet;

  //! Indicates whether to treat empty mesh group as a signal to keep all
  //! mesh elements visualized. Otherwise all them are just skipped.
  bool m_bIsEmptyGroupForAll;

};

#endif
