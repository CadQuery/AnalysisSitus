//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_FuncUnivariateSource.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkObjectFactory.h>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_FuncUnivariateSource);

//! Default constructor.
asiVisu_FuncUnivariateSource::asiVisu_FuncUnivariateSource() : asiVisu_CurveSource()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_FuncUnivariateSource::~asiVisu_FuncUnivariateSource()
{}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Sets input data.
//! \param func [in] function to convert.
void asiVisu_FuncUnivariateSource::SetFunc(const Handle(asiAlgo_FuncUnivariate)& func)
{
  if ( func.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: function is NULL" );
    return;
  }

  m_func = func;

  /* ============================================
   *  Prepare Cartesian arrays for visualization
   * ============================================ */

  const int    nSteps  = 1000;
  const double x_min   = m_func->Min_X();
  const double x_max   = m_func->Max_X();
  const double x_delta = (x_max - x_min) / nSteps;

  // Allocate arrays
  Handle(HRealArray) xCoords = new HRealArray(0, nSteps, 0.0),
                     yCoords = new HRealArray(0, nSteps, 0.0),
                     zCoords = new HRealArray(0, nSteps, 0.0);

  for ( int i = 0; i <= nSteps; ++i )
  {
    double x = x_min + i*x_delta;
    if ( x > x_max )
      x = x_max;

    // Evaluate
    const double f = (*m_func)(x);
    //
    xCoords->ChangeValue(i) = x;
    yCoords->ChangeValue(i) = f;
    zCoords->ChangeValue(i) = 0.0;
  }

  /* ===================
   *  Initialize source
   * =================== */

  // Perform basic initialization
  asiVisu_CurveSource::SetInputArrays(xCoords, yCoords, zCoords);
}
