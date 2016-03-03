//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_contour_pipeline_h
#define visu_mesh_contour_pipeline_h

// Visualization includes
#include <visu_mesh_pipeline.h>

// OCCT includes
#include <NCollection_DataMap.hxx>

DEFINE_STANDARD_HANDLE(visu_mesh_contour_pipeline, visu_mesh_pipeline)

//! Visualization pipeline for a tessellation represented with contour.
class visu_mesh_contour_pipeline : public visu_mesh_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_mesh_contour_pipeline, visu_mesh_pipeline)

public:

  visu_mesh_contour_pipeline();

public:

  void
    SetColor(const double fR, const double fG, const double fB);

  void
    SetOpacity(const double fOp);

  void
    SetLineWidth(const double theLineWidth);

private:

  //! Copying prohibited.
  visu_mesh_contour_pipeline(const visu_mesh_contour_pipeline&);

  //! Assignment prohibited.
  visu_mesh_contour_pipeline& operator=(const visu_mesh_contour_pipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_ExtractEdges = 1 //!< Filter to extract polygonal contour.
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Map of internally used filters.
  FilterMap m_filterMap;

  //! Contour color.
  double m_fColor[3];

  //! Contour opacity.
  double m_fOpacity;

  //! Contour line width.
  double m_fLineWidth;

};

#endif
