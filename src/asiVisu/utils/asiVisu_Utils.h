//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_Utils_h
#define asiVisu_Utils_h

// A-Situs includes
#include <asiVisu.h>

// Visualization includes
#include <asiVisu_Prs.h>

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
typedef Handle(asiVisu_Prs) (*asiVisu_PrsAllocator)(const Handle(ActAPI_INode)&);

#define DEFINE_PRESENTATION_FACTORY(CNode, AllocFunction) \
  static void RegisterPrs() \
  { \
    asiVisu_Utils::RegisterPrsType(STANDARD_TYPE(CNode)->Name(), AllocFunction); \
  }

#define REGISTER_PRESENTATION(C) \
  C::RegisterPrs();

//-----------------------------------------------------------------------------

//! Orientations.
enum asiVisu_Orientation
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
class asiVisu_Utils
{
// Presentation factory:
public:

  //! Mapping between Node types and allocation routines.
  typedef NCollection_DataMap<TCollection_AsciiString, asiVisu_PrsAllocator> TPrsAllocMap;

  asiVisu_EXPORT static TPrsAllocMap
    RegisterPrsType(const TCollection_AsciiString& theType,
                    const asiVisu_PrsAllocator     theAllocFunc);

  asiVisu_EXPORT static const TPrsAllocMap&
    GetAllocMap();

// Scene:
public:

  asiVisu_EXPORT static int
    ComputeVisiblePropBounds(vtkRenderer*       theRenderer,
                             double             theBounds[6],
                             vtkPropCollection* thePropsToSkip);

  asiVisu_EXPORT static void
    ResetCamera(vtkRenderer*       theRenderer,
                vtkPropCollection* thePropsToSkip);

  asiVisu_EXPORT static bool
    AdjustCamera(vtkRenderer*       theRenderer,
                 vtkPropCollection* thePropsToSkip,
                 bool               isDefaultNorm   = false,
                 bool               doScaling       = true);

  asiVisu_EXPORT static void
    AdjustCameraClippingRange(vtkRenderer* theRenderer);

  asiVisu_EXPORT static void
    CameraOnTop(vtkRenderer* theRenderer);

  asiVisu_EXPORT static void
    CameraOnBottom(vtkRenderer* theRenderer);

  asiVisu_EXPORT static void
    CameraOnFront(vtkRenderer* theRenderer);

  asiVisu_EXPORT static void
    CameraOnBack(vtkRenderer* theRenderer);

  asiVisu_EXPORT static void
    CameraOnLeft(vtkRenderer* theRenderer);

  asiVisu_EXPORT static void
    CameraOnRight(vtkRenderer* theRenderer);

  asiVisu_EXPORT static void
    ApplyLightingRules(vtkActor* theActor);

  asiVisu_EXPORT static void
    TranslateView(vtkRenderer* theRenderer,
                  const int    theOldX,
                  const int    theOldY,
                  const int    theNewX,
                  const int    theNewY);

  asiVisu_EXPORT static QImage
    DumpView(vtkRenderWindow* theRenderWindow);

  asiVisu_EXPORT static void
    AdjustTrihedron(vtkRenderer*       theRenderer,
                    vtkAxesActor*      theActor,
                    vtkPropCollection* thePropsToSkip);

// Other commons:
public:

  asiVisu_EXPORT static vtkSmartPointer<vtkIntArray>
    InitIntArray(const char* theArrName);

  asiVisu_EXPORT static vtkSmartPointer<vtkDoubleArray>
    InitDoubleArray(const char* theArrName);

  asiVisu_EXPORT static vtkSmartPointer<vtkDoubleArray>
    InitDoubleVectorArray(const char* theArrName);

  asiVisu_EXPORT static vtkSmartPointer<vtkStringArray>
    InitStringArray(const char* theArrName);

  asiVisu_EXPORT static void
    DefaultPickingColor(double& fR,
                        double& fG,
                        double& fB);

  asiVisu_EXPORT static void
    DefaultDetectionColor(double& fR,
                          double& fG,
                          double& fB);

  asiVisu_EXPORT static double
    DefaultPickLineWidth();

  asiVisu_EXPORT static double
    DefaultDetectionLineWidth();

  asiVisu_EXPORT static double
    DefaultHilightPointSize();

  asiVisu_EXPORT static vtkSmartPointer<vtkLookupTable>
    InitLookupTable();

  asiVisu_EXPORT static void
    SetLookupTableColor(vtkLookupTable* theColorTable,
                        const IVtk_MeshType theColorRole,
                        const double theR, const double theG, const double theB,
                        const double /*theA*/);

  asiVisu_EXPORT static void
    GetLookupTableColor(vtkLookupTable* theColorTable,
                        const IVtk_MeshType theColorRole,
                        double &theR, double &theG, double &theB);

  asiVisu_EXPORT static void
    GetLookupTableColor(vtkLookupTable* theColorTable,
                        const IVtk_MeshType theColorRole,
                        double &theR, double &theG, double &theB,
                        double &theA);

  asiVisu_EXPORT static void
    InitShapeMapper(vtkMapper* theMapper);

  asiVisu_EXPORT static void
    InitShapeMapper(vtkMapper* theMapper, vtkLookupTable* theColorTable);

  asiVisu_EXPORT static vtkSmartPointer<vtkLookupTable>
    InitDomainLookupTable();

  asiVisu_EXPORT static void
    InitMapper(vtkMapper*      theMapper,
               vtkLookupTable* theLookup,
               const char*     theScalarsArrName);

  asiVisu_EXPORT static void
    InitTextWidget(vtkTextWidget* theTextWidget);

public:

  double static TrimInf(const double val,
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
