//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_FaceDomainPipeline_h
#define asiVisu_FaceDomainPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkExtractSelection.h>
#include <vtkGeometryFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_FaceDomainPipeline, asiVisu_Pipeline)

//! Visualization pipeline for face domain.
class asiVisu_FaceDomainPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FaceDomainPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_FaceDomainPipeline(const bool isDefaultColorScheme = true);

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

public:

  void ForceExecution() { m_bForced = true; }

public:

  asiVisu_EXPORT void
    SetSelectedCells(const TColStd_PackedMapOfInteger& mask);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_FaceDomainPipeline(const asiVisu_FaceDomainPipeline&);

  //! Assignment prohibited.
  asiVisu_FaceDomainPipeline& operator=(const asiVisu_FaceDomainPipeline&);

private:

  double computeTipSize(const TopoDS_Face& F) const;

private:

  bool                                 m_bDefaultColorScheme; //!< Indicates whether to use a default color scheme.
  bool                                 m_bForced;             //!< Forced update.
  bool                                 m_bMapperColorsSet;    //!< Boolean flag indicating whether lookup table is set.
  vtkSmartPointer<vtkIdTypeArray>      m_selected;            //!< Poles selected for visualization.
  vtkSmartPointer<vtkSelectionNode>    m_selectionNode;       //!< VTK selection node.
  vtkSmartPointer<vtkSelection>        m_selection;           //!< VTK selection.
  vtkSmartPointer<vtkExtractSelection> m_extractSelection;    //!< VTK selection extractor.
  vtkSmartPointer<vtkGeometryFilter>   m_toPolyData;          //!< VTK geometry filter.

};

#endif
