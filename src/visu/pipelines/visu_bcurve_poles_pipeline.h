//-----------------------------------------------------------------------------
// Created on: 15 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_bcurve_poles_pipeline_h
#define visu_bcurve_poles_pipeline_h

// A-Situs includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>

// VTK includes
#include <vtkExtractSelection.h>
#include <vtkGeometryFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_bcurve_poles_pipeline, visu_pipeline)

//! Visualization pipeline for b-curve poles.
class visu_bcurve_poles_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_bcurve_poles_pipeline, visu_pipeline)

public:

  visu_bcurve_poles_pipeline();

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& DP);

public:

  void
    SetSelectedPoles(const TColStd_PackedMapOfInteger& mask);

public:

  inline void ForceExecution() { m_bForced = true; }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_bcurve_poles_pipeline(const visu_bcurve_poles_pipeline&);

  //! Assignment prohibited.
  visu_bcurve_poles_pipeline& operator=(const visu_bcurve_poles_pipeline&);

private:

  bool                                 m_bForced;          //!< Forced update.
  bool                                 m_bMapperColorsSet; //!< Boolean flag indicating whether lookup table is set.
  vtkSmartPointer<vtkIdTypeArray>      m_selected;         //!< Poles selected for visualization.
  vtkSmartPointer<vtkSelectionNode>    m_selectionNode;    //!< VTK selection node.
  vtkSmartPointer<vtkSelection>        m_selection;        //!< VTK selection.
  vtkSmartPointer<vtkExtractSelection> m_extractSelection; //!< VTK selection extractor.
  vtkSmartPointer<vtkGeometryFilter>   m_toPolyData;       //!< VTK geometry filter.

};

#endif
