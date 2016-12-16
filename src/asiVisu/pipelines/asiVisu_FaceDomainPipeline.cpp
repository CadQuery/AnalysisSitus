//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_FaceDomainPipeline.h>

// Visualization includes
#include <asiVisu_FaceDataProvider.h>
#include <asiVisu_NodeInfo.h>
#include <asiVisu_PDomainSource.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OCCT includes
#include <BRepTools.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//-----------------------------------------------------------------------------

//! Creates new Face Domain Pipeline initialized by default VTK mapper and actor.
//! \param isDefaultColorScheme [in] indicates whether to use default color scheme.
asiVisu_FaceDomainPipeline::asiVisu_FaceDomainPipeline(const bool isDefaultColorScheme)
//
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() ),
//
  m_bDefaultColorScheme (isDefaultColorScheme),
  m_bMapperColorsSet    (false),
  m_bForced             (false)
{
  this->Actor()->GetProperty()->SetLineWidth(2.0);

  //---------------------------------------------------------------------------

  // Selected items
  m_selected = vtkSmartPointer<vtkIdTypeArray>::New();

  // Selection node
  m_selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
  m_selectionNode->SetFieldType(vtkSelectionNode::CELL);
  m_selectionNode->SetContentType(vtkSelectionNode::PEDIGREEIDS);

  // Selection
  m_selection = vtkSmartPointer<vtkSelection>::New();
  m_selection->AddNode(m_selectionNode);

  // Selection extraction
  m_extractSelection = vtkSmartPointer<vtkExtractSelection>::New();

  // Geometry filter to convert unstructured selection results to poly data
  m_toPolyData = vtkSmartPointer<vtkGeometryFilter>::New();
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
void asiVisu_FaceDomainPipeline::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  Handle(asiVisu_FaceDataProvider)
    faceProvider = Handle(asiVisu_FaceDataProvider)::DownCast(DP);

  /* =================
   *  Validate inputs
   * ================= */

  const int face_idx = faceProvider->GetFaceIndexAmongSubshapes();
  if ( !face_idx )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> aDummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(aDummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( m_bForced || faceProvider->MustExecute( this->GetMTime() ) )
  {
    if ( m_bForced ) m_bForced = false; // Executed, reset forced

    // Bind to Node
    asiVisu_NodeInfo::Store( faceProvider->GetNodeID(), this->Actor() );

    // Append filter
    vtkSmartPointer<asiVisu_PDomainSource>
      source = vtkSmartPointer<asiVisu_PDomainSource>::New();
    //
    source->SetFace( faceProvider->ExtractFace() );

    // Apply selection
    if ( m_selected->GetSize() )
    {
      m_extractSelection->SetInputConnection( 0, source->GetOutputPort() );
      m_extractSelection->SetInputData(1, m_selection);
      m_toPolyData->SetInputConnection( m_extractSelection->GetOutputPort() );

      ///
      m_toPolyData->Update();
      std::cout << "\t\t\tExtracted cells: " << m_toPolyData->GetOutput()->GetNumberOfCells() << std::endl;

      // Set ultimate input
      this->SetInputConnection( m_toPolyData->GetOutputPort() );
    }
    else
      this->SetInputConnection( source->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Sets cells selected for visualization.
//! \param mask [in] selected cells.
void asiVisu_FaceDomainPipeline::SetSelectedCells(const TColStd_PackedMapOfInteger& mask)
{
  m_selected->Reset();
  m_selected->SetNumberOfComponents(1);

  // Populate VTK array with cell IDs to keep in visualization
  for ( TColStd_MapIteratorOfPackedMapOfInteger it(mask); it.More(); it.Next() )
  {
    const int id = it.Key();
    m_selected->InsertNextValue(id);
  }

  // Initialize selection node
  m_selectionNode->SetSelectionList(m_selected);
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void asiVisu_FaceDomainPipeline::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
void asiVisu_FaceDomainPipeline::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
void asiVisu_FaceDomainPipeline::callback_update()
{
  if ( m_bDefaultColorScheme && !m_bMapperColorsSet )
  {
    vtkSmartPointer<vtkLookupTable> aLookup = asiVisu_Utils::InitDomainLookupTable();
    asiVisu_Utils::InitMapper(m_mapper, aLookup, ARRNAME_ORIENT_SCALARS);
    m_bMapperColorsSet = true;
  }
}
