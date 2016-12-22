//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshContourPipeline_h
#define asiVisu_MeshContourPipeline_h

// Visualization includes
#include <asiVisu_MeshPipeline.h>

// OCCT includes
#include <NCollection_DataMap.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_MeshContourPipeline, asiVisu_MeshPipeline)

//! Visualization pipeline for a tessellation represented with contour.
class asiVisu_MeshContourPipeline : public asiVisu_MeshPipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshContourPipeline, asiVisu_MeshPipeline)

public:

  asiVisu_EXPORT
    asiVisu_MeshContourPipeline(const bool doUseDefaultColor = true);

private:

  //! Copying prohibited.
  asiVisu_MeshContourPipeline(const asiVisu_MeshContourPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshContourPipeline& operator=(const asiVisu_MeshContourPipeline&);

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
