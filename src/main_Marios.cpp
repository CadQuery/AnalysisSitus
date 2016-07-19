#include <iostream>
#include <vector>
#include <string>

#include "TopoDS_Shape.hxx"
#include "IVtkOCC_Shape.hxx"
#include "IVtkTools_ShapeDataSource.hxx"
#include "IGESControl_Reader.hxx"
#include "STEPControl_Reader.hxx"
#include "IFSelect_ReturnStatus.hxx"
#include "BRepBuilderAPI_NurbsConvert.hxx"
#include "TColStd_HSequenceOfTransient.hxx"
#include "Handle_TColStd_HSequenceOfTransient.hxx"
#include "Standard.hxx"
#include "NCollection_TListIterator.hxx"
#include "IVtk_IShape.hxx"

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkRendererCollection.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include "vtkStandardPolyDataPainter.h"
#include "vtkActorCollection.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkSmartPointer.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "IVtkTools_DisplayModeFilter.hxx"
#include "vtkPolyDataNormals.h"
#include "vtkDataSetMapper.h"
#include "IVtkTools_ShapePicker.hxx"
#include "BRepPrimAPI_MakeSphere.hxx"

#include "stepigesreaderwriter.h"

using namespace std;

// Class with implemented mouse events. Just Left button down was implemented for the sake of testing

class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera
{
  public:
    static MouseInteractorStyle2* New();
    vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera)

    void OnLeftButtonDown()
    {
        // Get click position
        int* clickPos = this->GetInteractor()->GetEventPosition();

        // Create picker
        vtkSmartPointer<IVtkTools_ShapePicker> picker = vtkSmartPointer<IVtkTools_ShapePicker>::New();
        picker->SetTolerance(0.025);
        picker->SetRenderer(this->GetDefaultRenderer());

        // Initialize picking algo
        picker->SetSelectionMode(SM_Vertex, false);
        picker->SetSelectionMode(SM_Edge, false);
        picker->SetSelectionMode(SM_Face);

        // Perform pick
        picker->Pick(clickPos[0], clickPos[1], 0);

        // Access pick results
        vtkActorCollection* actorCollection = picker->GetPickedActors();
        if(actorCollection)
        {
            actorCollection->InitTraversal();

            // !! anActor is always NULL
            // !! No entry in while loop
            while(vtkActor* anActor = actorCollection->GetNextActor())
            {
                cout << 1 << endl;
                // Show face No.
            }
        }

        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
};

vtkStandardNewMacro(MouseInteractorStyle2)

int main(int argc, char *argv[])
{
    TCollection_AsciiString filename;
    if(argc == 2)
    {
        filename = argv[1];
    }
    else
    {
        cout << "ERROR: Invalid number of arguments\n";
        return 0;
    }

    // Input geometry
    StepIgesReaderWriter myRW(filename);

    TopoDS_Shape shape = myRW.returnShape();

    // The renderer generates the image
    // which is then displayed on the render window.
    // It can be thought of as a scene to which the actor is added
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    // Set shape to data source
    IVtkOCC_Shape::Handle aShapeImpl = new IVtkOCC_Shape(shape);
    vtkSmartPointer<IVtkTools_ShapeDataSource> DS = vtkSmartPointer<IVtkTools_ShapeDataSource>::New();
    DS->SetShape(aShapeImpl);

    // Create a mapper
    //vtkSmartPointer<vtkDataSetMapper> Mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    vtkSmartPointer<vtkPolyDataMapper> Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    // Enable shading representation
    vtkSmartPointer<IVtkTools_DisplayModeFilter> DMFilter = vtkSmartPointer<IVtkTools_DisplayModeFilter>::New();
    DMFilter->SetInputConnection(DS->GetOutputPort());//aSUBFilter->GetOutputPort());
    DMFilter->SetDisplayMode(DM_Shading);

    // Smoothen the shading by applying surface normals
    vtkSmartPointer<vtkPolyDataNormals> DataNormalsF = vtkSmartPointer<vtkPolyDataNormals>::New();
    DataNormalsF->SetInputConnection(DMFilter->GetOutputPort());
    Mapper->SetInputConnection(DataNormalsF->GetOutputPort());

    // Create shape actor. Connect to mapper and renderer
    vtkSmartPointer<vtkActor> Actor = vtkSmartPointer<vtkActor>::New();
    Actor->SetMapper(Mapper);
    Actor->GetProperty()->SetRepresentationToSurface();
    renderer->AddActor(Actor);
    renderer->SetBackground(0.1, 0.2, 0.4);


    // Zoom in a little by accessing the camera and invoking its "Zoom" method.
    renderer->ResetCamera();
    renderer->AutomaticLightCreationOn();
    renderer->GetActiveCamera()->Zoom(1.5);

    // The render window is the actual GUI window
    // that appears on the computer screen
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetMultiSamples(8);

    // The render window interactor captures mouse events
    // and will perform appropriate camera or actor manipulation
    // depending on the nature of the events.
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Set new clicking options
    vtkSmartPointer<MouseInteractorStyle2> style = vtkSmartPointer<MouseInteractorStyle2>::New();
    style->SetDefaultRenderer(renderer);
    renderWindowInteractor->SetInteractorStyle(style);


    // This starts the event loop and as a side effect causes an initial render.
    renderWindowInteractor->Start();

    cout << "Done" << endl;
    return 0;
}
