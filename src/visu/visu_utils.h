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

// Visualization includes
#include <visu_prs.h>

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

// IVtk includes
#include <IVtk_Types.hxx>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>

//-----------------------------------------------------------------------------

//! Pointer to Node allocation routine.
typedef Handle(visu_prs) (*visu_prs_allocator)(const Handle(ActAPI_INode)&);

#define DEFINE_PRESENTATION_FACTORY(CNode, AllocFunction) \
  static void RegisterPrs() \
  { \
    visu_utils::RegisterPrsType(STANDARD_TYPE(CNode)->Name(), AllocFunction); \
  }

#define REGISTER_PRESENTATION(C) \
  C::RegisterPrs();

//-----------------------------------------------------------------------------

//! Common visualization utilities.
class visu_utils
{
// Presentation factory:
public:

  //! Mapping between Node types and allocation routines.
  typedef NCollection_DataMap<TCollection_AsciiString, visu_prs_allocator> TPrsAllocMap;

  ASitus_EXPORT static TPrsAllocMap
    RegisterPrsType(const TCollection_AsciiString& theType,
                    const visu_prs_allocator       theAllocFunc);

  ASitus_EXPORT static const TPrsAllocMap&
    GetAllocMap();

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

  ASitus_EXPORT static vtkLookupTable*
    InitLookupTable();

  ASitus_EXPORT static void
    SetLookupTableColor(vtkLookupTable* theColorTable,
                        const IVtk_MeshType theColorRole,
                        const double theR, const double theG, const double theB,
                        const double /*theA*/);

  ASitus_EXPORT static void
    GetLookupTableColor(vtkLookupTable* theColorTable,
                        const IVtk_MeshType theColorRole,
                        double &theR, double &theG, double &theB);

  ASitus_EXPORT static void
    GetLookupTableColor(vtkLookupTable* theColorTable,
                        const IVtk_MeshType theColorRole,
                        double &theR, double &theG, double &theB,
                        double &theA);

  ASitus_EXPORT static void
    InitShapeMapper(vtkMapper* theMapper);

  ASitus_EXPORT static void
    InitShapeMapper(vtkMapper* theMapper, vtkLookupTable* theColorTable);

private:

  static TPrsAllocMap m_allocMap; //!< Presentation factory.

};

#endif
