/*=========================================================================

  Program:   Visualization Toolkit
  Module:    asiVisu_PlaneWidget.h
  Original:  vtkPlaneWidget.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   asiVisu_PlaneWidget
 * @brief   3D widget for manipulating a finite plane
 *
 * This is a copy of vtkPlaneWidget class with some ameliarations which
 * were impossible to make by inheritance.
*/

#ifndef asiVisu_PlaneWidget_h
#define asiVisu_PlaneWidget_h

// asiVisu includes
#include <asiVisu_Utils.h>

// asiData includes
#include <asiData_IVSurfaceNode.h>

// Active Data includes
#include <ActAPI_IModel.h>

// OCCT includes
#include <Geom_Plane.hxx>

// VTK includes
#include <vtkInteractionWidgetsModule.h> // For export macro
#include <vtkPolyDataSourceWidget.h>

class vtkActor;
class vtkCellPicker;
class vtkConeSource;
class vtkLineSource;
class vtkPlaneSource;
class vtkPoints;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkProp;
class vtkProperty;
class vtkSphereSource;
class vtkTransform;
class vtkPlane;

#define VTK_PLANE_OFF 0
#define VTK_PLANE_OUTLINE 1
#define VTK_PLANE_WIREFRAME 2
#define VTK_PLANE_SURFACE 3

class asiVisu_EXPORT asiVisu_PlaneWidget : public vtkPolyDataSourceWidget
{
public:
  /**
   * Instantiate the object.
   */
  static asiVisu_PlaneWidget *New();

  vtkTypeMacro(asiVisu_PlaneWidget,vtkPolyDataSourceWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  //@{
  /**
   * Methods that satisfy the superclass' API.
   */
  virtual void SetEnabled(int);
  virtual void PlaceWidget(double bounds[6]);
  void PlaceWidget()
    {this->Superclass::PlaceWidget();}
  void PlaceWidget(double xmin, double xmax, double ymin, double ymax,
                   double zmin, double zmax)
    {this->Superclass::PlaceWidget(xmin,xmax,ymin,ymax,zmin,zmax);}
  //@}

  //@{
  /**
   * Set/Get the resolution (number of subdivisions) of the plane.
   */
  void SetResolution(int r);
  int GetResolution();
  //@}

  //@{
  /**
   * Set/Get the origin of the plane.
   */
  void SetOrigin(double x, double y, double z);
  void SetOrigin(double x[3]);
  double* GetOrigin();
  void GetOrigin(double xyz[3]);
  //@}

  //@{
  /**
   * Set/Get the position of the point defining the first axis of the plane.
   */
  void SetPoint1(double x, double y, double z);
  void SetPoint1(double x[3]);
  double* GetPoint1();
  void GetPoint1(double xyz[3]);
  //@}

  //@{
  /**
   * Set/Get the position of the point defining the second axis of the plane.
   */
  void SetPoint2(double x, double y, double z);
  void SetPoint2(double x[3]);
  double* GetPoint2();
  void GetPoint2(double xyz[3]);
  //@}

  //@{
  /**
   * Get the center of the plane.
   */
  void SetCenter(double x, double y, double z);
  void SetCenter(double x[3]);
  double* GetCenter();
  void GetCenter(double xyz[3]);
  //@}

  //@{
  /**
   * Get the normal to the plane.
   */
  void SetNormal(double x, double y, double z);
  void SetNormal(double x[3]);
  double* GetNormal();
  void GetNormal(double xyz[3]);
  //@}

  //@{
  /**
   * Control how the plane appears when GetPolyData() is invoked.
   * If the mode is "outline", then just the outline of the plane
   * is shown. If the mode is "wireframe" then the plane is drawn
   * with the outline plus the interior mesh (corresponding to the
   * resolution specified). If the mode is "surface" then the plane
   * is drawn as a surface.
   */
  vtkSetClampMacro(Representation,int,VTK_PLANE_OFF,VTK_PLANE_SURFACE);
  vtkGetMacro(Representation,int);
  void SetRepresentationToOff()
    {this->SetRepresentation(VTK_PLANE_OFF);}
  void SetRepresentationToOutline()
    {this->SetRepresentation(VTK_PLANE_OUTLINE);}
  void SetRepresentationToWireframe()
    {this->SetRepresentation(VTK_PLANE_WIREFRAME);}
  void SetRepresentationToSurface()
    {this->SetRepresentation(VTK_PLANE_SURFACE);}
  //@}

  //@{
  /**
   * Force the plane widget to be aligned with one of the x-y-z axes.
   * Remember that when the state changes, a ModifiedEvent is invoked.
   * This can be used to snap the plane to the axes if it is originally
   * not aligned.
   */
  vtkSetMacro(NormalToXAxis,int);
  vtkGetMacro(NormalToXAxis,int);
  vtkBooleanMacro(NormalToXAxis,int);
  vtkSetMacro(NormalToYAxis,int);
  vtkGetMacro(NormalToYAxis,int);
  vtkBooleanMacro(NormalToYAxis,int);
  vtkSetMacro(NormalToZAxis,int);
  vtkGetMacro(NormalToZAxis,int);
  vtkBooleanMacro(NormalToZAxis,int);
  //@}

  /**
   * Grab the polydata (including points) that defines the plane.  The
   * polydata consists of (res+1)*(res+1) points, and res*res quadrilateral
   * polygons, where res is the resolution of the plane. These point values
   * are guaranteed to be up-to-date when either the InteractionEvent or
   * EndInteraction events are invoked. The user provides the vtkPolyData and
   * the points and polyplane are added to it.
   */
  void GetPolyData(vtkPolyData *pd);

  /**
   * Get the planes describing the implicit function defined by the plane
   * widget. The user must provide the instance of the class vtkPlane. Note
   * that vtkPlane is a subclass of vtkImplicitFunction, meaning that it can
   * be used by a variety of filters to perform clipping, cutting, and
   * selection of data.
   */
  void GetPlane(vtkPlane *plane);

  /**
   * Satisfies superclass API.  This returns a pointer to the underlying
   * PolyData.  Make changes to this before calling the initial PlaceWidget()
   * to have the initial placement follow suit.  Or, make changes after the
   * widget has been initialised and call UpdatePlacement() to realise.
   */
  vtkPolyDataAlgorithm* GetPolyDataAlgorithm();

  /**
   * Satisfies superclass API.  This will change the state of the widget to
   * match changes that have been made to the underlying PolyDataSource
   */
  void UpdatePlacement(void);

  //@{
  /**
   * Get the handle properties (the little balls are the handles). The
   * properties of the handles when selected and normal can be
   * manipulated.
   */
  vtkGetObjectMacro(HandleProperty,vtkProperty);
  vtkGetObjectMacro(SelectedHandleProperty,vtkProperty);
  //@}

  //@{
  /**
   * Get the plane properties. The properties of the plane when selected
   * and unselected can be manipulated.
   */
  virtual void SetPlaneProperty(vtkProperty*);
  vtkGetObjectMacro(PlaneProperty,vtkProperty);
  vtkGetObjectMacro(SelectedPlaneProperty,vtkProperty);
  //@}

protected:
  asiVisu_PlaneWidget();
  ~asiVisu_PlaneWidget();

  // Manage the state of the widget
  int State;
  enum WidgetState
  {
    Start=0,
    Moving,
    Scaling,
    Pushing,
    Rotating,
    Spinning,
    Outside,
    Pinching
  };

  //handles the events
  static void ProcessEvents(vtkObject* object,
                            unsigned long event,
                            void* clientdata,
                            void* calldata);

  // ProcessEvents() dispatches to these methods.
  void OnLeftButtonDown();
  void OnLeftButtonUp();
  void OnMiddleButtonDown();
  void OnMiddleButtonUp();
  void OnRightButtonDown();
  void OnRightButtonUp();
  void OnMouseMove();
  void OnStartPinch();
  void OnPinch();
  void OnEndPinch();

  // controlling ivars
  int NormalToXAxis;
  int NormalToYAxis;
  int NormalToZAxis;
  int Representation;
  void SelectRepresentation();

  // the plane
  vtkActor          *PlaneActor;
  vtkPolyDataMapper *PlaneMapper;
  vtkPlaneSource    *PlaneSource;
  vtkPolyData       *PlaneOutline;
  void HighlightPlane(int highlight);

  // glyphs representing hot spots (e.g., handles)
  vtkActor          **Handle;
  vtkPolyDataMapper **HandleMapper;
  vtkSphereSource   **HandleGeometry;
  void PositionHandles();
  void HandlesOn(double length);
  void HandlesOff();
  int HighlightHandle(vtkProp *prop); //returns cell id
  virtual void SizeHandles();

  // the normal cone
  vtkActor          *ConeActor;
  vtkPolyDataMapper *ConeMapper;
  vtkConeSource     *ConeSource;
  void HighlightNormal(int highlight);

  // the normal line
  vtkActor          *LineActor;
  vtkPolyDataMapper *LineMapper;
  vtkLineSource     *LineSource;

  // the normal cone
  vtkActor          *ConeActor2;
  vtkPolyDataMapper *ConeMapper2;
  vtkConeSource     *ConeSource2;

  // the normal line
  vtkActor          *LineActor2;
  vtkPolyDataMapper *LineMapper2;
  vtkLineSource     *LineSource2;

  // Do the picking
  vtkCellPicker *HandlePicker;
  vtkCellPicker *PlanePicker;
  vtkActor *CurrentHandle;

  // Register internal Pickers within PickingManager
  virtual void RegisterPickers();

  // Methods to manipulate the hexahedron.
  void MoveOrigin(double *p1, double *p2);
  void MovePoint1(double *p1, double *p2);
  void MovePoint2(double *p1, double *p2);
  void MovePoint3(double *p1, double *p2);
  void Rotate(int X, int Y, double *p1, double *p2, double *vpn);
  void Spin(double *p1, double *p2);
  void Scale(double *p1, double *p2, int X, int Y);
  void Translate(double *p1, double *p2);
  void Push(double *p1, double *p2);

  // Plane normal, normalized
  double Normal[3];

  // Transform the hexahedral points (used for rotations)
  vtkTransform *Transform;

  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty *HandleProperty;
  vtkProperty *SelectedHandleProperty;
  vtkProperty *PlaneProperty;
  vtkProperty *SelectedPlaneProperty;
  void CreateDefaultProperties();

  void GeneratePlane();

  int    LastPickValid;
  double HandleSizeFactor;

public:

  void SetSurfaceNode(const Handle(asiData_IVSurfaceNode)& node)
  {
    m_node = node;

    Handle(Geom_Plane)
      plane = Handle(Geom_Plane)::DownCast( m_node->GetSurface() );

    if ( plane.IsNull() )
      return;

    const gp_Pnt& origin = plane->Position().Location();
    const gp_Dir& normal = plane->Position().Direction();

    this->SetCenter( origin.X(),
                     origin.Y(),
                     origin.Z() );
    this->SetNormal( normal.X(),
                     normal.Y(),
                     normal.Z() );

    //double point1[3];
    //this->GetPoint1(point1);
    //point1[0] *= 100;
    //point1[1] *= 100;
    //point1[2] *= 100;
    //this->SetPoint1(point1);

    //double bounds[6];
    //bounds[0] = -0.5;
    //bounds[1] = 0.5;
    //bounds[2] = -0.5;
    //bounds[3] = 0.5;
    //bounds[4] = -0.5;
    //bounds[5] = 0.5;
    ////
    //this->PlaceWidget(bounds)
  }

  void SetModel(const Handle(ActAPI_IModel)& model)
  {
    m_model = model;
  }

public:

  void UpdateSurfaceNode();

private:

  Handle(asiData_IVSurfaceNode) m_node;  //!< Surface Node.
  Handle(ActAPI_IModel)         m_model; //!< Data Model instance.

private:
  asiVisu_PlaneWidget(const asiVisu_PlaneWidget&) VTK_DELETE_FUNCTION;
  void operator=(const asiVisu_PlaneWidget&) VTK_DELETE_FUNCTION;
};

#endif
