//-----------------------------------------------------------------------------
// Created on: 09 February 2016
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
