//-----------------------------------------------------------------------------
// Created on: 30 November 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeRobustPipeline_h
#define asiVisu_ShapeRobustPipeline_h

// A-Situs includes
#include <asiVisu_Pipeline.h>
#include <asiVisu_ShapeDataSource.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_ShapeRobustPipeline, asiVisu_Pipeline)

//! Visualization pipeline for OCCT shapes. This pipeline serves the purpose
//! of visualization of "not-so-valid" shapes. Such shapes can occur either
//! because of any sort of corruption, or they can represent some intermediate
//! state of modeling, e.g. a result of Euler Operation which breaks geometry,
//! but keeps topology consistent.
class asiVisu_ShapeRobustPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ShapeRobustPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_ShapeRobustPipeline(const bool isOCCTColorScheme = true,
                                const bool isBound2Node      = true);

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_ShapeRobustPipeline(const asiVisu_ShapeRobustPipeline&);

  //! Assignment prohibited.
  asiVisu_ShapeRobustPipeline& operator=(const asiVisu_ShapeRobustPipeline&);

protected:

  //! Indicates whether the standard OCCT color mapping is used as a default
  //! color scheme for the visualized shapes.
  bool m_bOCCTColorScheme;

  //! Indicates whether this pipeline must propagate the corresponding Node
  //! ID to its actor via vtkInformation hook.
  bool m_bIsBound2Node;

  //! Technical flag indicating whether a GL-mapper is initialized with the
  //! custom color scheme or not.
  bool m_bMapperColorsSet;

  //! Data source.
  vtkSmartPointer<asiVisu_ShapeDataSource> m_DS;

};

#endif
