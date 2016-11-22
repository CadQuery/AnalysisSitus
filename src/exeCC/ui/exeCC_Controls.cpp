//-----------------------------------------------------------------------------
// Created on: 16 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeCC_Controls.h>

// exeCC includes
#include <exeCC_CommonFacilities.h>
#include <exeCC_CreateContourCallback.h>
#include <exeCC_DialogBuildOffsets.h>
#include <exeCC_DialogCapture.h>
#include <exeCC_DialogEdgesOverlapping.h>
#include <exeCC_DialogEnrich.h>
#include <exeCC_DialogHealing.h>

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>
#include <asiAlgo_BVHIterator.h>
#include <asiAlgo_HitFacet.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_PickFacetCallback.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiVisu includes
#include <asiVisu_GeomPrs.h>

// VTK includes
#include <vtkActor.h>
#include <vtkProperty.h>

// Qt includes
#include <QGroupBox>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

// SPE includes
#include <CC_ClassifyPointFace.h>
#include <CC_Validate.h>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
exeCC_Controls::exeCC_Controls(QWidget* parent) : QWidget(parent), m_iPrevSelMode(0)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pPickContour          = new QPushButton("Pick contour");
  m_widgets.pLoadContour          = new QPushButton("Import contour");
  m_widgets.pSaveContour          = new QPushButton("Save contour");
  m_widgets.pCheckContourDistance = new QPushButton("Check contour-shell distance");
  m_widgets.pCheckVertexDistance  = new QPushButton("Check distance at poles");
  m_widgets.pHealedVSOriginal     = new QPushButton("Check healed-original distance");
  m_widgets.pProjectVertices      = new QPushButton("Project poles to body");
  m_widgets.pEnrichContour        = new QPushButton("Enrich contour");
  m_widgets.pHealContour          = new QPushButton("Heal contour");
  m_widgets.pCapture              = new QPushButton("Capture");
  m_widgets.pValidateResult       = new QPushButton("Validate result");
  m_widgets.pBVH_SAH              = new QPushButton("BVH [SAH]");
  m_widgets.pBVH_Linear           = new QPushButton("BVH [linear]");
  m_widgets.pPickFacet            = new QPushButton("Pick facet");
  m_widgets.pPutInCompound        = new QPushButton("Put in compound");
  m_widgets.pCheckOverlapping     = new QPushButton("Check overlapping");
  m_widgets.pBuildOffsets         = new QPushButton("Build offsets");
  //
  m_widgets.pPickContour          -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadContour          -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSaveContour          -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCheckContourDistance -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCheckVertexDistance  -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pHealedVSOriginal     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pProjectVertices      -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pEnrichContour        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pHealContour          -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCapture              -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pValidateResult       -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pBVH_SAH              -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pBVH_Linear           -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pPickFacet            -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pPutInCompound        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pCheckOverlapping     -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pBuildOffsets         -> setMinimumWidth(BTN_MIN_WIDTH);

  // Other configurations
  m_widgets.pPickContour  ->setCheckable(true);
  m_widgets.pPickFacet    ->setCheckable(true);

  // Group of buttons for contour definition
  QGroupBox*   pContourGroup = new QGroupBox("Contour geometry");
  QVBoxLayout* pContourLay   = new QVBoxLayout(pContourGroup);
  //
  pContourLay->addWidget(m_widgets.pPickContour);
  pContourLay->addWidget(m_widgets.pLoadContour);
  pContourLay->addWidget(m_widgets.pSaveContour);

  // Group of buttons for contour validation
  QGroupBox*   pValidateGroup = new QGroupBox("Contour processing");
  QVBoxLayout* pValidateLay   = new QVBoxLayout(pValidateGroup);
  //
  pValidateLay->addWidget(m_widgets.pCheckContourDistance);
  pValidateLay->addWidget(m_widgets.pCheckVertexDistance);
  pValidateLay->addWidget(m_widgets.pHealedVSOriginal);
  pValidateLay->addWidget(m_widgets.pProjectVertices);
  pValidateLay->addWidget(m_widgets.pEnrichContour);
  pValidateLay->addWidget(m_widgets.pHealContour);

  // Group of buttons for contour capture
  QGroupBox*   pCaptureGroup = new QGroupBox("Contour capture");
  QVBoxLayout* pCaptureLay   = new QVBoxLayout(pCaptureGroup);
  //
  pCaptureLay->addWidget(m_widgets.pCapture);
  pCaptureLay->addWidget(m_widgets.pValidateResult);

  // Group of buttons for additional tests
  QGroupBox*   pAddendumGroup = new QGroupBox("Additional tools");
  QVBoxLayout* pAddendumLay   = new QVBoxLayout(pAddendumGroup);
  //
  pAddendumLay->addWidget(m_widgets.pPickFacet);
  pAddendumLay->addWidget(m_widgets.pBVH_SAH);
  pAddendumLay->addWidget(m_widgets.pBVH_Linear);
  pAddendumLay->addWidget(m_widgets.pPutInCompound);
  pAddendumLay->addWidget(m_widgets.pCheckOverlapping);

  // Group of buttons for plate modeling
  QGroupBox*   pPlateGroup = new QGroupBox("Plate");
  QVBoxLayout* pPlateLay   = new QVBoxLayout(pPlateGroup);
  //
  pPlateLay->addWidget(m_widgets.pBuildOffsets);

  // Set layout
  m_pMainLayout->addWidget(pContourGroup);
  m_pMainLayout->addWidget(pValidateGroup);
  m_pMainLayout->addWidget(pCaptureGroup);
  m_pMainLayout->addWidget(pAddendumGroup);
  m_pMainLayout->addWidget(pPlateGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pPickContour,          SIGNAL( clicked() ), SLOT( onPickContour          () ) );
  connect( m_widgets.pLoadContour,          SIGNAL( clicked() ), SLOT( onLoadContour          () ) );
  connect( m_widgets.pSaveContour,          SIGNAL( clicked() ), SLOT( onSaveContour          () ) );
  connect( m_widgets.pCheckContourDistance, SIGNAL( clicked() ), SLOT( onCheckContourDistance () ) );
  connect( m_widgets.pCheckVertexDistance,  SIGNAL( clicked() ), SLOT( onCheckVertexDistance  () ) );
  connect( m_widgets.pHealedVSOriginal,     SIGNAL( clicked() ), SLOT( onHealedVSOriginal     () ) );
  connect( m_widgets.pProjectVertices,      SIGNAL( clicked() ), SLOT( onProjectVertices      () ) );
  connect( m_widgets.pEnrichContour,        SIGNAL( clicked() ), SLOT( onEnrichContour        () ) );
  connect( m_widgets.pHealContour,          SIGNAL( clicked() ), SLOT( onHealContour          () ) );
  connect( m_widgets.pCapture,              SIGNAL( clicked() ), SLOT( onCapture              () ) );
  connect( m_widgets.pValidateResult,       SIGNAL( clicked() ), SLOT( onValidateResult       () ) );
  connect( m_widgets.pBVH_SAH,              SIGNAL( clicked() ), SLOT( onBVH_SAH              () ) );
  connect( m_widgets.pBVH_Linear,           SIGNAL( clicked() ), SLOT( onBVH_Linear           () ) );
  connect( m_widgets.pPickFacet,            SIGNAL( clicked() ), SLOT( onPickFacet            () ) );
  connect( m_widgets.pPutInCompound,        SIGNAL( clicked() ), SLOT( onPutInCompound        () ) );
  connect( m_widgets.pCheckOverlapping,     SIGNAL( clicked() ), SLOT( onCheckOverlapping     () ) );
  connect( m_widgets.pBuildOffsets,         SIGNAL( clicked() ), SLOT( onBuildOffsets         () ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
exeCC_Controls::~exeCC_Controls()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Allows user to pick a contour interactively.
void exeCC_Controls::onPickContour()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  const bool isOn = m_widgets.pPickContour->isChecked();

  // Create the accelerating structure
  if ( m_bvh.IsNull() || part != m_bvh->GetShape() )
    m_bvh = new asiAlgo_BVHFacets(part,
                                  asiAlgo_BVHFacets::Builder_Binned,
                                  cf->Notifier,
                                  cf->Plotter);

  // Get contour Node
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();

  // Depending on the state of the control, either let user to pick some
  // points on the shape or finalize contour creation
  if ( isOn )
  {
    // Clear contour
    model->OpenCommand();
    {
      contour_n->Init();
    }
    model->CommitCommand();

    // Enable an appropriate selection mode
    m_iPrevSelMode = cf->Prs.Part->GetSelectionMode();
    cf->Prs.Part->SetSelectionMode(SelectionMode_Workpiece);

    // Add observer which takes responsibility to fill the data object with
    // the base points of the contour
    if ( !cf->Prs.Part->HasObserver(EVENT_PICK_WORLD_POINT) )
    {
      vtkSmartPointer<exeCC_CreateContourCallback>
        cb = vtkSmartPointer<exeCC_CreateContourCallback>::New();
      //
      cb->SetBVH(m_bvh);

      // Add observer
      cf->Prs.Part->AddObserver(EVENT_PICK_WORLD_POINT, cb);
    }
  }
  else
  {
    if ( !contour_n->IsClosed() )
    {
      model->OpenCommand();
      {
        contour_n->SetClosed(true);
      }
      model->CommitCommand();
    }

    cf->Prs.Part->Actualize( contour_n.get() );

    // Restore original selection mode
    cf->Prs.Part->SetSelectionMode(m_iPrevSelMode);
    //
    cf->Prs.Part->RemoveObserver(EVENT_PICK_WORLD_POINT);
  }
}

//-----------------------------------------------------------------------------

//! Allows user to load an externally defined contour.
void exeCC_Controls::onLoadContour()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Open);

  // Read BREP
  TopoDS_Shape shape;
  if ( !asiAlgo_Utils::ReadBRep(QStr2AsciiStr(filename), shape) )
  {
    std::cout << "Error: cannot read b-rep file" << std::endl;
    return;
  }

  if ( shape.ShapeType() != TopAbs_WIRE )
  {
    std::cout << "Error: loaded shape is not a WIRE" << std::endl;
    return;
  }

  // Create the accelerating structure
  if ( m_bvh.IsNull() || part != m_bvh->GetShape() )
    m_bvh = new asiAlgo_BVHFacets(part,
                                  asiAlgo_BVHFacets::Builder_Binned,
                                  cf->Notifier,
                                  cf->Plotter);

  // Tool to find the owner face
  asiAlgo_HitFacet HitFacet(m_bvh,
                            cf->Notifier,
                            cf->Plotter);

  // Set geometry of contour
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  model->OpenCommand(); // tx start
  {
    // Clear contour
    contour_n->Init();

    // Iterate over the vertices to compose a contour data structure
    TopTools_IndexedMapOfShape traversed;
    int vidx = 1;
    //
    for ( TopExp_Explorer exp(shape, TopAbs_VERTEX); exp.More(); exp.Next(), ++vidx )
    {
      TopoDS_Vertex V = TopoDS::Vertex( exp.Current() );
      //
      if ( traversed.Contains(V) )
        continue;

      // Access the probe point
      traversed.Add(V);
      gp_Pnt P = BRep_Tool::Pnt(V);

      // Recover host face
      int facet_idx = -1;
      if ( !HitFacet(P, 1e-1, facet_idx) )
      {
        std::cout << "Cannot locate a host facet for a contour's point" << std::endl;
        cf->Plotter->DRAW_POINT(P, Color_Red, "Failure");
        continue;
      }

      // Add point to the contour
      contour_n->AddPoint( P.XYZ(), m_bvh->GetFacet(facet_idx).FaceIndex );
    }
    contour_n->SetClosed(true);
  }
  model->CommitCommand(); // tx commit

  // Actualize
  cf->Prs.Part->DeletePresentation( contour_n.get() );
  cf->Prs.Part->Actualize( contour_n.get() );
}

//-----------------------------------------------------------------------------

//! Saves contour to a BREP file.
void exeCC_Controls::onSaveContour()
{
  // Get contour Node
  Handle(asiData_ContourNode)
    contour_n = exeCC_CommonFacilities::Instance()->Model->GetPartNode()->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Save);

  // Save shape
  TopoDS_Wire contourShape = contour_n->AsShape(true);
  //
  if ( !asiAlgo_Utils::WriteBRep( contourShape, QStr2AsciiStr(filename) ) )
    std::cout << "Error: cannot save contour" << std::endl;
}

//-----------------------------------------------------------------------------

//! Checks distance from the contour to the host faces.
void exeCC_Controls::onCheckContourDistance()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Get contour Node
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  // Imperative viewer
  ActAPI_PlotterEntry IV(cf->Plotter);

  // Create the accelerating structure
  if ( m_bvh.IsNull() || part != m_bvh->GetShape() )
    m_bvh = new asiAlgo_BVHFacets(part,
                                  asiAlgo_BVHFacets::Builder_Binned,
                                  cf->Notifier,
                                  cf->Plotter);

  // Get contour wire
  TopoDS_Wire contourWire = contour_n->AsShape();

  // Linear precision
  const int nProbePts = 23; // hey, this number is a magic one in OpenCascade,
                            // so it should work!

  // Pick up sample points on the wire and find the nearest faces for them
  TColgp_SequenceOfPnt pts;
  //
  for ( TopExp_Explorer exp(contourWire, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    double f, l;
    BRep_Tool::Range(E, f, l);
    BRepAdaptor_Curve BAC(E);
    GCPnts_QuasiUniformAbscissa Defl(BAC, nProbePts, f, l);

    if ( !Defl.IsDone() )
      continue;

    if ( E.Orientation() == TopAbs_REVERSED )
    {
      for ( int pt_idx = Defl.NbPoints(); pt_idx >= 1; --pt_idx )
      {
        gp_Pnt P = BAC.Value( Defl.Parameter(pt_idx) );
        pts.Append(P);
      }
    }
    else
    {
      for ( int pt_idx = 1; pt_idx <= Defl.NbPoints(); ++pt_idx )
      {
        gp_Pnt P = BAC.Value( Defl.Parameter(pt_idx) );
        pts.Append(P);
      }
    }
  }

  std::cout << "Number of probe points: " << pts.Length() << std::endl;

  // Get all faces with indices
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(part, TopAbs_FACE, faces);

  // Prepare a tool to find the intersected facet
  asiAlgo_HitFacet HitFacet(m_bvh,
                            cf->Notifier,
                            cf->Plotter);

  double maxGap = 0.0;
  gp_Pnt maxGapPt;

  // For each point, find the nearest faces
  for ( TColgp_SequenceOfPnt::Iterator it(pts); it.More(); it.Next() )
  {
    const gp_Pnt& P = it.Value();

    // Recover host face
    int facet_idx = -1;
    if ( !HitFacet(P, 1e-1, facet_idx) )
      continue;

    // Get face index and face
    const int          face_idx = m_bvh->GetFacet(facet_idx).FaceIndex;
    const TopoDS_Face& face     = TopoDS::Face( faces(face_idx) );

    // Point-face classifier
    gp_Pnt2d UV;
    double gap;
    //
    CC_ClassifyPointFace ClassPtFace( face, BRep_Tool::Tolerance(face), Precision::Confusion() );
    //
    ClassPtFace(P, false, UV, gap, NULL);

    if ( gap > maxGap )
    {
      maxGap   = gap;
      maxGapPt = P;
    }
  }

  std::cout << "Max gap (C-B error): " << maxGap
            << " at (" << maxGapPt.X() << ", " << maxGapPt.Y() << ", " << maxGapPt.Z() << ")"
            << std::endl;
  //
  IV.DRAW_POINT(maxGapPt, Color_Violet);
}

//-----------------------------------------------------------------------------

//! Checks distance from the contour vertices to the host faces.
void exeCC_Controls::onCheckVertexDistance()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Get contour Node
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  // Imperative viewer
  ActAPI_PlotterEntry IV(cf->Plotter);

  // Get all faces with indices
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(part, TopAbs_FACE, faces);

  double maxGap = 0.0;
  gp_Pnt maxGapPt;

  // Get all points of the contour
  TColgp_SequenceOfPnt points;
  TColStd_SequenceOfInteger faceIndices;
  //
  contour_n->AsPointsOnFaces(points, faceIndices);
  //
  TColgp_SequenceOfPnt::Iterator pit(points);
  TColStd_SequenceOfInteger::Iterator fit(faceIndices);
  //
  for ( ; pit.More(); pit.Next(), fit.Next() )
  {
    const gp_Pnt& point    = pit.Value();
    const int     face_idx = fit.Value();

    // Get face
    const TopoDS_Face& face = TopoDS::Face( faces(face_idx) );

    // Point-face classifier
    gp_Pnt2d UV;
    double gap;
    //
    CC_ClassifyPointFace ClassPtFace( face, BRep_Tool::Tolerance(face), Precision::Confusion() );
    //
    ClassPtFace(point, false, UV, gap, NULL);

    if ( gap > maxGap )
    {
      maxGap   = gap;
      maxGapPt = point;
    }
  }

  std::cout << "Max gap is " << maxGap << std::endl;
  //
  IV.CLEAN();
  IV.DRAW_POINT(maxGapPt, Color_Violet);
}

//-----------------------------------------------------------------------------

//! Checks distance from the original contour to the healed one.
void exeCC_Controls::onHealedVSOriginal()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Get contour Node
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  // Get healed geometry
  TopoDS_Wire healedContour = TopoDS::Wire( contour_n->GetGeometry() );

  // Get original geometry
  TopoDS_Wire originalContour = contour_n->AsShape(false);

  // Compute Hausdorff distance
  const double H = CC_Validate::Compute_HausdorffDistance(healedContour, originalContour);
  //
  std::cout << "Hausdorff deviation (healed-original): " << H << std::endl;
}

//-----------------------------------------------------------------------------

//! Projects vertices of the contour to the part.
void exeCC_Controls::onProjectVertices()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Get contour Node
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  // Get all faces
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(part, TopAbs_FACE, faces);

  // Get all points of the contour
  TColgp_SequenceOfPnt points;
  TColStd_SequenceOfInteger faceIndices;
  //
  contour_n->AsPointsOnFaces(points, faceIndices);
  //
  TColgp_SequenceOfPnt::Iterator pit(points);
  TColStd_SequenceOfInteger::Iterator fit(faceIndices);

  // Perform projection point-by-point
  model->OpenCommand();
  {
    int pointIndex = 0;
    for ( ; pit.More(); pit.Next(), fit.Next(), ++pointIndex )
    {
      const gp_Pnt& point    = pit.Value();
      const int     face_idx = fit.Value();

      // Get face
      const TopoDS_Face& face = TopoDS::Face( faces(face_idx) );

      // Point-face classifier
      gp_Pnt2d UV;
      double gap;
      //
      CC_ClassifyPointFace ClassPtFace( face, BRep_Tool::Tolerance(face), Precision::Confusion() );
      //
      ClassPtFace(point, false, UV, gap, NULL);

      // Reconstruct a point from UV
      gp_Pnt newPole = BRep_Tool::Surface(face)->Value( UV.X(), UV.Y() );
      //
      contour_n->ReplacePoint(pointIndex, newPole);
    }
  }
  model->CommitCommand();

  // Actualize
  cf->Prs.Part->DeletePresentation( contour_n.get() );
  cf->Prs.Part->Actualize( contour_n.get() );
}

//-----------------------------------------------------------------------------

//! Enriches contour with additional points.
void exeCC_Controls::onEnrichContour()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Create the accelerating structure
  if ( m_bvh.IsNull() || part != m_bvh->GetShape() )
    m_bvh = new asiAlgo_BVHFacets(part,
                                  asiAlgo_BVHFacets::Builder_Binned,
                                  cf->Notifier,
                                  cf->Plotter);

  // Run dialog
  exeCC_DialogEnrich* wCE = new exeCC_DialogEnrich(m_bvh, this);
  wCE->show();
}

//-----------------------------------------------------------------------------

//! Performs contour healing.
void exeCC_Controls::onHealContour()
{
  // Run dialog
  exeCC_DialogHealing* wCH = new exeCC_DialogHealing(this);
  wCH->show();
}

//-----------------------------------------------------------------------------

//! Performs contour capturing.
void exeCC_Controls::onCapture()
{
  // Run dialog
  exeCC_DialogCapture* wCC = new exeCC_DialogCapture(this);
  wCC->show();
}

//-----------------------------------------------------------------------------

//! Performs validation of capture result.
void exeCC_Controls::onValidateResult()
{
  // Get part Node
  Handle(asiData_PartNode)
    part_n = exeCC_CommonFacilities::Instance()->Model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    std::cout << "Error: part is not defined" << std::endl;
    return;
  }

  // Get contour Node
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  // Get contour
  TopoDS_Shape contour = contour_n->AsShape();

  // Get part shape
  TopoDS_Shape capturedShape = part_n->GetShape();

  // Check gap. The check is performed on vertices. I.e. the vertices of
  // the reference contour are projected to the edges of the outer wire
  // of the extracted shape. Since we consider the original vertices as
  // "perfect" ones (we cannot say this for the interior edges which
  // normally do not lie on surface), our expectation is that a good
  // contour capture procedure will not move them significantly
  ShapeAnalysis_FreeBounds shellAnalyzer(capturedShape, 0.2);
  const TopoDS_Compound& outerWire = shellAnalyzer.GetClosedWires();
  //
  const double H = CC_Validate::Compute_HausdorffDistance(contour, outerWire);

  std::cout << "Deviation in vertices: " << H << std::endl;
}

//-----------------------------------------------------------------------------

//! Constructs BVH with SAH builder.
void exeCC_Controls::onBVH_SAH()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Create the accelerating structure
  m_bvh = new asiAlgo_BVHFacets(part,
                                asiAlgo_BVHFacets::Builder_Binned,
                                cf->Notifier,
                                cf->Plotter);

  // Draw
  cf->Plotter->CLEAN();
  m_bvh->Dump(cf->Plotter);
}

//-----------------------------------------------------------------------------

//! Constructs BVH with linear builder.
void exeCC_Controls::onBVH_Linear()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Create the accelerating structure
  m_bvh = new asiAlgo_BVHFacets(part,
                                asiAlgo_BVHFacets::Builder_Linear,
                                cf->Notifier,
                                cf->Plotter);

  // Draw
  cf->Plotter->CLEAN();
  m_bvh->Dump(cf->Plotter);
}

//-----------------------------------------------------------------------------

//! Allows user to pick a single facet.
void exeCC_Controls::onPickFacet()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Create the accelerating structure
  if ( m_bvh.IsNull() || part != m_bvh->GetShape() )
    m_bvh = new asiAlgo_BVHFacets(part,
                                  asiAlgo_BVHFacets::Builder_Binned,
                                  cf->Notifier,
                                  cf->Plotter);

  const bool isOn = m_widgets.pPickFacet->isChecked();

  // Depending on the state of the control, either let user to pick a
  // point on the shape or finalize picking
  if ( isOn )
  {
    // Enable an appropriate selection mode
    m_iPrevSelMode = cf->Prs.Part->GetSelectionMode();
    cf->Prs.Part->SetSelectionMode(SelectionMode_Workpiece);

    // Add observer which takes responsibility to interact with user
    if ( !cf->Prs.Part->HasObserver(EVENT_PICK_WORLD_POINT) )
    {
      vtkSmartPointer<asiUI_PickFacetCallback>
        cb = vtkSmartPointer<asiUI_PickFacetCallback>::New();
      //
      cb->SetBVH(m_bvh);

      // Add observer
      cf->Prs.Part->AddObserver(EVENT_PICK_WORLD_POINT, cb);
    }
  }
  else
  {
    // Restore original selection mode
    cf->Prs.Part->SetSelectionMode(m_iPrevSelMode);
    //
    cf->Prs.Part->RemoveObserver(EVENT_PICK_WORLD_POINT);
  }
}

//-----------------------------------------------------------------------------

//! Puts a part shape and a contour to a compound.
void exeCC_Controls::onPutInCompound()
{
  Handle(exeCC_CommonFacilities) cf    = exeCC_CommonFacilities::Instance();
  const Handle(asiEngine_Model)& model = cf->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Get contour Node
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  // Get contour
  TopoDS_Shape contour = contour_n->AsShape();

  // Make a compound of them
  TopoDS_Compound comp;
  BRep_Builder BB;
  BB.MakeCompound(comp);
  BB.Add(comp, part);
  BB.Add(comp, contour);

  // Store as a part and clean up the contour
  model->OpenCommand();
  {
    contour_n->Init();
    part_n->Init();
    //
    part_n->SetShape(comp);
  }
  model->CommitCommand();

  // Update UI
  cf->Prs.DeleteAll();
  cf->Prs.Part->InitializePickers();
  cf->Prs.Part->Actualize(part_n, false, true);
}

//-----------------------------------------------------------------------------

//! Allows user to check overlapping between two selected edges.
void exeCC_Controls::onCheckOverlapping()
{
  // Run dialog
  exeCC_DialogEdgesOverlapping* wEO = new exeCC_DialogEdgesOverlapping(this);
  wEO->show();
}

//-----------------------------------------------------------------------------

//! Builds offset surfaces.
void exeCC_Controls::onBuildOffsets()
{
  Handle(exeCC_CommonFacilities) cf = exeCC_CommonFacilities::Instance();
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(cf->Model, part_n, part) ) return;

  // A part should be a shell
  if ( part.ShapeType() != TopAbs_SHELL )
  {
    std::cout << "Part is not a shell" << std::endl;
    return;
  }

  // Initialize plate base
  SpeCore_Surface plateBase;
  plateBase.Geometry = TopoDS::Shell(part);

  // Create plate interface
  Handle(SpeCore_Plate) plate = new SpeCore_Plate;
  if ( !plate->Init(plateBase, NULL) )
  {
    std::cout << "Error: cannot initialize plate base" << std::endl;
    return;
  }

  // Run dialog
  exeCC_DialogBuildOffsets* wBO = new exeCC_DialogBuildOffsets(plate, this);
  wBO->show();
}
