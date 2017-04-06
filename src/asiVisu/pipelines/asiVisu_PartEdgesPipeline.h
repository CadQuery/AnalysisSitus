//-----------------------------------------------------------------------------
// Created on: 06 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PartEdgesPipeline_h
#define asiVisu_PartEdgesPipeline_h

// asiVisu includes
#include <asiVisu_DisplayModeFilter.h>
#include <asiVisu_Pipeline.h>
#include <asiVisu_ShapeRobustSource.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_PartEdgesPipeline, asiVisu_Pipeline)

//! Visualization pipeline for wireframe of OCCT shape. This pipeline is
//! secondary: it should be feeded with a source of a main shape pipeline.
class asiVisu_PartEdgesPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartEdgesPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_PartEdgesPipeline(const vtkSmartPointer<asiVisu_ShapeRobustSource>& source);

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

public:

  //! \return data source.
  const vtkSmartPointer<asiVisu_ShapeRobustSource>& GetSource() const
  {
    return m_source;
  }

  //! \return display mode filter.
  const vtkSmartPointer<asiVisu_DisplayModeFilter>& GetDisplayModeFilter() const
  {
    return m_dmFilter;
  }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_PartEdgesPipeline(const asiVisu_PartEdgesPipeline&);

  //! Assignment prohibited.
  asiVisu_PartEdgesPipeline& operator=(const asiVisu_PartEdgesPipeline&);

protected:

  //! Technical flag indicating whether a GL-mapper is initialized with the
  //! custom color scheme or not.
  bool m_bMapperColorsSet;

  //! Data source.
  vtkSmartPointer<asiVisu_ShapeRobustSource> m_source;

  //! Display mode filter.
  vtkSmartPointer<asiVisu_DisplayModeFilter> m_dmFilter;

};

#endif
