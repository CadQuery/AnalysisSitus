//-----------------------------------------------------------------------------
// Created on: 20 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

//! Curvature combs.
enum asiVisu_CurvatureCombElem
{
  VisuCurvComb_Undefined    = -1,
  VisuCurvComb_PointOk      =  0,
  VisuCurvComb_PointFailure =  1,
  VisuCurvComb_Comb         =  2,
  VisuCurvComb_Envelope     =  3,
  VisuCurvComb_Last
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
    InitShapeMapper(vtkMapper* theMapper);

  asiVisu_EXPORT static void
    InitShapeMapper(vtkMapper* theMapper, vtkLookupTable* theColorTable);

  asiVisu_EXPORT static vtkSmartPointer<vtkLookupTable>
    InitDomainLookupTable();

  asiVisu_EXPORT static vtkSmartPointer<vtkLookupTable>
    InitCurvatureCombsLookupTable();

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

  double static Trim(const double val,
                     const double limit)
  {
    double ret_val = val;
    if ( val > limit )
      ret_val = limit;
    else if ( val < -limit )
      ret_val = -limit;

    return ret_val;
  }

private:

  static TPrsAllocMap m_allocMap; //!< Presentation factory.

};

#endif
