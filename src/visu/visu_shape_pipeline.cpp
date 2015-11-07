//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su
//-----------------------------------------------------------------------------

#include <SbmVisu_ShapePipeline.h>

// SBM Visualization Framework includes
#include <SbmVisu_NodeInformation.h>

// SBM algorithmic collection includes
#include <SbmAlgo_ShapeUtils.h>

// SBM Data Framework includes
#include <ActData_ParameterFactory.h>
#include <ActData_ShapeParameter.h>

// VIS includes
#pragma warning(push, 0)
#include <IVtkOCC/IVtkOCC_Shape.hxx>
#include <IVtkTools/IVtkTools_DisplayModeFilter.hxx>
#include <IVtkTools/IVtkTools_ShapeDataSource.hxx>
#include <IVtkTools/IVtkTools_ShapeObject.hxx>
#include <IVtkTools/IVtkTools_SubPolyDataFilter.hxx>
#pragma warning(pop)

// VTK includes
#include <vtkActor.h>
#include <vtkGeometryFilter.h>
#include <vtkInformation.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>

// OCCT includes
#include <Standard/Standard_ProgramError.hxx>
#include <TColStd/TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopLoc/TopLoc_Location.hxx>

#undef DEBUG_SHOW_TRIANGULATION

//-----------------------------------------------------------------------------
// Shape Pipeline
//-----------------------------------------------------------------------------

//! Creates new Shape Pipeline initialized by default VTK mapper and actor.
//! \param isOCCTColorScheme [in] indicates whether to use native OCCT
//!        color scheme for rendering of OCCT shapes.
//! \param isBound2Node [in] indicates whether to bind Node ID to actor.
//! \param isSecondary [in] indicates whether the pipeline is secondary.
SbmVisu_ShapePipeline::SbmVisu_ShapePipeline(const Standard_Boolean isOCCTColorScheme,
                                             const Standard_Boolean isBound2Node,
                                             const Standard_Boolean isSecondary)
: SbmVisu_BasePipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                        vtkSmartPointer<vtkActor>::New() ),
  m_bOCCTColorScheme(isOCCTColorScheme),
  m_bIsBound2Node(isBound2Node),
  m_bIsSecondary(isSecondary),
  m_bMapperColorsSet(Standard_False),
  m_bShadingMode(Standard_True), // Shading by default
  m_bWireframeMode(!m_bShadingMode),
  m_bSubShapesVoid(Standard_False)
{
  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Allocate Data Source
  vtkSmartPointer<IVtkTools_ShapeDataSource>
    aDS = vtkSmartPointer<IVtkTools_ShapeDataSource>::New();

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
}

//! Sets input data for the pipeline.
//! \param theDataProvider [in] Data Provider.
void SbmVisu_ShapePipeline::SetInput(const Handle(SbmAPI_IPipelineDataProvider)& theDataProvider)
{
  Handle(SbmVisu_ShapeDataProvider)
    aShapePrv = Handle(SbmVisu_ShapeDataProvider)::DownCast(theDataProvider);

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
      aDataToKeep.Add( it.Key() );

  Standard_Boolean doFiltering;
  if ( m_bSubShapesVoid )
    doFiltering = Standard_True;
  else
    doFiltering = !aDataToKeep.IsEmpty();

  aSubShapesFilter->SetDoFiltering(doFiltering ? true : false);
  aSubShapesFilter->SetData(aDataToKeep);
  aSubShapesFilter->Modified();

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( aShapePrv->MustExecute( this->GetMTime() ) && !m_bIsSecondary )
  {
    // Access transformation to be applied
    Standard_Real aPosX, aPosY, aPosZ, anAngA, anAngB, anAngC;
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
    aShape = SbmAlgo_ShapeUtils::ApplyTransformation(aShape, aPosX, aPosY, aPosZ,
                                                     anAngA, anAngB, anAngC);

    static Standard_Integer ShapeID = 0; ++ShapeID;
    Handle(IVtkOCC_Shape) aShapeIntoVtk = new IVtkOCC_Shape(aShape);
    aShapeIntoVtk->SetId(ShapeID);

    // Re-use existing Data Source to benefit from its lightweight
    // transformation capabilities
    IVtkTools_ShapeDataSource*
      aShapeDS = IVtkTools_ShapeDataSource::SafeDownCast( m_filterMap(Filter_Source) );
    aShapeDS->SetShape(aShapeIntoVtk);

    // Bind actor to owning Node ID. Thus we set back reference from VTK
    // entity to data object
    if ( m_bIsBound2Node )
      SbmVisu_NodeInformation::Store( aShapePrv->GetNodeID(), this->Actor() );

    // Bind Shape DS with actor. This is necessary for VIS picker -- it will
    // not work without the corresponding Information key
    IVtkTools_ShapeObject::SetShapeSource( aShapeDS, this->Actor() );

    // Initialize pipeline
    this->SetInputConnection( aShapeDS->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//! Returns true if the pipeline is configured to work in SHADING mode
//! currently, false -- otherwise.
//! \return true/false.
Standard_Boolean SbmVisu_ShapePipeline::IsShadingMode() const
{
  return m_bShadingMode;
}

//! Switches ON SHADING visualization mode.
void SbmVisu_ShapePipeline::ShadingModeOn()
{
  if ( m_bShadingMode )
    return;

  IVtkTools_DisplayModeFilter*
    aDMFilter = IVtkTools_DisplayModeFilter::SafeDownCast( m_filterMap.Find(Filter_DM) );

  aDMFilter->SetDisplayMode(DM_Shading);

  m_bShadingMode = Standard_True;
  m_bWireframeMode = Standard_False;
}

//! Returns true if the pipeline is configured to work in WIREFRAME mode
//! currently, false -- otherwise.
//! \return true/false.
Standard_Boolean SbmVisu_ShapePipeline::IsWireframeMode() const
{
  return m_bWireframeMode;
}

//! Switches ON WIREFRAME visualization mode.
void SbmVisu_ShapePipeline::WireframeModeOn()
{
  if ( m_bWireframeMode )
    return;

  IVtkTools_DisplayModeFilter*
    aDMFilter = IVtkTools_DisplayModeFilter::SafeDownCast( m_filterMap.Find(Filter_DM) );

  aDMFilter->SetDisplayMode(DM_Wireframe);

  m_bShadingMode = Standard_False;
  m_bWireframeMode = Standard_True;
}

//! Cleans up the underlying sub-shapes filter, however, keeps it chained.
void SbmVisu_ShapePipeline::VoidSubShapesOn()
{
  IVtkTools_SubPolyDataFilter*
    aSubShapesFilter = IVtkTools_SubPolyDataFilter::SafeDownCast( m_filterMap(Filter_SubShapes) );

  aSubShapesFilter->Clear();
  aSubShapesFilter->SetDoFiltering(true);
  aSubShapesFilter->Modified();

  m_bSubShapesVoid = Standard_True;
}

//! Enables sub-shapes filtering.
void SbmVisu_ShapePipeline::VoidSubShapesOff()
{
  m_bSubShapesVoid = Standard_False;
}

//! Enables shared vertices in Display Mode filter.
void SbmVisu_ShapePipeline::SharedVerticesOn()
{
  IVtkTools_DisplayModeFilter*
    aDMFilter = IVtkTools_DisplayModeFilter::SafeDownCast( m_filterMap(Filter_DM) );

  aDMFilter->SetDisplaySharedVertices(true);
  aDMFilter->Modified();
}

//! Disables shared vertices in Display Mode filter.
void SbmVisu_ShapePipeline::SharedVerticesOff()
{
  IVtkTools_DisplayModeFilter*
    aDMFilter = IVtkTools_DisplayModeFilter::SafeDownCast( m_filterMap(Filter_DM) );

  aDMFilter->SetDisplaySharedVertices(false);
  aDMFilter->Modified();
}

//! Accessor for the used shape Data Source.
//! \return shape Data Source.
vtkPolyDataAlgorithm* SbmVisu_ShapePipeline::DataSource() const
{
  return vtkPolyDataAlgorithm::SafeDownCast( m_filterMap(Filter_Source) );
}

//! Callback for AddToRenderer base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void SbmVisu_ShapePipeline::addToRendererCallback(vtkRenderer*)
{
#ifdef DEBUG_SHOW_TRIANGULATION
  this->Actor()->GetProperty()->SetEdgeVisibility(1);
#endif
  this->Actor()->GetProperty()->SetInterpolationToPhong();
}

//! Callback for RemoveFromRenderer base routine.
void SbmVisu_ShapePipeline::removeFromRendererCallback(vtkRenderer*)
{
}

//! Callback for Update routine.
void SbmVisu_ShapePipeline::updateCallback()
{
  if ( m_bOCCTColorScheme && !m_bMapperColorsSet )
  {
    IVtkTools::InitShapeMapper(m_mapper);
    m_bMapperColorsSet = Standard_True;
  }
}
