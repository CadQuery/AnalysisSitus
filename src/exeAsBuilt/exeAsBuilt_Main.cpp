//-----------------------------------------------------------------------------
// Created on: 14 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#undef VTK_SAMPLE

#if !defined VTK_SAMPLE
// Windows includes
#include <Windows.h>

// exeAsBuilt includes
#include <exeAsBuilt_MainWindow.h>
#include <exeAsBuilt_Model.h>

// asiVisu includes
#include <asiVisu_CalculusLawPrs.h>
#include <asiVisu_GeomBoundaryEdgesPrs.h>
#include <asiVisu_GeomContourPrs.h>
#include <asiVisu_GeomCurvePrs.h>
#include <asiVisu_GeomEdgePrs.h>
#include <asiVisu_GeomFacePrs.h>
#include <asiVisu_GeomPrs.h>
#include <asiVisu_GeomSurfPrs.h>
#include <asiVisu_IVCurvePrs.h>
#include <asiVisu_IVPointSet2dPrs.h>
#include <asiVisu_IVPointSetPrs.h>
#include <asiVisu_IVSurfacePrs.h>
#include <asiVisu_IVTessItemPrs.h>
#include <asiVisu_IVTextItemPrs.h>
#include <asiVisu_IVTopoItemPrs.h>
#include <asiVisu_MeshPrs.h>
#include <asiVisu_REContoursPrs.h>
#include <asiVisu_REPointsPrs.h>
#include <asiVisu_RESurfacesPrs.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QMainWindow>
#pragma warning(pop)

// Qt-VTK includes
#include <QVTKWidget.h>

// VTK includes
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

// VTK init
#include <vtkAutoInit.h>

// OCCT includes
#include <OSD_Environment.hxx>

// Activate object factories
VTK_MODULE_INIT(vtkRenderingContextOpenGL2);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType)

//-----------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------

//! main().
int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  //
  TCollection_AsciiString ico_fn = OSD_Environment("AS_RESOURCES").Value();
  ico_fn += "/asitus_icon_16x16.png";
  QApplication::setWindowIcon( QIcon( ico_fn.ToCString() ) );

  //---------------------------------------------------------------------------
  // Register Presentations
  //---------------------------------------------------------------------------

  REGISTER_PRESENTATION(asiVisu_GeomPrs)
  REGISTER_PRESENTATION(asiVisu_GeomBoundaryEdgesPrs)
  REGISTER_PRESENTATION(asiVisu_GeomCurvePrs)
  REGISTER_PRESENTATION(asiVisu_GeomEdgePrs)
  REGISTER_PRESENTATION(asiVisu_GeomFacePrs)
  REGISTER_PRESENTATION(asiVisu_GeomSurfPrs)
  REGISTER_PRESENTATION(asiVisu_GeomContourPrs)
  REGISTER_PRESENTATION(asiVisu_CalculusLawPrs)

  // Imperative viewer
  REGISTER_PRESENTATION(asiVisu_IVPointSet2dPrs)
  REGISTER_PRESENTATION(asiVisu_IVPointSetPrs)
  REGISTER_PRESENTATION(asiVisu_IVCurvePrs)
  REGISTER_PRESENTATION(asiVisu_IVSurfacePrs)
  REGISTER_PRESENTATION(asiVisu_IVTessItemPrs)
  REGISTER_PRESENTATION(asiVisu_IVTextItemPrs)
  REGISTER_PRESENTATION(asiVisu_IVTopoItemPrs)

  // Reverse engineering
  REGISTER_PRESENTATION(asiVisu_REPointsPrs)
  REGISTER_PRESENTATION(asiVisu_REContoursPrs)
  REGISTER_PRESENTATION(asiVisu_RESurfacesPrs)

  //---------------------------------------------------------------------------
  // Create main window
  //---------------------------------------------------------------------------

  exeAsBuilt_MainWindow* pMainWindow = new exeAsBuilt_MainWindow;
  //
  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  const int center_x   = ( screenGeometry.width() - pMainWindow->width() ) / 2;
  const int center_y   = ( screenGeometry.height() - pMainWindow->height() ) / 2;
  //
  pMainWindow->move(center_x/2, center_y/2); // Move to a handy position
  pMainWindow->show();

  //---------------------------------------------------------------------------
  // Run event loop
  //---------------------------------------------------------------------------

  return app.exec();
}
#else

// VTK includes
#include <vtkActor.h>
#include <vtkAxes.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkPlaneWidget.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
  
// VIS includes
#include <IVtkOCC_Shape.hxx>
#include <IVtkTools_ShapeDataSource.hxx>
#include <IVtkTools_ShapeObject.hxx>
#include <IVtkTools_ShapePicker.hxx>
#include <IVtkTools_SubPolyDataFilter.hxx>
  
// OCCT includes
#include <BRepPrimAPI_MakeBox.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
  
//-----------------------------------------------------------------------------
// Shape Pipeline
//-----------------------------------------------------------------------------
  
DEFINE_STANDARD_HANDLE(ShapePipeline, Standard_Transient)
  
class ShapePipeline : public Standard_Transient
{
public:
  
  DEFINE_STANDARD_RTTI_INLINE(ShapePipeline, Standard_Transient)
  
public:
  
  ShapePipeline(const bool isHili = false) : m_bIsHili(isHili)
  {
    this->Build();
  
    if ( m_bIsHili )
    {
      m_actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_actor->GetProperty()->SetLineWidth(4.0);
    }
  }
  
  void Build()
  {
    m_subDataFilter = vtkSmartPointer<IVtkTools_SubPolyDataFilter>::New(); // Filter for polygonal sub-set of data
    m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New(); // Create mapper
    m_actor = vtkSmartPointer<vtkActor>::New(); // Create actor
  
    m_actor->SetMapper(m_mapper);
  }
  
  void InitSubPolyFilter(const TColStd_PackedMapOfInteger& theMask)
  {
    IVtk_IdTypeMap aDataToKeep;
    for ( TColStd_MapIteratorOfPackedMapOfInteger it(theMask); it.More(); it.Next() )
      aDataToKeep.Add( it.Key() );
  
    m_subDataFilter->SetData(aDataToKeep);
    m_subDataFilter->Modified();
  }
  
  void Init(const TopoDS_Shape& theShape)
  {
    // Prepare VIS topological wrapper
    static Standard_Integer ShapeID = 0; ++ShapeID;
    Handle(IVtkOCC_Shape) aShapeWrapper = new IVtkOCC_Shape(theShape);
    aShapeWrapper->SetId(ShapeID);
  
    // Create Data Source
    vtkSmartPointer<IVtkTools_ShapeDataSource>
      aShapeDS = vtkSmartPointer<IVtkTools_ShapeDataSource>::New();
  
    // Initialize Data Source with new wrapper
    aShapeDS->SetShape(aShapeWrapper);
  
    // Establish filters connectivity
    if ( m_bIsHili )
    {
      m_subDataFilter->SetInputConnection( aShapeDS->GetOutputPort() );
      m_mapper->SetInputConnection( m_subDataFilter->GetOutputPort() );
    }
    else
      m_mapper->SetInputConnection( aShapeDS->GetOutputPort() );
  
    // Bind DS to actor
    IVtkTools_ShapeObject::SetShapeSource(aShapeDS, m_actor);
  
    // Initialize mapper
    if ( m_bIsHili )
      m_mapper->ScalarVisibilityOff();
    else
      IVtkTools::InitShapeMapper(m_mapper);
  }
  
  void AddToRenderer(vtkRenderer* theRenderer) const
  {
    theRenderer->AddActor(m_actor);
  }
  
  void Update()
  {
    m_mapper->Update();
  }
  
public:
  
  inline vtkActor* Actor() const { return m_actor; }
  inline vtkPolyDataMapper* Mapper() const { return m_mapper; }
  
private:
  
  bool m_bIsHili;
  vtkSmartPointer<IVtkTools_SubPolyDataFilter> m_subDataFilter;
  vtkSmartPointer<vtkPolyDataMapper> m_mapper;
  vtkSmartPointer<vtkActor> m_actor;
  
};
  
//-----------------------------------------------------------------------------
// Global context
//-----------------------------------------------------------------------------
  
namespace CTX
{
  TopoDS_Shape Shape;
  Handle(ShapePipeline) ShapePL;
  Handle(ShapePipeline) ShapeHiliPL;
  vtkSmartPointer<vtkRenderWindow> RenderWindow;
};
  
//-----------------------------------------------------------------------------
// Shape transformation callback
//-----------------------------------------------------------------------------
  
#define EVENT_TRANSFORM (vtkCommand::UserEvent + 2001)
  
class TransformCallback : public vtkCommand
{
public:
  
  static TransformCallback* New();
  vtkTypeMacro(TransformCallback, vtkCommand);
  
public:
  
  virtual void Execute(vtkObject*,
                       unsigned long theEventId,
                       void*)
  {
    if ( theEventId != EVENT_TRANSFORM )
      return;
  
    static double SHIFT = 0; SHIFT += 10;
    int RAND_INDX = rand() % 10;
    int RAND_INDY = rand() % 10;
    int RAND_INDZ = rand() % 10;
  
    gp_Trsf aTrsf;
    aTrsf.SetRotation( gp_Ax1( gp_Pnt(0.0, 0.0, 0.0), gp_Dir(RAND_INDX, RAND_INDY, RAND_INDZ) ), SHIFT );
    aTrsf.SetTranslationPart( gp_Vec(0.0 + SHIFT, 0.0 + SHIFT, 0.0 + SHIFT) );
  
    TopoDS_Shape aNewShape = CTX::Shape.Moved(aTrsf);
  
    CTX::ShapePL->Init(aNewShape);
    CTX::ShapeHiliPL->Init(aNewShape);
  
    CTX::ShapePL->Update();
    CTX::ShapeHiliPL->Update();
  }
  
private:
  
  TransformCallback() : vtkCommand() {};
  ~TransformCallback() {};
  
};
  
TransformCallback* TransformCallback::New()
{
  return new TransformCallback();
}
  
//-----------------------------------------------------------------------------
// Interactor Style for picking
//-----------------------------------------------------------------------------
  
class InteractorStylePick : public vtkInteractorStyleTrackballCamera
{
public:
  
  static InteractorStylePick* New();
  vtkTypeMacro(InteractorStylePick, vtkInteractorStyleTrackballCamera);
  
// Customization:
public:
  
  void SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer) { m_renderer = theRenderer; }
  vtkSmartPointer<vtkRenderer> GetRenderer() const { return m_renderer; }
  void SetPicker(const vtkSmartPointer<IVtkTools_ShapePicker>& thePicker) { m_picker = thePicker; }
  vtkSmartPointer<IVtkTools_ShapePicker> GetPicker() const { return m_picker; }
  
// Overriding:
public:
  
  virtual void OnRightButtonDown()
  {
    vtkInteractorStyleTrackballCamera::OnRightButtonDown();
    // Invoke observers
    this->InvokeEvent(EVENT_TRANSFORM, NULL);
  }
  
  virtual void OnLeftButtonDown()
  {
    // Set selection mode only here in order to have access to rendered actors
    m_picker->SetSelectionMode(SM_Face);
  
    // Invoke basic method
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  
    Standard_Integer aPos[2] = { this->Interactor->GetEventPosition()[0],
                                 this->Interactor->GetEventPosition()[1] };
    m_picker->Pick(aPos[0], aPos[1], 0);

    // Traversing results
    vtkActor* aPickedActor = NULL;
    vtkSmartPointer<vtkActorCollection> anActorCollection = m_picker->GetPickedActors();
    //
    if ( anActorCollection && anActorCollection->GetNumberOfItems() > 0 )
    {
      anActorCollection->InitTraversal();
      while ( vtkActor* anActor = anActorCollection->GetNextActor() )
      {
        aPickedActor = anActor;
        IVtkTools_ShapeDataSource* aDataSource = IVtkTools_ShapeObject::GetShapeSource(anActor);
        if ( !aDataSource )
          continue;
  
        // Access initial shape wrapper
        IVtkOCC_Shape::Handle aShapeWrapper = aDataSource->GetShape();
        if ( aShapeWrapper.IsNull() )
          continue;
  
        IVtk_IdType aShapeID = aShapeWrapper->GetId();
        IVtk_ShapeIdList subShapeIds = m_picker->GetPickedSubShapesIds(aShapeID);
  
        // Get IDs of cells for picked sub-shapes.
        TColStd_PackedMapOfInteger aCellMask;
        for ( IVtk_ShapeIdList::Iterator sIt(subShapeIds); sIt.More(); sIt.Next() )
        {
          aCellMask.Add( (int) sIt.Value() );
          const TopoDS_Shape& aSubShape = aShapeWrapper->GetSubShape( sIt.Value() );
          cout << "--------------------------------------------------------------" << endl;
          cout << "Sub-shape ID: " << sIt.Value() << endl;
          cout << "Sub-shape type: " << aSubShape.TShape()->DynamicType()->Name() << endl;
        }
  
        CTX::ShapeHiliPL->InitSubPolyFilter(aCellMask);
        CTX::ShapeHiliPL->Update();
        break;
      }
    }
  }
  
private:
  
  InteractorStylePick(const InteractorStylePick&);
  void operator=(const InteractorStylePick&);
  
private:
  
  InteractorStylePick() : vtkInteractorStyleTrackballCamera() {}
  ~InteractorStylePick() {}
  
private:
  
  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<IVtkTools_ShapePicker> m_picker;
  
};
  
vtkStandardNewMacro(InteractorStylePick);
  
//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

int main(int, char **)
{
  // Initialize Renderer & Render Window
  vtkSmartPointer<vtkRenderer> aRenderer = vtkSmartPointer<vtkRenderer>::New();
  aRenderer->GetActiveCamera()->ParallelProjectionOn();
  aRenderer->LightFollowCameraOn();
  aRenderer->SetBackground(0, 0, 0);
  CTX::RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  CTX::RenderWindow->AddRenderer(aRenderer);
  
  // Initialize Picker
  vtkSmartPointer<IVtkTools_ShapePicker> aShapePicker = vtkSmartPointer<IVtkTools_ShapePicker>::New();
  aShapePicker->SetTolerance(0.025);
  aShapePicker->SetRenderer(aRenderer);
  
  // Create test OCCT shape
  CTX::Shape = BRepPrimAPI_MakeBox(60, 80, 90).Shape();
  
  // Create VTK pipeline enriched with VIS stuff for initial shape
  CTX::ShapePL = new ShapePipeline();
  CTX::ShapePL->Init(CTX::Shape);
  CTX::ShapePL->AddToRenderer(aRenderer);
  
  // Create VIS pipeline for highlighting
  CTX::ShapeHiliPL = new ShapePipeline(true);
  CTX::ShapeHiliPL->Init(CTX::Shape);
  CTX::ShapeHiliPL->AddToRenderer(aRenderer);
  
  // Initialize Interactor Style
  vtkSmartPointer<InteractorStylePick> aStylePick = vtkSmartPointer<InteractorStylePick>::New();
  aStylePick->SetRenderer(aRenderer);
  aStylePick->SetPicker(aShapePicker);
  
  // Establish listeners
  vtkSmartPointer<TransformCallback> aTransformCB = vtkSmartPointer<TransformCallback>::New();
  if ( !aStylePick->HasObserver(EVENT_TRANSFORM) )
    aStylePick->AddObserver(EVENT_TRANSFORM, aTransformCB);
  
  // Initialize Interactor
  vtkSmartPointer<vtkRenderWindowInteractor> aRenInter = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  aRenInter->SetRenderWindow(CTX::RenderWindow);
  aRenInter->SetInteractorStyle(aStylePick);

  /////////////////////////////////////////////////////////////////////////////
  vtkSmartPointer<vtkPlaneWidget> planeWidget = 
      vtkSmartPointer<vtkPlaneWidget>::New();
  planeWidget->SetInteractor(aRenInter);

  planeWidget->On();
  /////////////////////////////////////////////////////////////////////////////
  
  // Start rendering
  CTX::RenderWindow->Render();
  aRenInter->Start();
  
  return EXIT_SUCCESS;
}

#endif
