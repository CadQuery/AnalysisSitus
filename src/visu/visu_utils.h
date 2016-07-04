//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
#include <vtkAxesActor.h>
#include <vtkCubeAxesActor.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkInteractorObserver.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTextWidget.h>
#pragma warning(pop)

// IVtk includes
#include <IVtk_Types.hxx>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>

//-----------------------------------------------------------------------------

#define INF_LIMIT 100

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

//! Orientations.
enum visu_orientation
{
  VisuOri_Undefined = -1,
  VisuOri_Forward   =  0,
  VisuOri_Reversed  =  1,
  VisuOri_Internal  =  2,
  VisuOri_External  =  3,
  VisuOri_Last
};

//-----------------------------------------------------------------------------

//! Common visualization utilities.
class visu_utils
{
// Presentation factory:
public:

  //! Mapping between Node types and allocation routines.
  typedef NCollection_DataMap<TCollection_AsciiString, visu_prs_allocator> TPrsAllocMap;

  static TPrsAllocMap
    RegisterPrsType(const TCollection_AsciiString& theType,
                    const visu_prs_allocator       theAllocFunc);

  static const TPrsAllocMap&
    GetAllocMap();

// Scene:
public:

  static int
    ComputeVisiblePropBounds(vtkRenderer*       theRenderer,
                             double             theBounds[6],
                             vtkPropCollection* thePropsToSkip);

  static void
    ResetCamera(vtkRenderer*       theRenderer,
                vtkPropCollection* thePropsToSkip);

  static bool
    AdjustCamera(vtkRenderer*       theRenderer,
                 vtkPropCollection* thePropsToSkip,
                 bool               isDefaultNorm   = false,
                 bool               doScaling       = true);

  static void
    AdjustCameraClippingRange(vtkRenderer* theRenderer);

  static void
    CameraOnTop(vtkRenderer* theRenderer);

  static void
    CameraOnBottom(vtkRenderer* theRenderer);

  static void
    CameraOnFront(vtkRenderer* theRenderer);

  static void
    CameraOnBack(vtkRenderer* theRenderer);

  static void
    CameraOnLeft(vtkRenderer* theRenderer);

  static void
    CameraOnRight(vtkRenderer* theRenderer);

  static void
    ApplyLightingRules(vtkActor* theActor);

  static void
    TranslateView(vtkRenderer* theRenderer,
                  const int    theOldX,
                  const int    theOldY,
                  const int    theNewX,
                  const int    theNewY);

  static QImage
    DumpView(vtkRenderWindow* theRenderWindow);

  static void
    AdjustTrihedron(vtkRenderer*       theRenderer,
                    vtkAxesActor*      theActor,
                    vtkPropCollection* thePropsToSkip);

// Other commons:
public:

  static vtkSmartPointer<vtkIntArray>
    InitIntArray(const char* theArrName);

  static vtkSmartPointer<vtkDoubleArray>
    InitDoubleArray(const char* theArrName);

  static vtkSmartPointer<vtkDoubleArray>
    InitDoubleVectorArray(const char* theArrName);

  static vtkSmartPointer<vtkStringArray>
    InitStringArray(const char* theArrName);

  static void
    DefaultPickingColor(double& fR,
                        double& fG,
                        double& fB);

  static void
    DefaultDetectionColor(double& fR,
                          double& fG,
                          double& fB);

  static double
    DefaultPickLineWidth();

  static double
    DefaultDetectionLineWidth();

  static double
    DefaultHilightPointSize();

  static vtkLookupTable*
    InitLookupTable();

  static void
    SetLookupTableColor(vtkLookupTable* theColorTable,
                        const IVtk_MeshType theColorRole,
                        const double theR, const double theG, const double theB,
                        const double /*theA*/);

  static void
    GetLookupTableColor(vtkLookupTable* theColorTable,
                        const IVtk_MeshType theColorRole,
                        double &theR, double &theG, double &theB);

  static void
    GetLookupTableColor(vtkLookupTable* theColorTable,
                        const IVtk_MeshType theColorRole,
                        double &theR, double &theG, double &theB,
                        double &theA);

  static void
    InitShapeMapper(vtkMapper* theMapper);

  static void
    InitShapeMapper(vtkMapper* theMapper, vtkLookupTable* theColorTable);

  static vtkSmartPointer<vtkLookupTable>
    InitDomainLookupTable();

  static void
    InitMapper(vtkMapper*      theMapper,
               vtkLookupTable* theLookup,
               const char*     theScalarsArrName);

  static void
    InitTextWidget(vtkTextWidget* theTextWidget);

public:

  inline double static TrimInf(const double val,
                               const double limit = INF_LIMIT)
  {
    double ret_val = val;
    if ( Precision::IsPositiveInfinite(val) )
      ret_val = limit;
    else if ( Precision::IsNegativeInfinite(val) )
      ret_val = -limit;

    return ret_val;
  }

private:

  static TPrsAllocMap m_allocMap; //!< Presentation factory.

};

#endif
