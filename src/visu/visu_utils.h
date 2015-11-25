//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_utils_h
#define visu_utils_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// Qt includes
#pragma warning(push, 0)
#include <QImage>
#pragma warning(pop)

// VTK includes
#pragma warning(push, 0)
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkInteractorObserver.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkAxesActor.h>
#include <vtkCubeAxesActor.h>
#include <vtkCubeAxesActor2D.h>
#pragma warning(pop)

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>

//! Common visualization utilities.
class visu_utils
{
// Scene:
public:

  ASitus_EXPORT static int
    ComputeVisiblePropBounds(vtkRenderer*         theRenderer,
                             vtkFloatingPointType theBounds[6],
                             vtkPropCollection*   thePropsToSkip);

  ASitus_EXPORT static void
    ResetCamera(vtkRenderer*       theRenderer,
                vtkPropCollection* thePropsToSkip);

  ASitus_EXPORT static bool
    AdjustCamera(vtkRenderer*       theRenderer,
                 vtkPropCollection* thePropsToSkip,
                 bool               isDefaultNorm   = false,
                 bool               doScaling       = true);

  ASitus_EXPORT static void
    AdjustCameraClippingRange(vtkRenderer* theRenderer);

  ASitus_EXPORT static void
    CameraOnTop(vtkRenderer* theRenderer);

  ASitus_EXPORT static void
    CameraOnBottom(vtkRenderer* theRenderer);

  ASitus_EXPORT static void
    CameraOnFront(vtkRenderer* theRenderer);

  ASitus_EXPORT static void
    CameraOnBack(vtkRenderer* theRenderer);

  ASitus_EXPORT static void
    CameraOnLeft(vtkRenderer* theRenderer);

  ASitus_EXPORT static void
    CameraOnRight(vtkRenderer* theRenderer);

  ASitus_EXPORT static void
    ApplyLightingRules(vtkActor* theActor);

  ASitus_EXPORT static void
    TranslateView(vtkRenderer* theRenderer,
                  const int    theOldX,
                  const int    theOldY,
                  const int    theNewX,
                  const int    theNewY);

  ASitus_EXPORT static QImage
    DumpView(vtkRenderWindow* theRenderWindow);

  ASitus_EXPORT static void
    AdjustTrihedron(vtkRenderer*       theRenderer,
                    vtkAxesActor*      theActor,
                    vtkPropCollection* thePropsToSkip);

// Other commons:
public:

  ASitus_EXPORT static vtkSmartPointer<vtkIntArray>
    InitIntArray(const char* theArrName);

  ASitus_EXPORT static vtkSmartPointer<vtkDoubleArray>
    InitDoubleArray(const char* theArrName);

  ASitus_EXPORT static vtkSmartPointer<vtkDoubleArray>
    InitDoubleVectorArray(const char* theArrName);

  ASitus_EXPORT static vtkSmartPointer<vtkStringArray>
    InitStringArray(const char* theArrName);

  ASitus_EXPORT static void
    DefaultPickingColor(double& fR,
                        double& fG,
                        double& fB);

  ASitus_EXPORT static void
    DefaultDetectionColor(double& fR,
                          double& fG,
                          double& fB);

  ASitus_EXPORT static double
    DefaultPickLineWidth();

  ASitus_EXPORT static double
    DefaultDetectionLineWidth();

  ASitus_EXPORT static double
    DefaultHilightPointSize();

};

#endif
