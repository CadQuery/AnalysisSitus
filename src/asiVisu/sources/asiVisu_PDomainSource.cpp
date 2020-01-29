//-----------------------------------------------------------------------------
// Created on: 30 March 2016
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
#include <asiVisu_PDomainSource.h>

// asiVisu includes
#include <asiVisu_PCurveSource.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>

// OCCT includes
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_PDomainSource)

//! Default constructor.
asiVisu_PDomainSource::asiVisu_PDomainSource()
: vtkPolyDataAlgorithm()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
  this->SetPCurveModeOn();
}

//! Destructor.
asiVisu_PDomainSource::~asiVisu_PDomainSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Sets input geometry.
//! \param face [in] face to render the parametric portrait for.
void asiVisu_PDomainSource::SetFace(const TopoDS_Face& face)
{
  m_face = face;
  this->Modified();
}

//! Enables p-curve visualization mode.
void asiVisu_PDomainSource::SetPCurveModeOn()
{
  m_bPCurveMode = true;
  this->Modified();
}

//! Enables 3D curve visualization mode.
void asiVisu_PDomainSource::Set3DCurveModeOn()
{
  m_bPCurveMode = false;
  this->Modified();
}

//! Sets optional normal vector for plane where tip of the curves
//! composing a parametric domain will be defined.
//! \param[in] tipNorm normal vector to set.
void asiVisu_PDomainSource::SetTipNorm(const gp_Vec& tipNorm)
{
  m_tipNorm = tipNorm;
  this->Modified();
}

//-----------------------------------------------------------------------------

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param request      [in]  describes "what" algorithm should do. This is
//!                           typically just one key such as REQUEST_INFORMATION.
//! \param inputVector  [in]  inputs of the algorithm.
//! \param outputVector [out] outputs of the algorithm.
int asiVisu_PDomainSource::RequestData(vtkInformation*        request,
                                       vtkInformationVector** inputVector,
                                       vtkInformationVector*  outputVector)
{
  if ( m_face.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: nullptr face" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  // Compute tip size for the orientation markers
  const double tipSize = this->computeTipSize();

  // Append filter
  vtkSmartPointer<vtkAppendPolyData>
    appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

  // Explode by edges. In "p-curves mode", orientation of face in question
  // is set to FORWARD in order to prevent explorer from multiplying it to
  // the orientations of sub-shapes. That's important because we see parametric
  // domain from direction of the natural surface normal (this is by
  // definition of "parametric domain"). However, when we look at the oriented
  // contour in 3D, we have to take into account the face orientation flag
  // in order to respect the "material-on-the-left" rule.
  TopoDS_Face face2Iterate;
  //
  if ( m_bPCurveMode )
    face2Iterate = TopoDS::Face( m_face.Oriented(TopAbs_FORWARD) );
  else
    face2Iterate = m_face;
  //
  int edgeId = 0;
  for ( TopExp_Explorer exp(face2Iterate, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    // Current edge
    ++edgeId;
    const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );

    // Prepare curve source
    vtkSmartPointer<asiVisu_CurveSource> curveSource;

    // Allocate source wrt the desired visualization mode
    if ( m_bPCurveMode )
    {
      curveSource = vtkSmartPointer<asiVisu_PCurveSource>::New();

      asiVisu_PCurveSource*
        pcurveSource = asiVisu_PCurveSource::SafeDownCast(curveSource);

      // Initialize data source
      pcurveSource->SetEdgeOnFace(E, m_face);
    }
    else
    {
      curveSource = vtkSmartPointer<asiVisu_CurveSource>::New();

      // Initialize data source
      curveSource->SetInputEdge(E);
    }

    // Common settings
    curveSource->SetTipSize(tipSize);
    curveSource->SetTipNorm(m_tipNorm);
    curveSource->SetPedigreeId(edgeId);

    // Append poly data
    appendFilter->AddInputConnection( curveSource->GetOutputPort() );
  }

  if ( !edgeId ) // Empty dataset
  {
    // Feed append filter with empty dataset just in order to have some
    // fictive input
    vtkSmartPointer<vtkPolyData> dummyData = vtkSmartPointer<vtkPolyData>::New();
    appendFilter->SetInputData(dummyData);
  }

  appendFilter->Update();
  //
  polyOutput->DeepCopy( appendFilter->GetOutput() );

  return Superclass::RequestData(request, inputVector, outputVector);
}

//-----------------------------------------------------------------------------

//! Computes size for orientation tip glyph. This size is decided for the
//! whole face to have it identical for all edges.
//! \return tip size.
double asiVisu_PDomainSource::computeTipSize() const
{
  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(m_face, uMin, uMax, vMin, vMax);

  // Take a ratio of a bounding diagonal
  return ( gp_XY(uMax, vMax) - gp_XY(uMin, vMin) ).Modulus() * 0.025;
}
