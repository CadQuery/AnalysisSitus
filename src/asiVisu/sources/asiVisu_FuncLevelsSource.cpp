//-----------------------------------------------------------------------------
// Created on: 28 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_FuncLevelsSource.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

//! Default constructor.
template <typename T_VARIABLE>
asiVisu_FuncLevelsSource<T_VARIABLE>::asiVisu_FuncLevelsSource() : vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//-----------------------------------------------------------------------------

//! Destructor.
template <typename T_VARIABLE>
asiVisu_FuncLevelsSource<T_VARIABLE>::~asiVisu_FuncLevelsSource()
{}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Sets input function to visualize. This method also accepts the function
//! domain bounds which are used to limit the sampled are.
//! \param func         [in] functions to visualize.
//! \param minCorner    [in] min corner of domain hypercube.
//! \param maxCorner    [in] max corner of domain hypercube.
//! \param numIntervals [in] number of sampling intervals.
template <typename T_VARIABLE>
void asiVisu_FuncLevelsSource<T_VARIABLE>::SetInputFunction(const Handle(asiAlgo_Function<T_VARIABLE>)& func,
                                                            const T_VARIABLE&                           minCorner,
                                                            const T_VARIABLE&                           maxCorner,
                                                            const int                                   numIntervals)
{
  m_func          =  points;
  m_minCorner     =  minCorner;
  m_maxCorner     =  maxCorner;
  m_iNumIntervals =  numIntervals;
  m_fFuncMin      =  RealLast();
  m_fFuncMax      = -RealLast();
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

//! This is called by the superclass. Creates VTK polygonal data set
//! from the input arrays.
//! \param request      [in] information about data object.
//! \param inputVector  [in] the input data. As a data source is the start
//!                          stage of the VTK pipeline, the Input Vector is
//!                          empty and not used (no input port).
//! \param outputVector [in] the pointer to output data, that is filled
//!                          in this method.
//! \return status.
template <typename T_VARIABLE>
int asiVisu_FuncLevelsSource<T_VARIABLE>::RequestData(vtkInformation*        request,
                                                      vtkInformationVector** inputVector,
                                                      vtkInformationVector*  outputVector)
{
  if ( m_func.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL function" );
    return 0;
  }

  if ( T_VARIABLE::num_coordinates() != 2 )
  {
    vtkErrorMacro( << "Invalid domain: only two-dimensional domains are supported" );
    return 0;
  }

  if ( m_iNumIntervals < 1 )
  {
    vtkErrorMacro( << "Invalid number of sampling intervals" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  /* ===============
   *  Sample domain
   * =============== */

  const int numRanges_x0 = m_iNumIntervals;
  const int numRanges_x1 = m_iNumIntervals;
  //
  const double x0_step = ( m_maxCorner.Coord(0) - m_minCorner.Coord(0) ) / numRanges_x0;
  const double x1_step = ( m_maxCorner.Coord(1) - m_minCorner.Coord(1) ) / numRanges_x1;

  // Function values
  std::vector<double> funcValues;

  // Sample
  for ( int i = 0; i <= numRanges_x0; ++i )
  {
    double x0 = m_minCorner.Coord(0) + i*numRanges_x0;
    //
    if ( x0 > m_maxCorner.Coord(0) )
      x0 = m_maxCorner.Coord(0);

    for ( int j = 0; j <= numRanges_x1; ++j )
    {
      double x1 = m_minCorner.Coord(1) + j*numRanges_x1;
      //
      if ( x1 > m_maxCorner.Coord(1) )
        x1 = m_maxCorner.Coord(1);

      // Register point and cell
      gp_Pnt P( x0, x1, 0.0 );
      vtkIdType pointIndex = this->registerGridPoint(P, polyOutput);
      this->registerVertex( pointIndex, polyOutput );

      // Evaluate function
      const double f = m_func->Value( T_VARIABLE(x0, x1) );
      //
      if ( f < m_fFuncMin ) m_fFuncMin = f;
      if ( f > m_fFuncMax ) m_fFuncMax = f;
      //
      funcValues.push_back(f);
    }
  }

  /* ====================================================================
   *  Associate colors to the domain points according to function values
   * ==================================================================== */

  // Create the color map
  vtkSmartPointer<vtkLookupTable>
    colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
  //
  colorLookupTable->SetTableRange(m_fFuncMin, m_fFuncMax);
  colorLookupTable->Build();

  // Generate the colors for each point based on the color map
  vtkSmartPointer<vtkUnsignedCharArray>
    colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
  //
  colors->SetNumberOfComponents(3);
  colors->SetName("Colors");

  // Set color for each point
  for ( int i = 0; i < polyOutput->GetNumberOfPoints(); ++i )
  {
    double p[3];
    polyOutput->GetPoint(i, p);
 
    double dcolor[3];
    colorLookupTable->GetColor(funcValues[i], dcolor);

    unsigned char color[3];
    for ( unsigned int j = 0; j < 3; ++j )
    {
      color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
    }

#if defined COUT_DEBUG
    std::cout << "color: "
              << (int) color[0] << " "
              << (int) color[1] << " "
              << (int) color[2] << std::endl;
#endif
 
    colors->InsertNextTupleValue(color);
  }

  // Set scalars
  polyOutput->GetPointData()->SetScalars(colors);

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

//! Adds the given point to the passed polygonal data set.
//! \param point    [in]     point to add.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
template <typename T_VARIABLE>
vtkIdType asiVisu_FuncLevelsSource<T_VARIABLE>::registerGridPoint(const gp_Pnt& point,
                                                                  vtkPolyData*  polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType pid = points->InsertNextPoint( point.X(),
                                           point.Y(),
                                           point.Z() );

  return pid;
}

//-----------------------------------------------------------------------------

//! Adds a vertex cell into the polygonal data set.
//! \param n        [in]     index of the corresponding geometric point.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
template <typename T_VARIABLE>
vtkIdType asiVisu_FuncLevelsSource<T_VARIABLE>::registerVertex(const vtkIdType n,
                                                               vtkPolyData*    polyData)
{
  std::vector<vtkIdType> nodes;
  nodes.push_back(n);
  //
  vtkIdType cellID = polyData->InsertNextCell(VTK_VERTEX, 1, &nodes[0]);
  //
  return cellID;
}
