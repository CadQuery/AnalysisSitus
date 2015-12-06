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

  visu_mesh_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& dataProvider);

public:

  void
    EmptyGroupForAllModeOn();

  void
    EmptyGroupForAllModeOff();

  bool
    IsShrinkMode() const;

  void
    ShrinkModeOn();

  void
    ShrinkModeOff();

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

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
