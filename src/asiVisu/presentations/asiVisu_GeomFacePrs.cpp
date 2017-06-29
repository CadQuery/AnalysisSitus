//-----------------------------------------------------------------------------
// Created on: 02 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomFacePrs.h>

// Visualization includes
#include <asiVisu_FaceDataProvider.h>
#include <asiVisu_FaceDomainPipeline.h>
#include <asiVisu_Utils.h>

// Geometry includes
#include <asiAlgo_Utils.h>

// OCCT includes
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>

//! Creates a Presentation object for the passed Geometry Face Node.
//! \param N [in] Face Node to create a Presentation for.
asiVisu_GeomFacePrs::asiVisu_GeomFacePrs(const Handle(ActAPI_INode)& N)
: asiVisu_Prs(N)
{
  Handle(asiData_FaceNode) face_n = Handle(asiData_FaceNode)::DownCast(N);

  // Create Data Provider
  Handle(asiVisu_FaceDataProvider) DP = new asiVisu_FaceDataProvider(face_n);

  // Pipelines for face
  this->addPipeline        ( Pipeline_Main, new asiVisu_FaceDomainPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  asiVisu_Utils::InitTextWidget(m_textWidget);

  /* ========================
   *  Pipeline for detecting
   * ======================== */

  double detect_color[3];
  asiVisu_Utils::DefaultDetectionColor(detect_color[0], detect_color[1], detect_color[2]);

  // Create pipeline for highlighting
  Handle(asiVisu_FaceDomainPipeline) detect_pl = new asiVisu_FaceDomainPipeline(false);

  // Adjust props
  detect_pl->Actor()->GetProperty()->SetColor(detect_color[0], detect_color[1], detect_color[2]);
  detect_pl->Actor()->GetProperty()->SetLineWidth( asiVisu_Utils::DefaultDetectionLineWidth() + 1 );
  detect_pl->Actor()->GetProperty()->SetPointSize( asiVisu_Utils::DefaultHilightPointSize() );
  detect_pl->Actor()->SetPickable(0);
  detect_pl->Mapper()->ScalarVisibilityOff();
  //
  detect_pl->Actor()->GetProperty()->SetOpacity(1.0);
  detect_pl->Actor()->GetProperty()->SetAmbient(0.8);
  detect_pl->Actor()->GetProperty()->SetDiffuse(0.2);
  detect_pl->Actor()->GetProperty()->SetSpecular(0.9);
  detect_pl->Actor()->GetProperty()->SetSpecularPower(20);
  //
  detect_pl->Actor()->SetVisibility(0);

  // Bind to the data provider
  this->installDetectPipeline( detect_pl, DP );

  /* ========================
   *  Pipeline for selection
   * ======================== */

  double sel_color[3];
  asiVisu_Utils::DefaultPickingColor(sel_color[0], sel_color[1], sel_color[2]);

  // Create pipeline for selection
  Handle(asiVisu_FaceDomainPipeline) sel_pl = new asiVisu_FaceDomainPipeline(false);

  // Adjust props
  sel_pl->Actor()->GetProperty()->SetColor(sel_color[0], sel_color[1], sel_color[2]);
  sel_pl->Actor()->GetProperty()->SetLineWidth( asiVisu_Utils::DefaultPickLineWidth() + 1 );
  sel_pl->Actor()->GetProperty()->SetPointSize( asiVisu_Utils::DefaultHilightPointSize() );
  sel_pl->Actor()->SetPickable(0);
  sel_pl->Mapper()->ScalarVisibilityOff();
  //
  sel_pl->Actor()->GetProperty()->SetOpacity(1.0);
  sel_pl->Actor()->GetProperty()->SetAmbient(0.8);
  sel_pl->Actor()->GetProperty()->SetDiffuse(0.2);
  sel_pl->Actor()->GetProperty()->SetSpecular(0.9);
  sel_pl->Actor()->GetProperty()->SetSpecularPower(20);
  //
  sel_pl->Actor()->SetVisibility(0);

  // Bind to the data provider
  this->installPickPipeline( sel_pl, DP );
}

//! Factory method for Presentation.
//! \param theNode [in] Face Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_GeomFacePrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_GeomFacePrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomFacePrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomFacePrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomFacePrs::afterInitPipelines()
{
  Handle(asiVisu_FaceDataProvider)
    DP = Handle(asiVisu_FaceDataProvider)::DownCast( this->dataProvider(Pipeline_Main) );

  // Get working face
  const int   F_idx = DP->GetFaceIndexAmongFaces();
  TopoDS_Face F     = DP->ExtractFace();

  // Prepare main title
  TCollection_AsciiString TITLE("Face (#");
  TITLE += F_idx; TITLE += "): ";
  TITLE += asiAlgo_Utils::OrientationToString(F);

  // Add orientation of all wires
  int wire_idx = 0;
  for ( TopExp_Explorer exp(F, TopAbs_WIRE); exp.More(); exp.Next() )
  {
    ++wire_idx;
    const TopoDS_Wire& W = TopoDS::Wire( exp.Current() );

    TITLE += "\n";
    TITLE += "Wire (#";
    TITLE += wire_idx; TITLE += "): ";
    TITLE += asiAlgo_Utils::OrientationToString(W);
  }

  // Take location
  if ( !F.IsNull() )
  {
    TopLoc_Location loc;
    Handle(Geom_Surface) surf = BRep_Tool::Surface(F, loc);
    //
    const gp_Trsf& T      = loc.Transformation();
    gp_Mat         T_roto = T.VectorialPart();
    const gp_XYZ&  T_move = T.TranslationPart();
    gp_TrsfForm    T_form = T.Form();
    //
    TITLE += "\n---\nTransformation: ";
    if ( T_form == gp_Identity )
      TITLE += "identity";
    else if ( T_form == gp_Rotation )
      TITLE += "rotation";
    else if ( T_form == gp_Translation )
      TITLE += "translation";
    else if ( T_form == gp_PntMirror )
      TITLE += "point mirror (central symmetry)";
    else if ( T_form == gp_Ax1Mirror )
      TITLE += "axis mirror (rotational symmetry)";
    else if ( T_form == gp_Ax2Mirror )
      TITLE += "plane mirror (bilateral symmetry)";
    else if ( T_form == gp_Scale )
      TITLE += "scaling";
    else if ( T_form == gp_CompoundTrsf )
      TITLE += "combination of orthogonal transformations";
    else
      TITLE += "non-orthogonal transformation";
    //
    TITLE += "\n---\n";
    TITLE += T_roto(1, 1); TITLE += " "; TITLE += T_roto(1, 2); TITLE += " "; TITLE += T_roto(1, 3); TITLE += "\n";
    TITLE += T_roto(2, 1); TITLE += " "; TITLE += T_roto(2, 2); TITLE += " "; TITLE += T_roto(2, 3); TITLE += "\n";
    TITLE += T_roto(3, 1); TITLE += " "; TITLE += T_roto(3, 2); TITLE += " "; TITLE += T_roto(3, 3);
    TITLE += "\n---\n";
    TITLE += T_move.X(); TITLE += " "; TITLE += T_move.Y(); TITLE += " "; TITLE += T_move.Z();
  }

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );

  //---------------------------------------------------------------------------
  // Detection and highlighting
  //---------------------------------------------------------------------------

  // Access pipelines dedicated for highlighting
  const Handle(asiVisu_FaceDomainPipeline)&
    detect_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetDetectPipeline() );
  const Handle(asiVisu_FaceDomainPipeline)&
    pick_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetPickPipeline() );

  const Handle(asiVisu_FaceDataProvider)&
    detect_dp = Handle(asiVisu_FaceDataProvider)::DownCast( this->dataProviderDetect() );
  const Handle(asiVisu_FaceDataProvider)&
    pick_dp = Handle(asiVisu_FaceDataProvider)::DownCast( this->dataProviderPick() );

  // Init PL as selection pipelines are not automated
  detect_pl->SetInput(detect_dp);
  pick_pl->SetInput(pick_dp);
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_GeomFacePrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomFacePrs::afterUpdatePipelines() const
{
  /* ====================================
   *  Update selection pipelines as well
   * ==================================== */

  // Access pipelines dedicated for highlighting
  const Handle(asiVisu_FaceDomainPipeline)&
    detect_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetDetectPipeline() );
  const Handle(asiVisu_FaceDomainPipeline)&
    pick_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetPickPipeline() );

  // IMPORTANT: We update our highlighting pipelines here just to make things
  // faster. The better place to do that is "highlight" method, because
  // we do not really need to build highlighting pipelines just after
  // the Nodal Presentation is created. Logically, we would better to prepare
  // this pipeline only on actual selection request from user. However, in the
  // latter case the reactivity of application might significantly slow down
  detect_pl->Update();
  pick_pl->Update();
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void asiVisu_GeomFacePrs::highlight(vtkRenderer*                  theRenderer,
                                    const asiVisu_PickResult&     thePickRes,
                                    const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);

  //// Get target actor which is the only sensitive
  //Handle(asiVisu_Pipeline) poles_pl = this->GetPipeline(Pipeline_Main);
  ////
  //vtkActor* poles_actor = poles_pl->Actor();

  //// Get the list of picked cell IDs
  //TColStd_PackedMapOfInteger cellIds;
  //if ( thePickRes.IsSelectionWorkpiece() )
  //{
  //  const asiVisu_ActorElemMap& pickMap = thePickRes.GetPickMap();
  //  for ( asiVisu_ActorElemMap::Iterator it(pickMap); it.More(); it.Next() )
  //  {
  //    const vtkSmartPointer<vtkActor>& aResActor = it.Key();
  //    if ( aResActor != poles_actor )
  //      continue;

  //    cellIds = it.Value();
  //  }
  //}

  ////---------------------------------------------------------------------------
  //// Update highlighting pipelines
  ////---------------------------------------------------------------------------

  //// Access pipeline for highlighting
  //Handle(asiVisu_FaceDomainPipeline) hili_pl;
  ////
  //if ( theSelNature == SelectionNature_Pick )
  //  hili_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetPickPipeline() );
  //else
  //  hili_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetDetectPipeline() );

  //if ( !hili_pl )
  //  return;

  //// Set selection mask...
  //hili_pl->SetSelectedCells(cellIds);
  //hili_pl->ForceExecution();
  //hili_pl->SetInput( this->dataProviderDetect() );

  //// ... and visibility
  //hili_pl->Actor()->SetVisibility( !cellIds.IsEmpty() );
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_GeomFacePrs::unHighlight(vtkRenderer*                  theRenderer,
                                      const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);

  //// Access pipeline for highlighting
  //Handle(asiVisu_FaceDomainPipeline) hili_pl;
  ////
  //if ( theSelNature == SelectionNature_Pick )
  //  hili_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetPickPipeline() );
  //else
  //  hili_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetDetectPipeline() );

  //if ( !hili_pl )
  //  return;

  //// Set selection mask...
  //hili_pl->SetSelectedCells( TColStd_PackedMapOfInteger() );

  //// ... and visibility
  //hili_pl->Actor()->SetVisibility(0);
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomFacePrs::renderPipelines(vtkRenderer* theRenderer) const
{
  if ( !m_textWidget->GetCurrentRenderer() )
  {
    m_textWidget->SetInteractor      ( theRenderer->GetRenderWindow()->GetInteractor() );
    m_textWidget->SetDefaultRenderer ( theRenderer );
    m_textWidget->SetCurrentRenderer ( theRenderer );
    m_textWidget->On                 ( );
  }

  //---------------------------------------------------------------------------
  // Highlighting
  //---------------------------------------------------------------------------

  Handle(asiVisu_FaceDomainPipeline)
    detect_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetDetectPipeline() ),
    pick_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetPickPipeline() );

  // Picking pipeline must be added to renderer the LAST (!). Otherwise
  // we experience some strange coloring bug because of their coincidence
  /* (1) */ detect_pl->AddToRenderer(theRenderer);
  /* (2) */ pick_pl->AddToRenderer(theRenderer);
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomFacePrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  m_textWidget->Off();

  //---------------------------------------------------------------------------
  // Highlighting
  //---------------------------------------------------------------------------

  Handle(asiVisu_FaceDomainPipeline)
    detect_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetDetectPipeline() ),
    pick_pl = Handle(asiVisu_FaceDomainPipeline)::DownCast( this->GetPickPipeline() );
  //
  detect_pl->RemoveFromRenderer(theRenderer);
  pick_pl->RemoveFromRenderer(theRenderer);
}
