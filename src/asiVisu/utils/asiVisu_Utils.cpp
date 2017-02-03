//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_Utils.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#pragma warning(pop)

// IVtk includes
#include <IVtkVTK_ShapeData.hxx>

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

  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  if ( !anActiveCamera )
    return false;

  double aBounds[6];
  int anActorCount = ComputeVisiblePropBounds(theRenderer, aBounds, thePropsToSkip);

  if ( anActorCount )
  {
    double aLength = aBounds[1] - aBounds[0];
    aLength = Max(aBounds[3] - aBounds[2], aLength );
    aLength = Max(aBounds[5] - aBounds[4], aLength );

    if ( aLength < MIN_DISTANCE )
      return false;

    double aWidth =
      Sqrt( (aBounds[1] - aBounds[0]) * (aBounds[1] - aBounds[0]) +
            (aBounds[3] - aBounds[2]) * (aBounds[3] - aBounds[2]) +
            (aBounds[5] - aBounds[4]) * (aBounds[5] - aBounds[4]) );

    if ( aWidth < MIN_DISTANCE )
      return false;

    double aViewPlaneNormal[3];
    if ( isDefaultNorm )
    {
      aViewPlaneNormal[0] =  0.57;
      aViewPlaneNormal[1] = -0.57;
      aViewPlaneNormal[2] =  0.57;
    }
    else
      anActiveCamera->GetViewPlaneNormal(aViewPlaneNormal);

    double aCenter[3] = {0.0, 0.0, 0.0};
    aCenter[0] = (aBounds[0] + aBounds[1]) / 2.0;
    aCenter[1] = (aBounds[2] + aBounds[3]) / 2.0;
    aCenter[2] = (aBounds[4] + aBounds[5]) / 2.0;
    anActiveCamera->SetFocalPoint(aCenter[0], aCenter[1], aCenter[2]);

    double aViewAngle = anActiveCamera->GetViewAngle();
    double aDistance =
      2.0 * aWidth / Tan(aViewAngle * vtkMath::Pi() / 360.0);

    double aViewUp[3];
    anActiveCamera->GetViewUp(aViewUp);
    if ( Abs( vtkMath::Dot(aViewUp, aViewPlaneNormal) ) > 0.999 )
      anActiveCamera->SetViewUp(-aViewUp[2], aViewUp[0], aViewUp[1]);

    // Update the camera
    anActiveCamera->SetPosition(aCenter[0] + aDistance*aViewPlaneNormal[0],
                                aCenter[1] + aDistance*aViewPlaneNormal[1],
                                aCenter[2] + aDistance*aViewPlaneNormal[2]);

    // Find size of the viewport
    int* aViewPortSize = theRenderer->GetSize();
    if ( aViewPortSize[0] < aViewPortSize[1] ) 
      aWidth *=
        double(aViewPortSize[1]) / double(aViewPortSize[0]);

    if ( doScaling )
      anActiveCamera->SetParallelScale(aWidth / 2.0);
  }

  AdjustCameraClippingRange(theRenderer);

  return anActorCount > 0;
}

//-----------------------------------------------------------------------------

//! Adjusts clipping range of the camera.
//! \param theRenderer [in] Renderer instance.
void asiVisu_Utils::AdjustCameraClippingRange(vtkRenderer* theRenderer)
{
  if ( !theRenderer || !theRenderer->VisibleActorCount() )
    return;

  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  if ( !anActiveCamera )
    return;

  // Find the plane equation for the camera view plane
  double aNorm[3];
  anActiveCamera->GetViewPlaneNormal(aNorm);
  double aPos[3];
  anActiveCamera->GetPosition(aPos);

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

  anActiveCamera->SetClippingRange(aRange);
}

//-----------------------------------------------------------------------------

//! Puts camera on the top position.
//! \param theRenderer [in] Renderer instance.
void asiVisu_Utils::CameraOnTop(vtkRenderer* theRenderer)
{
  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  anActiveCamera->SetPosition(0, 0, 1);
  anActiveCamera->SetViewUp(0, 1, 0);
  anActiveCamera->SetFocalPoint(0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the bottom position.
//! \param theRenderer [in] Renderer instance.
void asiVisu_Utils::CameraOnBottom(vtkRenderer* theRenderer)
{
  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  anActiveCamera->SetPosition(0, 0, -1);
  anActiveCamera->SetViewUp(0, 1, 0);
  anActiveCamera->SetFocalPoint(0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the front position.
//! \param theRenderer [in] Renderer instance.
void asiVisu_Utils::CameraOnFront(vtkRenderer* theRenderer)
{
  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  anActiveCamera->SetPosition(1, 0, 0);
  anActiveCamera->SetViewUp(0, 0, 1);
  anActiveCamera->SetFocalPoint(0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the back position.
//! \param theRenderer [in] Renderer instance.
void asiVisu_Utils::CameraOnBack(vtkRenderer* theRenderer)
{
  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  anActiveCamera->SetPosition(-1, 0, 0);
  anActiveCamera->SetViewUp(0, 0, 1);
  anActiveCamera->SetFocalPoint(0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the left position.
//! \param theRenderer [in] Renderer instance.
void asiVisu_Utils::CameraOnLeft(vtkRenderer* theRenderer)
{
  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera(); 
  anActiveCamera->SetPosition(0, -1, 0);
  anActiveCamera->SetViewUp(0, 0, 1);
  anActiveCamera->SetFocalPoint(0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Puts camera on the right position.
//! \param theRenderer [in] Renderer instance.
void asiVisu_Utils::CameraOnRight(vtkRenderer* theRenderer)
{
  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  anActiveCamera->SetPosition(0, 1, 0);
  anActiveCamera->SetViewUp(0, 0, 1);
  anActiveCamera->SetFocalPoint(0, 0, 0);
}

//-----------------------------------------------------------------------------

//! Sets the predefined lighting options to the passed Actor.
//! \param actor [in] Actor to adjust lighting options.
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

//! Dumps view contents to Qt Image object.
//! \param theRenderWindow [in] Render Window to dump the contents for.
//! \return resulting Qt Image object.
QImage asiVisu_Utils::DumpView(vtkRenderWindow* theRenderWindow)
{
  int* aSize = theRenderWindow->GetSize();
  int aWidth = aSize[0];
  int aHeight = aSize[1];

  unsigned char* aData =
    theRenderWindow->GetRGBACharPixelData(0, 0, aWidth - 1, aHeight - 1, 1);

  QImage anImage(aData, aWidth, aHeight, QImage::Format_ARGB32);

  anImage = anImage.rgbSwapped();
  anImage = anImage.mirrored();

  return anImage;
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
  double aBounds[6];
  ComputeVisiblePropBounds(theRenderer, aBounds, thePropsToSkip);

  if ( aBounds[0] == VTK_FLOAT_MAX )
    return;

  // Calculate the new length by the bounding box
  double aLength = 0;
  if ( CALC_BY_DIAG )
  {
    aLength =
      Sqrt( (aBounds[1] - aBounds[0]) * (aBounds[1] - aBounds[0]) +
            (aBounds[3] - aBounds[2]) * (aBounds[3] - aBounds[2]) +
            (aBounds[5] - aBounds[4]) * (aBounds[5] - aBounds[4]) );
  }
  else {
    aLength = aBounds[1] - aBounds[0];
    aLength = Max(aBounds[3] - aBounds[2], aLength);
    aLength = Max(aBounds[5] - aBounds[4], aLength);
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
// Handy shortcuts to commonly used VTK-based functionality
//-----------------------------------------------------------------------------

//! Allocates VTK single-component integer data array with the given name.
//! \param theArrName [in] name of array.
//! \return array.
vtkSmartPointer<vtkIntArray> asiVisu_Utils::InitIntArray(const char* theArrName)
{
  vtkSmartPointer<vtkIntArray> aResult = vtkSmartPointer<vtkIntArray>::New();
  aResult->SetName(theArrName);
  aResult->SetNumberOfComponents(1);
  return aResult;
}

//-----------------------------------------------------------------------------

//! Allocates VTK single-component real data array with the given name.
//! \param theArrName [in] name of array.
//! \return array.
vtkSmartPointer<vtkDoubleArray> asiVisu_Utils::InitDoubleArray(const char* theArrName)
{
  vtkSmartPointer<vtkDoubleArray> aResult = vtkSmartPointer<vtkDoubleArray>::New();
  aResult->SetName(theArrName);
  aResult->SetNumberOfComponents(1);
  return aResult;
}

//-----------------------------------------------------------------------------

//! Allocates VTK three-component real data array with the given name.
//! \param theArrName [in] name of array.
//! \return array.
vtkSmartPointer<vtkDoubleArray>
  asiVisu_Utils::InitDoubleVectorArray(const char* theArrName)
{
  vtkSmartPointer<vtkDoubleArray> aResult = vtkSmartPointer<vtkDoubleArray>::New();
  aResult->SetName(theArrName);
  aResult->SetNumberOfComponents(3);
  return aResult;
}

//-----------------------------------------------------------------------------

//! Allocates VTK single-component string data array with the given name.
//! \param theArrName [in] name of array.
//! \return array.
vtkSmartPointer<vtkStringArray>
  asiVisu_Utils::InitStringArray(const char* theArrName)
{
  vtkSmartPointer<vtkStringArray> aResult = vtkSmartPointer<vtkStringArray>::New();
  aResult->SetName(theArrName);
  aResult->SetNumberOfComponents(1);
  return aResult;
}

//-----------------------------------------------------------------------------

//! Returns default color for picking.
//! \param fR [out] red component [0;1].
//! \param fG [out] green component [0;1].
//! \param fB [out] blue component [0;1].
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
//! \param fR [out] red component [0,1].
//! \param fG [out] green component [0,1].
//! \param fB [out] blue component [0,1].
void asiVisu_Utils::DefaultDetectionColor(double& fR,
                                          double& fG,
                                          double& fB)
{
  fR = 130.0 / 255.0;
  fG = 200.0 / 255.0;
  fB = 255.0 / 255.0;
}

//-----------------------------------------------------------------------------

//! Returns default line width for picking highlight.
//! \return default line width for picking highlight.
double asiVisu_Utils::DefaultPickLineWidth()
{
  return 5.0;
}

//-----------------------------------------------------------------------------

//! Returns default line width for detection highlight.
//! \return default line width for detection highlight.
double asiVisu_Utils::DefaultDetectionLineWidth()
{
  return 5.0;
}

//-----------------------------------------------------------------------------

//! Returns default point size for highlighting.
//! \return default point size for highlighting.
double asiVisu_Utils::DefaultHilightPointSize()
{
  return 8.0;
}

//-----------------------------------------------------------------------------

vtkLookupTable* asiVisu_Utils::InitLookupTable()
{
  vtkLookupTable* aColorTable = vtkLookupTable::New();

  // Set colors table for 3D shapes
  double aRange[2];
  aRange[0] = MT_Undefined;
  aRange[1] = MT_ShadedFace;
  aColorTable->Allocate(9);
  aColorTable->SetNumberOfTableValues(9);
  aColorTable->SetTableRange(aRange);
  aColorTable->SetValueRange(0, 1);

/*
  MT_Undefined     = -1   Undefined
  MT_IsoLine       =  0   IsoLine
  MT_FreeVertex    =  1   Free vertex
  MT_SharedVertex  =  2   Shared vertex
  MT_FreeEdge      =  3   Free edge
  MT_BoundaryEdge  =  4   Boundary edge (related to a single face)
  MT_SharedEdge    =  5   Shared edge (related to several faces)
  MT_WireFrameFace =  6   Wireframe face
  MT_ShadedFace    =  7   Shaded face
*/

  aColorTable->SetTableValue(0, 0, 0, 0);          // undefined
  aColorTable->SetTableValue(1, 0.5, 0.5, 0.5);    // gray for isoparametric line in wireframe
  aColorTable->SetTableValue(2, 1, 0, 0);          // red for free vertex
  aColorTable->SetTableValue(3, 0, 0, 1);          // blue for shared vertex
  aColorTable->SetTableValue(4, 1, 0, 0);          // red for free edge
  aColorTable->SetTableValue(5, 1, 0, 0);          // red for boundary edge (related to a single face)
  aColorTable->SetTableValue(6, 0, 0, 0);          // black for shared edge (related to several faces)
  aColorTable->SetTableValue(7, 0.05, 0.55, 0.85); // face in shading
  aColorTable->SetTableValue(8, 1, 1, 1);          // solid in shading

  return aColorTable;
}

//-----------------------------------------------------------------------------

void asiVisu_Utils::SetLookupTableColor(vtkLookupTable*     theColorTable,
                                        const IVtk_MeshType theColorRole,
                                        const double        theR,
                                        const double        theG,
                                        const double        theB,
                                        const double        theA)
{
  asiVisu_NotUsed(theA);
  theColorTable->SetTableValue(theColorRole + 1, theR, theG, theB);
}

//-----------------------------------------------------------------------------

void asiVisu_Utils::GetLookupTableColor(vtkLookupTable*     theColorTable,
                                        const IVtk_MeshType theColorRole,
                                        double&             theR,
                                        double&             theG,
                                        double&             theB)
{
  double aRgb[3];
  theColorTable->GetColor(theColorRole + 1, aRgb);
  theR = aRgb[0];
  theG = aRgb[1];
  theB = aRgb[2];
}

//-----------------------------------------------------------------------------

void asiVisu_Utils::GetLookupTableColor(vtkLookupTable*     theColorTable,
                                        const IVtk_MeshType theColorRole,
                                        double&             theR,
                                        double&             theG,
                                        double&             theB,
                                        double&             theA)
{
  theA = theColorTable->GetOpacity (theColorRole + 1);
  GetLookupTableColor(theColorTable, theColorRole, theR, theG, theB);
}

//-----------------------------------------------------------------------------

void asiVisu_Utils::InitShapeMapper(vtkMapper* theMapper)
{
  InitShapeMapper( theMapper, InitLookupTable() );
}

//-----------------------------------------------------------------------------

void asiVisu_Utils::InitShapeMapper(vtkMapper*      theMapper,
                                    vtkLookupTable* theColorTable)
{
  theMapper->ScalarVisibilityOn();
  theMapper->SetScalarModeToUseCellFieldData();
  theMapper->SelectColorArray(IVtkVTK_ShapeData::ARRNAME_MESH_TYPES);
  theMapper->SetColorModeToMapScalars();
  theMapper->SetScalarRange(theColorTable->GetRange());
  theMapper->SetLookupTable(theColorTable);
  theMapper->Update();
}

//-----------------------------------------------------------------------------

//! Initializes VTK lookup table charged with default color scheme for p-curves.
//! \return VTK lookup table.
vtkSmartPointer<vtkLookupTable> asiVisu_Utils::InitDomainLookupTable()
{
  vtkSmartPointer<vtkLookupTable> aLookup = vtkSmartPointer<vtkLookupTable>::New();

  double aRange[3] = {0, VisuOri_External};
  aLookup->SetRange(aRange);
  aLookup->SetNumberOfColors(VisuOri_Last);

  aLookup->SetTableValue(VisuOri_Forward,  1.0, 0.0, 0.0);
  aLookup->SetTableValue(VisuOri_Reversed, 0.2, 0.4, 1.0);
  aLookup->SetTableValue(VisuOri_Internal, 1.0, 1.0, 0.0);
  aLookup->SetTableValue(VisuOri_External, 0.0, 1.0, 1.0);

  return aLookup;
}

//-----------------------------------------------------------------------------

//! Initializes the passed VTK mapper with the given Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theLookup         [in]     Lookup Table to initialize the mapper with.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
void asiVisu_Utils::InitMapper(vtkMapper*      theMapper,
                               vtkLookupTable* theLookup,
                               const char*     theScalarsArrName)
{
  theMapper->ScalarVisibilityOn();
  theMapper->SetScalarModeToUseCellFieldData();
  theMapper->SelectColorArray(theScalarsArrName);
  theMapper->SetColorModeToMapScalars();
  theMapper->SetScalarRange( theLookup->GetRange() );
  theMapper->SetLookupTable(theLookup);
  theMapper->Update();
}

//-----------------------------------------------------------------------------

//! Initializes the passed text widget.
//! \param theScalarBarWidget [in] scalar bar widget to initialize.
void asiVisu_Utils::InitTextWidget(vtkTextWidget* theTextWidget)
{
  vtkTextRepresentation* textRep = vtkTextRepresentation::SafeDownCast( theTextWidget->GetRepresentation() );
  theTextWidget->SelectableOff();

  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
  textRep->SetTextActor(textActor);
  //
  textRep->GetPositionCoordinate()->SetValue(0.01, 0.01);
  textRep->GetPosition2Coordinate()->SetValue(0.45, 0.35);
  //
  textActor->GetTextProperty()->SetJustificationToLeft();
  textActor->GetTextProperty()->SetVerticalJustificationToBottom();
}
