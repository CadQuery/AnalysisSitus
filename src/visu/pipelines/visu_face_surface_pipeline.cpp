//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_face_surface_pipeline.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_curve_source.h>
#include <visu_face_data_provider.h>
#include <visu_utils.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <BRep_Tool.hxx>

//-----------------------------------------------------------------------------

//! Creates new Face Surface Pipeline initialized by default VTK mapper and actor.
visu_face_surface_pipeline::visu_face_surface_pipeline()
: visu_pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                 vtkSmartPointer<vtkActor>::New() ),
  m_iStepsNumber(80)
{
  this->Actor()->GetProperty()->SetLineWidth(1.0);
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void visu_face_surface_pipeline::SetInput(const Handle(visu_data_provider)& DP)
{
  if ( !m_iStepsNumber )
    return;

  Handle(visu_face_data_provider)
    faceProvider = Handle(visu_face_data_provider)::DownCast(DP);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  const int face_idx = faceProvider->GetFaceIndexAmongSubshapes();
  if ( !face_idx )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> aDummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(aDummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( faceProvider->MustExecute( this->GetMTime() ) )
  {
    TopoDS_Face F = faceProvider->ExtractFace();
    //
    if ( F.IsNull() )
      return;

    // Get access to the host surface
    Handle(Geom_Surface) S = BRep_Tool::Surface(F);
    //
    if ( S.IsNull() )
      return;

    // Append filter
    vtkSmartPointer<vtkAppendPolyData>
      appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

    // The idea is to represent a surface with a grid of iso-parametric lines.
    // Each line in a grid is a parametric curve. To obtain it, we have to
    // sample the parametric space by u and v coordinates consequently (with
    // one variable fixed and another varying). The step of parameter sampling
    // is to be configured by user externally

    double uMin, uMax, vMin, vMax;
    S->Bounds(uMin, uMax, vMin, vMax);

    uMin = visu_utils::TrimInf(uMin);
    uMax = visu_utils::TrimInf(uMax);
    vMin = visu_utils::TrimInf(vMin);
    vMax = visu_utils::TrimInf(vMax);

    const double uStep = (uMax - uMin) / m_iStepsNumber;
    const double vStep = (vMax - vMin) / m_iStepsNumber;

    // Generate u-isos
    double u     = uMin;
    bool   uStop = false;
    while ( !uStop )
    {
      if ( u > uMax )
      {
        u     = uMax;
        uStop = true;
      }

      Handle(Geom_Curve) uIso = S->UIso(u);
      if ( uIso.IsNull() )
        continue;

      u += uStep;

      // Allocate Data Source
      vtkSmartPointer<visu_curve_source>
        curveSource = vtkSmartPointer<visu_curve_source>::New();

      // Set geometry to be converted to VTK polygonal DS
      if ( !curveSource->SetInputCurve( uIso, uIso->FirstParameter(), uIso->LastParameter() ) )
        continue; // No poly data produced

      // Append poly data
      appendFilter->AddInputConnection( curveSource->GetOutputPort() );
    }

    // Generate v-isos
    double v     = vMin;
    bool   vStop = false;
    while ( !vStop )
    {
      if ( v > vMax )
      {
        v     = vMax;
        vStop = true;
      }

      Handle(Geom_Curve) vIso = S->VIso(v);
      if ( vIso.IsNull() )
        continue;

      v += vStep;

      // Allocate Data Source
      vtkSmartPointer<visu_curve_source>
        curveSource = vtkSmartPointer<visu_curve_source>::New();

      // Set geometry to be converted to VTK polygonal DS
      if ( !curveSource->SetInputCurve( vIso, vIso->FirstParameter(), vIso->LastParameter() ) )
        continue; // No poly data produced

      // Append poly data
      appendFilter->AddInputConnection( curveSource->GetOutputPort() );
    }

    // Initialize pipeline
    this->SetInputConnection( appendFilter->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_face_surface_pipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void visu_face_surface_pipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void visu_face_surface_pipeline::callback_update()
{}
