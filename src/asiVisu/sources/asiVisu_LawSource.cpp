//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_LawSource.h>

// A-Situs (visualization) includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkObjectFactory.h>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_LawSource);

//! Default constructor.
asiVisu_LawSource::asiVisu_LawSource() : asiVisu_CurveSource()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_LawSource::~asiVisu_LawSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Sets input data.
//! \param law [in] law function to convert.
void asiVisu_LawSource::SetLaw(const Handle(asiAlgo_DesignLaw)& law)
{
  if ( law.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: law is NULL" );
    return;
  }

  m_law = law;

  /* ============================================
   *  Prepare Cartesian arrays for visualization
   * ============================================ */

  const int    nSteps  = 1000;
  const double x_min   = m_law->Min_X();
  const double x_max   = m_law->Max_X();
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
    const double f = (*m_law)(x);
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
