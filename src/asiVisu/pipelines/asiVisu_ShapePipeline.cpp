//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_ShapePipeline.h>

// Visualization includes
#include <asiVisu_NodeInfo.h>
#include <asiVisu_ShapeDataProvider.h>
#include <asiVisu_ShapeDataSource.h>
#include <asiVisu_Utils.h>

// Modeling includes
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>
#include <ActData_ShapeParameter.h>

// VIS includes
#pragma warning(push, 0)
#include <IVtkOCC_Shape.hxx>
#include <IVtkTools_DisplayModeFilter.hxx>
#include <IVtkTools_ShapeObject.hxx>
#include <IVtkTools_SubPolyDataFilter.hxx>
#pragma warning(pop)

// VTK includes
#include <vtkActor.h>
#include <vtkGeometryFilter.h>
#include <vtkInformation.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>

// OCCT includes
#include <Standard_ProgramError.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopLoc_Location.hxx>

//! Creates new Shape Pipeline initialized by default VTK mapper and actor.
//! \param isOCCTColorScheme [in] indicates whether to use native OCCT
//!                               color scheme for rendering of OCCT shapes.
//! \param isBound2Node      [in] indicates whether to bind Node ID to actor.
//! \param isSecondary       [in] indicates whether the pipeline is secondary.
//! \param isTrianglesMode   [in] indicates whether to show triangulation or not.
//! \param pSource           [in] Data Source for secondary pipelines.
asiVisu_ShapePipeline::asiVisu_ShapePipeline(const bool            isOCCTColorScheme,
                                             const bool            isBound2Node,
                                             const bool            isSecondary,
                                             const bool            isTrianglesMode,
                                             vtkPolyDataAlgorithm* pSource)
//
: asiVisu_Pipeline   ( vtkSmartPointer<vtkPolyDataMapper>::New(), vtkSmartPointer<vtkActor>::New() ),
  m_bOCCTColorScheme ( isOCCTColorScheme ), // Native OCCT color scheme (as in Draw)
  m_bIsBound2Node    ( isBound2Node ),
  m_bIsSecondary     ( isSecondary ),
  m_bMapperColorsSet ( false ),
  m_bShadingMode     ( true ),              // Shading by default
  m_bWireframeMode   ( !m_bShadingMode ),
  m_bSubShapesVoid   ( false ),
  m_bShowTriangles   ( isTrianglesMode )
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate Data Source
  vtkSmartPointer<asiVisu_ShapeDataSource>
    aDS = m_bIsSecondary ? asiVisu_ShapeDataSource::SafeDownCast(pSource)
                         : vtkSmartPointer<asiVisu_ShapeDataSource>::New();

  // Use optimized approach to isotropic transformations
  aDS->FastTransformModeOn();

  // Allocate Display Mode filter
  vtkSmartPointer<IVtkTools_DisplayModeFilter>
    aDMFilter = vtkSmartPointer<IVtkTools_DisplayModeFilter>::New();

  // Process initial flags
  aDMFilter->SetDisplayMode(m_bShadingMode ? DM_Shading : DM_Wireframe);

  // Allocate Sub-shapes filter
  vtkSmartPointer<IVtkTools_SubPolyDataFilter>
    aSubShapeFilter = vtkSmartPointer<IVtkTools_SubPolyDataFilter>::New();

  // Allocate Normals Calculation filter
  vtkSmartPointer<vtkPolyDataNormals>
    aNormalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_Source,    aDS);
  m_filterMap.Bind(Filter_SubShapes, aSubShapeFilter);
  m_filterMap.Bind(Filter_DM,        aDMFilter);
  m_filterMap.Bind(Filter_Normals,   aNormalsFilter);

  // Append custom filters to the pipeline
  this->append( m_filterMap.Find(Filter_DM) );
  this->append( m_filterMap.Find(Filter_SubShapes) );
  this->append( m_filterMap.Find(Filter_Normals) ); // Goes after DM as DM looses normals data

  // Set line width
  this->Actor()->GetProperty()->SetLineWidth(1);
  //
  asiVisu_Utils::ApplyLightingRules( this->Actor() );
}

//! Sets input data for the pipeline.
//! \param theDataProvider [in] Data Provider.
void asiVisu_ShapePipeline::SetInput(const Handle(asiVisu_DataProvider)& theDataProvider)
{
  Handle(asiVisu_ShapeDataProvider)
    aShapePrv = Handle(asiVisu_ShapeDataProvider)::DownCast(theDataProvider);

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  TopoDS_Shape aShape = aShapePrv->GetShape();
  if ( aShape.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> aDummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(aDummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ==============================
   *  Initialize sub-shapes filter
   * ============================== */

  IVtkTools_SubPolyDataFilter*
    aSubShapesFilter = IVtkTools_SubPolyDataFilter::SafeDownCast( m_filterMap(Filter_SubShapes) );

  IVtk_IdTypeMap aDataToKeep;
  Handle(TColStd_HPackedMapOfInteger) aSubShapes = aShapePrv->GetSubShapes();

  if ( !aSubShapes.IsNull() )
    for ( TColStd_MapIteratorOfPackedMapOfInteger it( aSubShapes->Map() ); it.More(); it.Next() )
    {
      const int subShapeId = it.Key();
      aDataToKeep.Add(subShapeId);
    }

  bool doFiltering;
  if ( m_bSubShapesVoid )
    doFiltering = true;
  else
    doFiltering = !aDataToKeep.IsEmpty();

  aSubShapesFilter->SetDoFiltering(doFiltering);
  aSubShapesFilter->SetData(aDataToKeep);
  aSubShapesFilter->Modified();

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aShapePrv->MustExecute( this->GetMTime() ) )
  {
    // Active mesh source for geometry
    asiVisu_ShapeDataSource*
      aShapeDS = asiVisu_ShapeDataSource::SafeDownCast( m_filterMap(Filter_Source) );

    // Two modes: affect or not data source
    if ( !m_bIsSecondary )
    {
      // Access transformation to be applied
      double aPosX, aPosY, aPosZ, anAngA, anAngB, anAngC;
      if ( aShapePrv->HasPosition() )
      {
        aShapePrv->GetPosition(aPosX, aPosY, aPosZ);
        if ( aShapePrv->HasPositionAndRotation() )
          aShapePrv->GetRotation(anAngA, anAngB, anAngC);
        else
          anAngA = anAngB = anAngC = 0.0;
      }
      else
        aPosX = aPosY = aPosZ = anAngA = anAngB = anAngC = 0.0;

      // This is bad transformation ;)
      if ( aPosX == DBL_MAX || aPosY == DBL_MAX || aPosZ == DBL_MAX ||
           anAngA == DBL_MAX || anAngB == DBL_MAX || anAngC == DBL_MAX )
        return;

      // Set shape location. Notice that we do not use vtkTransform or any
      // other transformation filter in our Shape Pipeline as it will totally
      // ruin the picker's correctness (when VIS picker is active)
      aShape = asiAlgo_Utils::ApplyTransformation(aShape, aPosX, aPosY, aPosZ,
                                                  anAngA, anAngB, anAngC, false);

      static int ShapeID = 0; ++ShapeID;
      Handle(IVtkOCC_Shape) aShapeIntoVtk = new IVtkOCC_Shape(aShape);
      aShapeIntoVtk->SetId(ShapeID);

      // Re-use existing Data Source to benefit from its lightweight
      // transformation capabilities
      aShapeDS->SetShape(aShapeIntoVtk);

      // Bind actor to owning Node ID. Thus we set back reference from VTK
      // entity to data object
      if ( m_bIsBound2Node )
        asiVisu_NodeInfo::Store( aShapePrv->GetNodeID(), this->Actor() );

      // Bind Shape DS with actor. This is necessary for VIS picker -- it will
      // not work without the corresponding Information key
      IVtkTools_ShapeObject::SetShapeSource( aShapeDS, this->Actor() );
    }

    // Initialize pipeline
    this->SetInputConnection( aShapeDS->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Returns true if the pipeline is configured to work in SHADING mode
//! currently, false -- otherwise.
//! \return true/false.
bool asiVisu_ShapePipeline::IsShadingMode() const
{
  return m_bShadingMode;
}

//! Switches ON SHADING visualization mode.
void asiVisu_ShapePipeline::ShadingModeOn()
{
  if ( m_bShadingMode )
    return;

  IVtkTools_DisplayModeFilter*
    aDMFilter = IVtkTools_DisplayModeFilter::SafeDownCast( m_filterMap.Find(Filter_DM) );

  aDMFilter->SetDisplayMode(DM_Shading);

  m_bShadingMode = true;
  m_bWireframeMode = false;
}

//! Returns true if the pipeline is configured to work in WIREFRAME mode
//! currently, false -- otherwise.
//! \return true/false.
bool asiVisu_ShapePipeline::IsWireframeMode() const
{
  return m_bWireframeMode;
}

//! Switches ON WIREFRAME visualization mode.
void asiVisu_ShapePipeline::WireframeModeOn()
{
  if ( m_bWireframeMode )
    return;

  IVtkTools_DisplayModeFilter*
    aDMFilter = IVtkTools_DisplayModeFilter::SafeDownCast( m_filterMap.Find(Filter_DM) );

  aDMFilter->SetDisplayMode(DM_Wireframe);

  m_bShadingMode = false;
  m_bWireframeMode = true;
}

//! Cleans up the underlying sub-shapes filter, however, keeps it chained.
void asiVisu_ShapePipeline::VoidSubShapesOn()
{
  IVtkTools_SubPolyDataFilter*
    aSubShapesFilter = IVtkTools_SubPolyDataFilter::SafeDownCast( m_filterMap(Filter_SubShapes) );

  aSubShapesFilter->Clear();
  aSubShapesFilter->SetDoFiltering(true);
  aSubShapesFilter->Modified();

  m_bSubShapesVoid = true;
}

//! Enables sub-shapes filtering.
void asiVisu_ShapePipeline::VoidSubShapesOff()
{
  m_bSubShapesVoid = false;
}

//! Enables shared vertices in Display Mode filter.
void asiVisu_ShapePipeline::SharedVerticesOn()
{
  IVtkTools_DisplayModeFilter*
    aDMFilter = IVtkTools_DisplayModeFilter::SafeDownCast( m_filterMap(Filter_DM) );

  aDMFilter->SetDisplaySharedVertices(true);
  aDMFilter->Modified();
}

//! Disables shared vertices in Display Mode filter.
void asiVisu_ShapePipeline::SharedVerticesOff()
{
  IVtkTools_DisplayModeFilter*
    aDMFilter = IVtkTools_DisplayModeFilter::SafeDownCast( m_filterMap(Filter_DM) );

  aDMFilter->SetDisplaySharedVertices(false);
  aDMFilter->Modified();
}

//! Accessor for the used shape Data Source.
//! \return shape Data Source.
vtkPolyDataAlgorithm* asiVisu_ShapePipeline::DataSource() const
{
  return vtkPolyDataAlgorithm::SafeDownCast( m_filterMap(Filter_Source) );
}

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_ShapePipeline::callback_add_to_renderer(vtkRenderer*)
{
  if ( m_bShowTriangles )
    this->Actor()->GetProperty()->SetEdgeVisibility(1);
  else
    this->Actor()->GetProperty()->SetInterpolationToPhong();
}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_ShapePipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update() routine.
void asiVisu_ShapePipeline::callback_update()
{
  if ( m_bOCCTColorScheme && !m_bMapperColorsSet )
  {
    asiVisu_Utils::InitShapeMapper(m_mapper);
    m_bMapperColorsSet = true;
  }
}
