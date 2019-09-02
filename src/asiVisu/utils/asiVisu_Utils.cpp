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
//    * Neither the name of the copyright holder(s) nor the
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

// Own include
#include <asiVisu_Utils.h>

// asiVisu includes
#include <asiVisu_ShapePrimitive.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkImageImport.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkPlaneSource.h>
#include <vtkPNGWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#include <vtkWindowToImageFilter.h>
#pragma warning(pop)

// OCCT includes
#include <gp_XYZ.hxx>
#include <Precision.hxx>

//-----------------------------------------------------------------------------
// Presentation factory
//-----------------------------------------------------------------------------

asiVisu_Utils::TPrsAllocMap asiVisu_Utils::m_allocMap;

//-----------------------------------------------------------------------------

asiVisu_Utils::TPrsAllocMap
  asiVisu_Utils::RegisterPrsType(const TCollection_AsciiString& theType,
                                 const asiVisu_PrsAllocator     theAllocFunc)

{
  if ( !m_allocMap.IsBound(theType) )
    m_allocMap.Bind(theType, theAllocFunc);

  return m_allocMap;
}

//-----------------------------------------------------------------------------

const asiVisu_Utils::TPrsAllocMap& asiVisu_Utils::GetAllocMap()
{
  return m_allocMap;
}

//-----------------------------------------------------------------------------
// Managing scene
//-----------------------------------------------------------------------------

//! Calculates bounding box for visible VTK Props presented in the given
//! renderer excluding the passed collection.
//! \param theRenderer    [in]  renderer instance to traverse the Props.
//! \param theBounds      [out] resulting bounding box.
//! \param thePropsToSkip [in]  props to exclude from calculation routine.
//! \return number of props participated in the calculation.
int asiVisu_Utils::ComputeVisiblePropBounds(vtkRenderer*       theRenderer,
                                            double             theBounds[6],
                                            vtkPropCollection* thePropsToSkip)
{
  int aCount = 0;

  theBounds[0] = theBounds[2] = theBounds[4] =  VTK_FLOAT_MAX;
  theBounds[1] = theBounds[3] = theBounds[5] = -VTK_FLOAT_MAX;

  // Iterate over entire collection of Props
  vtkActorCollection* aCollection = theRenderer->GetActors();
  aCollection->InitTraversal();
  while ( vtkActor* aProp = aCollection->GetNextActor() )
  {
    if ( !aProp->GetVisibility() || !aProp->GetMapper() ||
         !vtkMath::AreBoundsInitialized( aProp->GetBounds() ) )
      continue;

    // Check if current Prop is requested not to play in calculations
    if ( thePropsToSkip )
    {
      thePropsToSkip->InitTraversal();
      bool isPropToSkip = false;
      while ( vtkProp* aPropToSkip = thePropsToSkip->GetNextProp() )
      {
        if ( aProp == aPropToSkip )
        {
          isPropToSkip = true;
          break;
        }
      }
      if ( isPropToSkip )
        continue;
    }

    double* aBounds = aProp->GetBounds();
    static double MAX_DISTANCE = 0.9 * VTK_FLOAT_MAX;

    if ( aBounds != NULL &&
         aBounds[0] > -MAX_DISTANCE && aBounds[1] < MAX_DISTANCE &&
         aBounds[2] > -MAX_DISTANCE && aBounds[3] < MAX_DISTANCE &&
         aBounds[4] > -MAX_DISTANCE && aBounds[5] < MAX_DISTANCE )
    {
      aCount++;

      theBounds[0] = Min(aBounds[0], theBounds[0]);
      theBounds[2] = Min(aBounds[2], theBounds[2]);
      theBounds[4] = Min(aBounds[4], theBounds[4]);

      theBounds[1] = Max(aBounds[1], theBounds[1]);
      theBounds[3] = Max(aBounds[3], theBounds[3]);
      theBounds[5] = Max(aBounds[5], theBounds[5]);
    }
  }

  return aCount;
}

//-----------------------------------------------------------------------------

//! Resets camera's parameters. The resulting state of the camera depends
//! on the contents of the active scene.
//! \param theRenderer    [in] renderer instance.
//! \param thePropsToSkip [in] props to ignore when calculating the bounding
//!                            box of the scene.
void asiVisu_Utils::ResetCamera(vtkRenderer*       theRenderer,
                                vtkPropCollection* thePropsToSkip)
{
  static double RESET_COEFF = 3.0;

  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  anActiveCamera->SetPosition(1, -1, 1);
  anActiveCamera->SetViewUp(0, 0, 1);

  if ( AdjustCamera(theRenderer, thePropsToSkip, true) )
    anActiveCamera->SetParallelScale( RESET_COEFF * anActiveCamera->GetParallelScale() );
}

//-----------------------------------------------------------------------------

//! Sets camera's options in order for the presented scene to fit all the
//! applicable viewport.
//! \param theRenderer    [in] renderer instance.
//! \param thePropsToSkip [in] props to ignore when calculating the bounding
//!                            box of the scene.
//! \param doScaling      [in] indicates whether to adjust scale factor as well.
//! \return true if visible Props exist, false -- otherwise.
bool asiVisu_Utils::AdjustCamera(vtkRenderer*       theRenderer,
                                 vtkPropCollection* thePropsToSkip,
                                 bool               isDefaultNorm,
                                 bool               doScaling)
{
  static double MIN_DISTANCE = 1.0 / VTK_FLOAT_MAX;

  if ( !theRenderer )
    return false;

  vtkCamera* pActiveCamera = theRenderer->GetActiveCamera();
  if ( !pActiveCamera )
    return false;

  double bounds[6];
  int numActors = ComputeVisiblePropBounds(theRenderer, bounds, thePropsToSkip);

  if ( numActors )
  {
    double length = bounds[1] - bounds[0];
    length = Max(bounds[3] - bounds[2], length );
    length = Max(bounds[5] - bounds[4], length );

    if ( length < MIN_DISTANCE )
      return false;

    double width =
      Sqrt( (bounds[1] - bounds[0]) * (bounds[1] - bounds[0]) +
            (bounds[3] - bounds[2]) * (bounds[3] - bounds[2]) +
            (bounds[5] - bounds[4]) * (bounds[5] - bounds[4]) );

    if ( width < MIN_DISTANCE )
      return false;

    double viewPlaneNormal[3];
    if ( isDefaultNorm )
    {
      viewPlaneNormal[0] =  0.57;
      viewPlaneNormal[1] = -0.57;
      viewPlaneNormal[2] =  0.57;
    }
    else
      pActiveCamera->GetViewPlaneNormal(viewPlaneNormal);

    double center[3] = {0.0, 0.0, 0.0};
    center[0] = (bounds[0] + bounds[1]) / 2.0;
    center[1] = (bounds[2] + bounds[3]) / 2.0;
    center[2] = (bounds[4] + bounds[5]) / 2.0;
    pActiveCamera->SetFocalPoint(center[0], center[1], center[2]);

    double viewAngle = pActiveCamera->GetViewAngle();
    double distance =
      2 * width / Tan(viewAngle * vtkMath::Pi() / 360.0);

    double aViewUp[3];
    pActiveCamera->GetViewUp(aViewUp);
    if ( Abs( vtkMath::Dot(aViewUp, viewPlaneNormal) ) > 0.999 )
      pActiveCamera->SetViewUp(-aViewUp[2], aViewUp[0], aViewUp[1]);

    // Update the camera
    pActiveCamera->SetPosition(center[0] + distance*viewPlaneNormal[0],
                               center[1] + distance*viewPlaneNormal[1],
                               center[2] + distance*viewPlaneNormal[2]);

    // Find size of the viewport
    int* pViewPortSize = theRenderer->GetSize();
    if ( pViewPortSize[0] < pViewPortSize[1] ) 
      width *= double(pViewPortSize[1]) / double(pViewPortSize[0]);

    if ( doScaling )
      pActiveCamera->SetParallelScale(width / 2.);
  }

  AdjustCameraClippingRange(theRenderer);

  return numActors > 0;
}

//-----------------------------------------------------------------------------

//! Adjusts clipping range of the camera.
//! \param theRenderer [in] Renderer instance.
void asiVisu_Utils::AdjustCameraClippingRange(vtkRenderer* theRenderer)
{
  if ( !theRenderer || !theRenderer->VisibleActorCount() )
    return;

  vtkCamera* activeCamera = theRenderer->GetActiveCamera();
  if ( !activeCamera )
    return;

  // Find the plane equation for the camera view plane
  double aNorm[3];
  activeCamera->GetViewPlaneNormal(aNorm);
  double aPos[3];
  activeCamera->GetPosition(aPos);

  double aBounds[6];
  theRenderer->ComputeVisiblePropBounds(aBounds);

  double aCenter[3];
  aCenter[0] = (aBounds[0] + aBounds[1]) / 2.0;
  aCenter[1] = (aBounds[2] + aBounds[3]) / 2.0;
  aCenter[2] = (aBounds[4] + aBounds[5]) / 2.0;

  double aWidth =
    Sqrt( (aBounds[1] - aBounds[0]) * (aBounds[1] - aBounds[0]) +
          (aBounds[3] - aBounds[2]) * (aBounds[3] - aBounds[2]) +
          (aBounds[5] - aBounds[4]) * (aBounds[5] - aBounds[4]) );

  double aDist =
    Sqrt( (aPos[0] - aCenter[0]) * (aPos[0] - aCenter[0]) +
          (aPos[1] - aCenter[1]) * (aPos[1] - aCenter[1]) +
          (aPos[2] - aCenter[2]) * (aPos[2] - aCenter[2]) );

  double aRange[2] = {aDist - aWidth / 2.0, aDist + aWidth / 2.0};

  if ( aRange[0] < 0.0 )
    aRange[0] = 0.0;

  activeCamera->SetClippingRange(aRange);
}

//-----------------------------------------------------------------------------

//! Puts camera on the top position.
//! \param[in] renderer renderer instance.
void asiVisu_Utils::CameraOnTop(vtkRenderer* renderer)
{
  vtkCamera* activeCamera = renderer->GetActiveCamera();
  activeCamera->SetPosition   (0, 0, 1);
  activeCamera->SetViewUp     (0, 1, 0);
  activeCamera->SetFocalPoint (0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the bottom position.
//! \param[in] renderer renderer instance.
void asiVisu_Utils::CameraOnBottom(vtkRenderer* renderer)
{
  vtkCamera* activeCamera = renderer->GetActiveCamera();
  activeCamera->SetPosition   (0, 0, -1);
  activeCamera->SetViewUp     (0, 1,  0);
  activeCamera->SetFocalPoint (0, 0,  0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the front position.
//! \param[in] renderer renderer instance.
void asiVisu_Utils::CameraOnFront(vtkRenderer* renderer)
{
  vtkCamera* activeCamera = renderer->GetActiveCamera();
  activeCamera->SetPosition   (1, 0, 0);
  activeCamera->SetViewUp     (0, 0, 1);
  activeCamera->SetFocalPoint (0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the back position.
//! \param[in] renderer renderer instance.
void asiVisu_Utils::CameraOnBack(vtkRenderer* renderer)
{
  vtkCamera* activeCamera = renderer->GetActiveCamera();
  activeCamera->SetPosition   (-1, 0, 0);
  activeCamera->SetViewUp     ( 0, 0, 1);
  activeCamera->SetFocalPoint ( 0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the left position.
//! \param[in] renderer renderer instance.
void asiVisu_Utils::CameraOnLeft(vtkRenderer* renderer)
{
  vtkCamera* activeCamera = renderer->GetActiveCamera(); 
  activeCamera->SetPosition   (0, -1, 0);
  activeCamera->SetViewUp     (0,  0, 1);
  activeCamera->SetFocalPoint (0,  0, 0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the right position.
//! \param[in] renderer renderer instance.
void asiVisu_Utils::CameraOnRight(vtkRenderer* renderer)
{
  vtkCamera* activeCamera = renderer->GetActiveCamera();
  activeCamera->SetPosition   (0, 1, 0);
  activeCamera->SetViewUp     (0, 0, 1);
  activeCamera->SetFocalPoint (0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Sets the predefined lighting options to the passed Actor.
//! \param[in] actor actor to adjust lighting options.
void asiVisu_Utils::ApplyLightingRules(vtkActor* actor)
{
  actor->GetProperty()->SetOpacity(1.0);
  actor->GetProperty()->SetAmbient(0.5);
  actor->GetProperty()->SetDiffuse(0.5);
  actor->GetProperty()->SetSpecular(0.5);
  actor->GetProperty()->SetSpecularPower(20);
}

//-----------------------------------------------------------------------------

//! Translates camera from one position to another.
//! \param theRenderer [in] Renderer instance.
//! \param theOldX     [in] old X coordinate.
//! \param theOldY     [in] old Y coordinate.
//! \param theNewX     [in] new X coordinate.
//! \param theNewY     [in] new Y coordinate.
void asiVisu_Utils::TranslateView(vtkRenderer* theRenderer,
                                  const int    theOldX,
                                  const int    theOldY,
                                  const int    theNewX,
                                  const int    theNewY)
{
  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  double aViewFocus[4], aFocalDepth, aViewPoint[3];
  double aNewPickPoint[4], anOldPickPoint[4], aMotionVector[3];
  anActiveCamera->GetFocalPoint(aViewFocus);

  vtkInteractorObserver::ComputeWorldToDisplay(theRenderer,
                                               aViewFocus[0], aViewFocus[1],
                                               aViewFocus[2], aViewFocus);
  aFocalDepth = aViewFocus[2];

  vtkInteractorObserver::ComputeDisplayToWorld(theRenderer,
                                               double(theNewX),
                                               double(theNewY),
                                               aFocalDepth, aNewPickPoint);

  vtkInteractorObserver::ComputeDisplayToWorld(theRenderer,
                                               double(theOldX),
                                               double(theOldY),
                                               aFocalDepth, anOldPickPoint);

  // Camera's motion is reversed
  aMotionVector[0] = anOldPickPoint[0] - aNewPickPoint[0];
  aMotionVector[1] = anOldPickPoint[1] - aNewPickPoint[1];
  aMotionVector[2] = anOldPickPoint[2] - aNewPickPoint[2];

  anActiveCamera->GetFocalPoint(aViewFocus);
  anActiveCamera->GetPosition(aViewPoint);
  anActiveCamera->SetFocalPoint(aMotionVector[0] + aViewFocus[0],
                                aMotionVector[1] + aViewFocus[1],
                                aMotionVector[2] + aViewFocus[2]);
  anActiveCamera->SetPosition(aMotionVector[0] + aViewPoint[0],
                              aMotionVector[1] + aViewPoint[1],
                              aMotionVector[2] + aViewPoint[2]);
}

//-----------------------------------------------------------------------------

//! Modifies the size of the trihedron so that to make its size comparable to
//! the bounding box of the currently rendered scene.
//! \param theRenderer    [in] the scene renderer.
//! \param theActor       [in] the cube axes actor.
//! \param thePropsToSkip [in] the actors to exclude from bounding.
void asiVisu_Utils::AdjustTrihedron(vtkRenderer*       theRenderer,
                                    vtkAxesActor*      theActor,
                                    vtkPropCollection* thePropsToSkip)
{
   // Some static configuration
  static bool   CALC_BY_DIAG = false;
  static double SIZE_FACTOR  = 1.5;
  static double EPS_SIZE     = 5.0e-3;

  // Calculate bounds of presented Actors excluding those ones which comprise
  // the trihedron itself
  double bounds[6];
  ComputeVisiblePropBounds(theRenderer, bounds, thePropsToSkip);

  if ( bounds[0] == VTK_FLOAT_MAX )
    return;

  // Calculate the new length by the bounding box
  double aLength = 0;
  if ( CALC_BY_DIAG )
  {
    aLength =
      Sqrt( (bounds[1] - bounds[0]) * (bounds[1] - bounds[0]) +
            (bounds[3] - bounds[2]) * (bounds[3] - bounds[2]) +
            (bounds[5] - bounds[4]) * (bounds[5] - bounds[4]) );
  }
  else
  {
    aLength = bounds[1] - bounds[0];
    aLength = Max(bounds[3] - bounds[2], aLength);
    aLength = Max(bounds[5] - bounds[4], aLength);
  }
  double anOldSize = theActor->GetTotalLength()[0];
  double aNewSize = aLength * SIZE_FACTOR;

  // Apply new size if it is good enough
  if (  Abs(aNewSize) > EPS_SIZE &&
       ( Abs(aNewSize - anOldSize) > anOldSize * EPS_SIZE ||
         Abs(aNewSize - anOldSize) > aNewSize * EPS_SIZE ) )
  {
    theActor->SetTotalLength(aNewSize, aNewSize, aNewSize);
  }

  // Adjust camera's options
  AdjustCameraClippingRange(theRenderer);
}

//-----------------------------------------------------------------------------

//! Collects image data from the passed render window.
//! \param[in] pRenderWindow render window to get image data from.
//! \return image in memory.
vtkSmartPointer<vtkImageData>
  asiVisu_Utils::GetImage(vtkRenderWindow* pRenderWindow)
{
  vtkSmartPointer<vtkWindowToImageFilter>
    windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  //
  windowToImageFilter->SetInput(pRenderWindow);
  windowToImageFilter->Update();

  vtkSmartPointer<vtkImageData> res = windowToImageFilter->GetOutput();
  return res;
}

//-----------------------------------------------------------------------------

//! Writes the passed image data to a PNG file. The image data should have
//! been obtained with vtkPNGWriter class.
//! \param[in] data     image data to write.
//! \param[in] filename target filename.
void asiVisu_Utils::WritePNG(const vtkSmartPointer<vtkImageData>& data,
                             const char*                          filename)
{
  if ( !data.GetPointer() )
    return;

  vtkSmartPointer<vtkPNGWriter>
    writer = vtkSmartPointer<vtkPNGWriter>::New();
  //
  writer->SetWriteToMemory(0);
  writer->SetFileName(filename);
  writer->SetInputData(data);
  writer->Write();
}

//-----------------------------------------------------------------------------
// Handy shortcuts to commonly used VTK-based functionality
//-----------------------------------------------------------------------------

//! Allocates VTK single-component integer data array with the given name.
//! \param[in] arrName name of array.
//! \return array.
vtkSmartPointer<vtkIntArray> asiVisu_Utils::InitIntArray(const char* arrName)
{
  vtkSmartPointer<vtkIntArray> result = vtkSmartPointer<vtkIntArray>::New();
  result->SetName(arrName);
  result->SetNumberOfComponents(1);
  return result;
}

//-----------------------------------------------------------------------------

//! Allocates VTK single-component real data array with the given name.
//! \param[in] arrName name of array.
//! \return array.
vtkSmartPointer<vtkDoubleArray>
  asiVisu_Utils::InitDoubleArray(const char* arrName)
{
  vtkSmartPointer<vtkDoubleArray> result = vtkSmartPointer<vtkDoubleArray>::New();
  result->SetName(arrName);
  result->SetNumberOfComponents(1);
  return result;
}

//-----------------------------------------------------------------------------

//! Allocates VTK three-component real data array with the given name.
//! \param[in] arrName name of array.
//! \return array.
vtkSmartPointer<vtkDoubleArray>
  asiVisu_Utils::InitDoubleVectorArray(const char* arrName)
{
  vtkSmartPointer<vtkDoubleArray> result = vtkSmartPointer<vtkDoubleArray>::New();
  result->SetName(arrName);
  result->SetNumberOfComponents(3);
  return result;
}

//-----------------------------------------------------------------------------

//! Allocates VTK single-component string data array with the given name.
//! \param[in] arrName name of array.
//! \return array.
vtkSmartPointer<vtkStringArray>
  asiVisu_Utils::InitStringArray(const char* arrName)
{
  vtkSmartPointer<vtkStringArray> result = vtkSmartPointer<vtkStringArray>::New();
  result->SetName(arrName);
  result->SetNumberOfComponents(1);
  return result;
}

//-----------------------------------------------------------------------------

//! Returns default color for picking.
//! \param[out] fR red component [0;1].
//! \param[out] fG green component [0;1].
//! \param[out] fB blue component [0;1].
void asiVisu_Utils::DefaultPickingColor(double& fR,
                                        double& fG,
                                        double& fB)
{
  fR = 1.0;
  fG = 1.0;
  fB = 0.0;
}

//-----------------------------------------------------------------------------

//! Returns default color for highlighting.
//! \param[out] fR red component [0,1].
//! \param[out] fG green component [0,1].
//! \param[out] fB blue component [0,1].
void asiVisu_Utils::DefaultDetectionColor(double& fR,
                                          double& fG,
                                          double& fB)
{
  fR = 130.0 / 255.0;
  fG = 200.0 / 255.0;
  fB = 255.0 / 255.0;
}

//-----------------------------------------------------------------------------

//! \return default line width for picking highlight.
double asiVisu_Utils::DefaultPickLineWidth()
{
  return 5.0;
}

//-----------------------------------------------------------------------------

//! \return default line width for detection highlight.
double asiVisu_Utils::DefaultDetectionLineWidth()
{
  return 5.0;
}

//-----------------------------------------------------------------------------

//! \return default point size for highlighting.
double asiVisu_Utils::DefaultHilightPointSize()
{
  return 8.0;
}

//-----------------------------------------------------------------------------

vtkSmartPointer<vtkLookupTable> asiVisu_Utils::InitLookupTable()
{
  NCollection_DataMap<int, int> customScalarMap;
  const int lastUnusedScalar = ShapePrimitive_LAST;

  return InitLookupTable(customScalarMap, lastUnusedScalar);
}

//-----------------------------------------------------------------------------

vtkSmartPointer<vtkLookupTable>
  asiVisu_Utils::InitLookupTable(const NCollection_DataMap<int, int>& customScalarMap,
                                 const int                            lastUnusedScalar)
{
  vtkSmartPointer<vtkLookupTable>
    colorTable = vtkSmartPointer<vtkLookupTable>::New();

  // Set colors table for 3D shapes
  double range[2] = {ShapePrimitive_Undefined, lastUnusedScalar - 1};
  //
  colorTable->SetRange(range);
  colorTable->SetNumberOfColors(lastUnusedScalar);
  //
  colorTable->SetTableValue(ShapePrimitive_Undefined,       0.0, 0.0, 0.0);
  //
  colorTable->SetTableValue(ShapePrimitive_FreeVertex,      0.75, 0.0, 0.0);
  colorTable->SetTableValue(ShapePrimitive_BorderVertex,    1.0,  0.0, 0.0);
  colorTable->SetTableValue(ShapePrimitive_SharedVertex,    0.0,  0.0, 0.0);
  //
  colorTable->SetTableValue(ShapePrimitive_FreeEdge,        0.75, 0.0, 0.0);
  colorTable->SetTableValue(ShapePrimitive_DanglingEdge,    1.0,  0.0, 0.0);
  colorTable->SetTableValue(ShapePrimitive_BorderEdge,      1.0,  0.0, 0.0);
  colorTable->SetTableValue(ShapePrimitive_ManifoldEdge,    0.1,  0.1, 0.1);
  colorTable->SetTableValue(ShapePrimitive_NonManifoldEdge, 1.0,  1.0, 0.0);
  //
  colorTable->SetTableValue(ShapePrimitive_Facet,           0.9, 0.9, 0.9);
  colorTable->SetTableValue(ShapePrimitive_Isoline,         0.9, 0.0, 0.0);
  //
  colorTable->SetTableValue(ShapePrimitive_Detected,        0.0, 1.0, 1.0);
  colorTable->SetTableValue(ShapePrimitive_Selected,        1.0, 1.0, 0.0);

  // Add extra scalars.
  for ( NCollection_DataMap<int, int>::Iterator it(customScalarMap); it.More(); it.Next() )
  {
    const int colorId = it.Key();
    const int scalar  = it.Value();

    // Convert color.
    QColor color = IntToColor(colorId);
    int r = color.red();
    int g = color.green();
    int b = color.blue();

    // Add scalar mapping to the lookup table.
    colorTable->SetTableValue(scalar, r/255., g/255., b/255.);
  }

  return colorTable;
}

//-----------------------------------------------------------------------------

void asiVisu_Utils::InitShapeMapper(vtkMapper* mapper)
{
  InitShapeMapper( mapper, InitLookupTable() );
}

//-----------------------------------------------------------------------------

void asiVisu_Utils::InitShapeMapper(vtkMapper*      mapper,
                                    vtkLookupTable* colorTable)
{
  mapper->ScalarVisibilityOn();
  mapper->SetScalarModeToUseCellFieldData();
  mapper->SelectColorArray(ARRNAME_PART_CELL_TYPES);
  mapper->SetColorModeToMapScalars();
  mapper->SetScalarRange(colorTable->GetRange());
  mapper->SetLookupTable(colorTable);
  mapper->Update();
}

//-----------------------------------------------------------------------------

void asiVisu_Utils::InitShapeMapper(vtkMapper*                           mapper,
                                    const NCollection_DataMap<int, int>& customScalarMap,
                                    const int                            lastUnusedScalar)
{
  InitShapeMapper( mapper, InitLookupTable(customScalarMap, lastUnusedScalar) );
}

//-----------------------------------------------------------------------------

//! Initializes VTK lookup table charged with default color scheme for p-curves.
//! \return VTK lookup table.
vtkSmartPointer<vtkLookupTable> asiVisu_Utils::InitDomainLookupTable()
{
  vtkSmartPointer<vtkLookupTable> lookup = vtkSmartPointer<vtkLookupTable>::New();

  double range[2] = {VisuOri_Forward, VisuOri_External};
  lookup->SetRange(range);
  lookup->SetNumberOfColors(VisuOri_Last);

  lookup->SetTableValue(VisuOri_Forward,  1.0, 0.0, 0.0);
  lookup->SetTableValue(VisuOri_Reversed, 0.2, 0.4, 1.0);
  lookup->SetTableValue(VisuOri_Internal, 1.0, 1.0, 0.0);
  lookup->SetTableValue(VisuOri_External, 0.0, 1.0, 1.0);

  return lookup;
}

//-----------------------------------------------------------------------------

//! Initializes VTK lookup table charged with a color scheme for curvature combs.
//! \return VTK lookup table.
vtkSmartPointer<vtkLookupTable> asiVisu_Utils::InitCurvatureCombsLookupTable()
{
  vtkSmartPointer<vtkLookupTable> lookup = vtkSmartPointer<vtkLookupTable>::New();

  double range[2] = {VisuCurvComb_PointOk, VisuCurvComb_Envelope};
  lookup->SetRange(range);
  lookup->SetNumberOfColors(VisuCurvComb_Last);

  lookup->SetTableValue(VisuCurvComb_PointOk,      0.0, 0.8, 0.0);
  lookup->SetTableValue(VisuCurvComb_PointFailure, 0.8, 0.0, 0.0);
  lookup->SetTableValue(VisuCurvComb_Comb,         0.6, 0.6, 0.6);
  lookup->SetTableValue(VisuCurvComb_Envelope,     0.0, 0.3, 1.0);

  return lookup;
}

//-----------------------------------------------------------------------------

//! Initializes VTK lookup table charged with a color scheme for knots isos.
//! \return VTK lookup table.
vtkSmartPointer<vtkLookupTable> asiVisu_Utils::InitKnotsIsosLookupTable()
{
  vtkSmartPointer<vtkLookupTable> aLookup = vtkSmartPointer<vtkLookupTable>::New();

  double aRange[2] = {VisuIsosCont_C0, VisuIsosCont_CN};
  aLookup->SetRange(aRange);
  aLookup->SetNumberOfColors(VisuIsosCont_Last);

  aLookup->SetTableValue(VisuIsosCont_C0, 1.0,       0.0,       0.0);
  aLookup->SetTableValue(VisuIsosCont_C1, 1.0,       1.0,       0.0);
  aLookup->SetTableValue(VisuIsosCont_C2, 153./255., 217./255., 1.0);
  aLookup->SetTableValue(VisuIsosCont_CN, 1.0,       1.0,       1.0);

  return aLookup;
}

//-----------------------------------------------------------------------------

//! Initializes VTK lookup table charged with a color scheme for
//! curvilinear axes.
//! \return VTK lookup table.
vtkSmartPointer<vtkLookupTable> asiVisu_Utils::InitCurviAxesLookupTable()
{
  vtkSmartPointer<vtkLookupTable> aLookup = vtkSmartPointer<vtkLookupTable>::New();

  double aRange[2] = {VisuCurviAxis_U, VisuCurviAxis_V};
  aLookup->SetRange(aRange);
  aLookup->SetNumberOfColors(VisuCurviAxis_Last);

  aLookup->SetTableValue(VisuCurviAxis_U, 1.0, 0.0, 0.0);
  aLookup->SetTableValue(VisuCurviAxis_V, 0.0, 1.0, 0.0);

  return aLookup;
}

//-----------------------------------------------------------------------------

//! Initializes the passed VTK mapper with the given Lookup Table.
//! \param[in,out] mapper         mapper to initialize.
//! \param[in]     lokup          lookup table to initialize the mapper with.
//! \param[in]     scalarsArrName name of the array storing the scalars
//!                               for colorization.
void asiVisu_Utils::InitMapper(vtkMapper*      mapper,
                               vtkLookupTable* lookup,
                               const char*     scalarsArrName)
{
  mapper->ScalarVisibilityOn();
  mapper->SetScalarModeToUseCellFieldData();
  mapper->SelectColorArray(scalarsArrName);
  mapper->SetColorModeToMapScalars();
  mapper->SetScalarRange( lookup->GetRange() );
  mapper->SetLookupTable(lookup);
  mapper->Update();
}

//-----------------------------------------------------------------------------

//! Initializes the passed text widget.
//! \param[in] textWidget text widget to initialize.
void asiVisu_Utils::InitTextWidget(vtkTextWidget* textWidget)
{
  vtkTextRepresentation* textRep = vtkTextRepresentation::SafeDownCast( textWidget->GetRepresentation() );
  textWidget->SelectableOff();
  textWidget->SetManagesCursor(0); // To avoid annoying bug with changing cursor at On() method

  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
  textRep->SetTextActor(textActor);
  //
  textRep->GetPositionCoordinate()->SetValue(0.01, 0.01);
  textRep->GetPosition2Coordinate()->SetValue(0.45, 0.35);
  //
  textActor->GetTextProperty()->SetJustificationToLeft();
  textActor->GetTextProperty()->SetVerticalJustificationToBottom();
}
