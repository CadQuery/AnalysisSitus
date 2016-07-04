//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_contour_capture.h>

// Common includes
#include <geom_reapprox_contour.h>
#include <geom_utils.h>

// Geometry includes
#include <geom_check_contour.h>
#include <geom_check_shell.h>
#include <geom_classify_contour_shell.h>
#include <geom_fix_small_contour.h>
#include <geom_min_shell.h>
#include <geom_pick_shell.h>
#include <geom_project_edges_faces.h>
#include <geom_refine_tolerance.h>
#include <geom_split_edges.h>
#include <geom_split_faces.h>

// OCCT includes
#include <BOPAlgo_PaveFiller.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <NCollection_IncAllocator.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

#define STOPHERE return false;

//! Default constructor.
geom_contour_capture::geom_contour_capture()
{}

//! Initializes the algorithm with the working shape.
//! \param Body [in] working shape.
void geom_contour_capture::Init(const TopoDS_Shape& Body)
{
  m_body = Body;
}

//! Runs the capturing algorithm.
//! \param Contour            [in]     contour to capture.
//! \param tolerance          [in]     tolerance of the input geometry.
//! \param doSimplifyContour  [in]     indicates whether to simplify contour.
//! \param Result             [out]    captured shape.
//! \param ResultContour      [out]    resulting contour (possibly healed).
//! \param ContourCenter      [out]    center point of the contour.
//! \param ContourOrientation [out]    contour orientation vector.
//! \param Journal            [in/out] journal to collect messages and anomalies.
//! \return true in case of success, false -- otherwise.
bool geom_contour_capture::operator()(const TopoDS_Shape&  Contour,
                                      const double         tolerance,
                                      const bool           doSimplifyContour,
                                      TopoDS_Shell&        Result,
                                      TopoDS_Shape&        ResultContour,
                                      gp_Pnt&              ContourCenter,
                                      gp_Vec&              ContourOrientation,
                                      ActAPI_ProgressEntry Journal)
{
  /* ======================
   *  Stage 0: preparation
   * ====================== */

  // Null check for body
  if ( m_body.IsNull() )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.BodyIsNotInitialized" );
    return false; // Failure
  }

  // Null check for contour
  if ( Contour.IsNull() )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.ContourIsNotInitialized" );
    return false; // Failure
  }

  // Get base shell
  if ( m_body.ShapeType() != TopAbs_SHELL )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.BodyIsNotShell" << m_body );
    return false; // Failure
  }
  TopoDS_Shell Shell = TopoDS::Shell(m_body);

  // Get contour wire
  if ( Contour.ShapeType() != TopAbs_WIRE )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.ContourIsNotWire" << Contour );
    return false; // Failure
  }
  TopoDS_Wire ContourWire = TopoDS::Wire(Contour);

  // Tools
  geom_check_contour CheckContour;
  geom_fix_small_contour FixSmallContour;

  /* ======================================================
   *  Stage 1: simplify contour and heal self-interactions
   * ====================================================== */

#if defined DRAW_DEBUG
  std::cout << "\n[>>>] Stage 1.1: contour pre-processing" << std::endl;

  int cout_nEdges_original = 0;
  for ( TopExp_Explorer exp(ContourWire, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    cout_nEdges_original++;
  }
  std::cout << "Outer contour contains " << cout_nEdges_original << " edge(s)" << std::endl;

  TIMER_NEW
  TIMER_GO
#endif

  // Run healing
  if ( doSimplifyContour )
  {
    geom_reapprox_contour ReapproxContour(ContourWire, tolerance*100);

    TopoDS_Wire ReapproxWire;
    if ( ReapproxContour(ReapproxWire, true, true, Journal) )
    {
      ContourWire        = ReapproxWire;
      ContourCenter      = ReapproxContour.Center();
      ContourOrientation = ReapproxContour.Orientation();
    }
    else
      Journal.SendLogMessage( LogWarn(Normal) << "geom_contour_capture.CannotHealContour" << Contour );
  }

#if defined DRAW_DEBUG
  DBRep::Set("cc", ContourWire);
#endif

  // Send a warning if the input contour is not closed
  if ( !CheckContour.Check_connectedWire(ContourWire, false, tolerance, Journal) )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_contour_capture.ContourIsNotClosed" << Contour );
  }

#if defined DRAW_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT

  int cout_nEdges_refined = 0;
  for ( TopExp_Explorer exp(ContourWire, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    cout_nEdges_refined++;
  }
  std::cout << "Refined outer contour contains " << cout_nEdges_refined << " edge(s)" << std::endl;

  std::cout << "\n[>>>] Stage 1.2: extracting min shell" << std::endl;
  TIMER_RESET
  TIMER_GO
#endif

  // Check contour
  if ( !CheckContour.Check_isSingleWire(ContourWire, Journal)  )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.CannotProceedWithBadContour" << ContourWire );
    return false; // Failure
  }

  // Extract min shell
  TopoDS_Shell TentativeShell;
  geom_min_shell MinShell(tolerance);
  if ( !MinShell(Shell, Contour, TentativeShell, Journal) )
  {
    // NOTICE: even if MinShell() returns false, it still populates
    //         TentativeShell variable with the best result it could
    Journal.SendLogMessage( LogWarn(Normal) << "geom_contour_capture.CannotExtractMinShell" );
  }

  // Check shell. Invalidity of the tentative shell is a severe sign that
  // input topology has to be pre-processed before one may ask for a plate base
  geom_check_shell CheckShell(TentativeShell);
  if ( !CheckShell.Check_everything(tolerance, Journal) )
  {
    Journal.SendLogMessage( LogWarn(Normal) << "geom_contour_capture.TentativeShellIsInvalid" << TentativeShell );
  }

#if defined DRAW_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT
  DBRep::Set("min_shell", TentativeShell);
#endif

  /* =====================================
   *  Stage 2: do Requicha classification
   * ===================================== */

#if defined DRAW_DEBUG
  std::cout << "\n[>>>] Stage 2.1: Requicha classification" << std::endl;
  TIMER_RESET
  TIMER_GO
#endif

  // Prepare tool
  geom_classify_contour_shell ClassifyCS(TentativeShell, tolerance*10);
  TSlicerMap Slicers, NewSlicers;
  geom_intersection_points_edges InterPts;

  // Classify
  bool badRequicha = !ClassifyCS(ContourWire, Slicers, InterPts, Journal);
  bool isImmediateOk = false;

#if defined DRAW_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT

  // Accumulate all intersection points
  TopoDS_Compound InterPtsComp;
  BRep_Builder().MakeCompound(InterPtsComp);
  for ( int p_idx = 1; p_idx <= InterPts.Length(); ++p_idx )
  {
    const geom_intersection_point_edges& p = InterPts(p_idx);
    BRep_Builder().Add( InterPtsComp, BRepBuilderAPI_MakeVertex(p.P) );
  }
  DBRep::Set("inter_pts", InterPtsComp);

  // Accumulate all slicers
  TopoDS_Compound InterEdgesComp;
  BRep_Builder().MakeCompound(InterEdgesComp);
  for ( TSlicerMap::Iterator it(Slicers); it.More(); it.Next() )
  {
    const Handle(TopTools_HSequenceOfShape)& F_slicers = it.Value();
    if ( F_slicers.IsNull() )
      continue;

    for ( int k = 1; k <= F_slicers->Length(); ++k )
    {
      BRep_Builder().Add( InterEdgesComp, F_slicers->Value(k) );
    }
  }
  DBRep::Set("inter_edges", InterEdgesComp);
#endif

  TopoDS_Shape ImmersedBody, ImmersedContour, ProjContour, ProjSlicers;

  if ( !badRequicha )
  {
#if defined DRAW_DEBUG
    std::cout << "\n[>>>] Stage 2.2: trying immediate splitting (split & walk)" << std::endl;
    TIMER_RESET
    TIMER_GO
#endif

    // Split face boundary
    TopoDS_Compound PreparedFaces;
    geom_split_edges SplitEdges(tolerance);
    SplitEdges(InterPts, Slicers, TentativeShell, TentativeShell,
               PreparedFaces, NewSlicers, Journal);

    // Split faces
    geom_split_faces SplitFaces(tolerance);
    isImmediateOk = SplitFaces(NewSlicers, PreparedFaces, TentativeShell,
                               ImmersedBody, ImmersedContour, Journal);

#if defined DRAW_DEBUG
    TIMER_FINISH
    TIMER_COUT_RESULT

    DBRep::Set("res_body", ImmersedBody);
    DBRep::Set("res_contour", ImmersedContour);
#endif

    if ( !isImmediateOk )
    {
#if defined DRAW_DEBUG
      std::cout << "\n[>>>] Stage 2.3: projecting Requicha" << std::endl;
      TIMER_RESET
      TIMER_GO
#endif

      Journal.SendLogMessage( LogInfo(Normal) << "geom_contour_capture.ImmediateModeFailed" );

      // Project slicers to their faces for better accuracy
      geom_project_edges_faces ProjEsFs;
      ProjEsFs(Slicers, 1.0, ProjContour, ProjSlicers, Journal);

      // Check the projection result
      if ( ProjContour.IsNull() )
      {
        Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.CannotProjectRequichaSlicers" );
        return false; // Failure
      }

      // Check contour
      badRequicha = !CheckContour.Check_isSingleWire(ProjContour, Journal);
      if ( badRequicha )
      {
        ProjContour = ContourWire;

        Journal.SendLogMessage( LogWarn(Normal) << "geom_contour_capture.BadProjContour" << ProjContour );
        Journal.SendLogMessage( LogInfo(Normal) << "geom_contour_capture.TryUsingNonProjectedContour" );

        // NOTE: we continue algorithm with highest hopes that General Fuse will
        //       correctly work on non-projected slices and with increased
        //       tolerance (fuzzy value)
      }
#if defined DRAW_DEBUG
      else
      {
        DBRep::Set("ccc", ProjContour);
      }
#endif

      // IMPORTANT: once the contour is projected, we do not modify it anymore
      //            (e.g. we do not attempt to remove "small" edges) as now all
      //            obtained vertices lie at boundaries facilitating General
      //            Fuse. If we affect the contour we will make slicers worse
      //
      // TODO: the following contradicts:

      TopoDS_Wire FixedContour;
      if ( FixSmallContour(ProjContour, tolerance*1.0e4, FixedContour, Journal) )
        ProjContour = FixedContour;

#if defined DRAW_DEBUG
      TIMER_FINISH
      TIMER_COUT_RESULT
#endif
    }
  }
  else // Requicha classification failed
  {
    ProjContour = ContourWire;

    Journal.SendLogMessage( LogWarn(Normal) << "geom_contour_capture.CannotClassifyRequicha" );
    Journal.SendLogMessage( LogInfo(Normal) <<  "geom_contour_capture.TryGeneralFuse" );
  }

  /* =========================================
   *  Stage 3: run General Fuse for immersing
   * ========================================= */

  if ( !isImmediateOk )
  {
#if defined DRAW_DEBUG
    std::cout << "\n[>>>] Stage 3: General Fuse" << std::endl;
    TIMER_RESET
    TIMER_GO
#endif

    // Note how much General Fuse tolerance is greater than the original one. This
    // is seldom justified, but we still have a good argument to proceed like
    // this. The matter is that if General Fuse is launched, it normally
    // means that Requicha classification has failed. But Requicha classification
    // works quite well in cases when no singularities exist. Therefore, if
    // General Fuse is called, it most likely means that we were not able to
    // escape from some singularity or to process it. Having this situation,
    // we slack the requirement to the General Fuse in order for it to have
    // big enough tolerance to process these singularities
    try
    {
      if ( !this->generalFuse(TentativeShell, ProjContour, tolerance*10, ImmersedBody, ImmersedContour) )
      {
        Journal.SendLogMessage( LogWarn(Normal) << "geom_contour_capture.CannotGFuseDefaultTolerance" );

        // Keep trying to fuse with increased fuzzy value (tolerance)
        if ( !this->generalFuse(TentativeShell, ProjContour, tolerance*20, ImmersedBody, ImmersedContour) )
        {
          Journal.SendLogMessage( LogWarn(Normal) << "geom_contour_capture.CannotGFuseDoubleTolerance" );

          // Keep trying to fuse with increased fuzzy value (tolerance)
          if ( !this->generalFuse(TentativeShell, Contour, tolerance*40, ImmersedBody, ImmersedContour) )
          {
            Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.CannotGFuseOriginalContour" );
            return false;
          }
        }
      }
    }
    catch ( ... )
    {
      Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.GeneralFuseCrashed" );
      return false;
    }

#if defined DRAW_DEBUG
    TIMER_FINISH
    TIMER_COUT_RESULT
    DBRep::Set("fused", ImmersedBody);
    DBRep::Set("fused_ccc", ImmersedContour);
#endif
  }

  /* ================================
   *  Stage 4: capture the sub-shell
   * ================================ */

#if defined DRAW_DEBUG
  std::cout << "\n[>>>] Stage 4: Capturing" << std::endl;
  TIMER_RESET
  TIMER_GO
#endif

  // Capture sub-shell. Notice that the middle point is calculated with
  // respect to original contour to have a proper middle (the immersed contour
  // may appear to be significantly trimmed and even not closed)
  geom_pick_shell Capture(tolerance, tolerance);
  Capture(ImmersedBody, ImmersedContour,
          geom_pick_shell::MidPoint(ImmersedContour, 100.0), // [mm], does not depend on tolerance
          Result, Journal);

  // Check result
  if ( Result.IsNull() )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.CannotPickShell" );
    return false; // Failure
  }

#if defined DRAW_DEBUG
  TIMER_FINISH
  TIMER_COUT_RESULT
#endif

  /* ========================================================
   *  Stage 5: run shape healing (it may help in some cases)
   * ======================================================== */

  ShapeFix_Shape ShapeHealer(Result);
  ShapeHealer.FixWireTool()->FixDegeneratedMode() = 0;
  try
  {
    ShapeHealer.Perform();
  }
  catch ( ... )
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.CannotFixShape" );
  }
  TopoDS_Shape ResultSh = ShapeHealer.Shape();

  if ( ResultSh.ShapeType() == TopAbs_SHELL )
    Result = TopoDS::Shell(ResultSh);
  else
  {
    Journal.SendLogMessage( LogErr(Normal) << "geom_contour_capture.FixShapeNotShell" );
    return false;
  }

  /* ===========================
   *  Stage 6: refine tolerance
   * =========================== */

  // Refine tolerance
  geom_refine_tolerance RefineToler;
  if ( !RefineToler.RebuildBounds(Result, Journal) )
    return false;
  if ( !RefineToler.Degrade(Result, Precision::Confusion(), Journal) )
    return false;

#if defined DRAW_DEBUG
  DBRep::Set("cc_result", Result);
#endif

  // Save working contour
  ResultContour = ImmersedContour;
  return true; // Success
}

//! Performs general fuse on the given arguments.
//! \param Body          [in]  body shell.
//! \param Contour       [in]  contour to fuse into the body shell.
//! \param fuzz          [in]  fuzzy value to use.
//! \param ResultBody    [out] fused shape.
//! \param ResultContour [out] contour image after general fuse.
//! \return true in case of success, false -- otherwise.
bool geom_contour_capture::generalFuse(const TopoDS_Shape& Body,
                                       const TopoDS_Shape& Contour,
                                       const double        fuzz,
                                       TopoDS_Shape&       ResultBody,
                                       TopoDS_Shape&       ResultContour) const
{
  Handle(NCollection_BaseAllocator) Alloc = new NCollection_IncAllocator;

  // Prepare the arguments
  TopTools_ListOfShape BOP_args;
  BOP_args.Append(Contour);
  BOP_args.Append(Body);

  const int bRunParallel = 0;

  BOPAlgo_PaveFiller DSFiller(Alloc);
  DSFiller.SetArguments(BOP_args);
  DSFiller.SetRunParallel(bRunParallel);
  DSFiller.SetFuzzyValue(fuzz);
  DSFiller.Perform();
  int iErr = DSFiller.ErrorStatus();
  if ( iErr )
  {
    return false;
  }

  BOPAlgo_Builder BOP(Alloc);
  BOP.SetArguments(BOP_args);
  BOP.SetRunParallel(bRunParallel);
  BOP.PerformWithFiller(DSFiller);
  iErr = BOP.ErrorStatus();
  if ( iErr )
  {
    return false;
  }

  // Set results
  ResultBody    = BOP.Shape();
  ResultContour = this->updateContour(BOP, Contour);
  return true;
}

//! Attempts to update the passed contour shape from the history of BOP.
//! Internally it iterates over the source-image map trying to find a source
//! with TShape pointer identical to the passed one. Then it takes its image
//! as a result. If nothing was found, the passed argument is returned as a
//! result for convenience.
//! \param BOP     [in] container of history.
//! \param Contour [in] original contour.
//! \return updated contour or the original one if nothing "better" was
//!         found.
TopoDS_Shape geom_contour_capture::updateContour(BOPAlgo_Builder&    BOP,
                                                 const TopoDS_Shape& Contour) const
{
  const BOPCol_DataMapOfShapeListOfShape& images = BOP.Images();
  for ( BOPCol_DataMapIteratorOfDataMapOfShapeListOfShape it(images); it.More(); it.Next() )
  {
    const TopoDS_Shape& key = it.Key();
    if ( !key.IsPartner(Contour) )
      continue;

    const BOPCol_ListOfShape& images = it.Value();

    if ( images.Extent() == 1 )
      return images.First();

    if ( images.Extent() )
    {
      TopoDS_Compound C;
      BRep_Builder().MakeCompound(C);

      for ( BOPCol_ListIteratorOfListOfShape lit(images); lit.More(); lit.Next() )
      {
        const TopoDS_Shape& image = lit.Value();
        BRep_Builder().Add(C, image);
      }

      return C;
    }
  }
  return Contour;
}
