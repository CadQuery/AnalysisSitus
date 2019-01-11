//-----------------------------------------------------------------------------
// Created on: 11 November 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiAlgo_MeshInterPlane.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>
#include <asiAlgo_MeshConvert.h>

// VTK includes
#include <vtkCutter.h>
#include <vtkPlane.h>

//-----------------------------------------------------------------------------

asiAlgo_MeshInterPlane::asiAlgo_MeshInterPlane(const Handle(Poly_Triangulation)& mesh,
                                               ActAPI_ProgressEntry              progress,
                                               ActAPI_PlotterEntry               plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_mesh = mesh;
}

//-----------------------------------------------------------------------------

bool asiAlgo_MeshInterPlane::Perform(const Handle(Geom_Plane)& plane)
{
  // Convert triangulation to VTK form.
  vtkSmartPointer<vtkPolyData> meshVtk;
  if ( !asiAlgo_MeshConvert::ToVTK(m_mesh, meshVtk) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot convert mesh to VTK data structure.");
    return false;
  }

  double minBound[3];
  minBound[0] = meshVtk->GetBounds()[0];
  minBound[1] = meshVtk->GetBounds()[2];
  minBound[2] = meshVtk->GetBounds()[4];

  double maxBound[3];
  maxBound[0] = meshVtk->GetBounds()[1];
  maxBound[1] = meshVtk->GetBounds()[3];
  maxBound[2] = meshVtk->GetBounds()[5];

  double center[3];
  center[0] = meshVtk->GetCenter()[0];
  center[1] = meshVtk->GetCenter()[1];
  center[2] = meshVtk->GetCenter()[2];

  const double distanceMin = sqrt( vtkMath::Distance2BetweenPoints(minBound, center) );
  const double distanceMax = sqrt( vtkMath::Distance2BetweenPoints(maxBound, center) );

  const gp_Ax3& origin = plane->Position();

  // Prepare plane to cut.
  vtkSmartPointer<vtkPlane> planeVtk = vtkSmartPointer<vtkPlane>::New();
  //
  planeVtk->SetOrigin( origin.Location().X(),
                       origin.Location().Y(),
                       origin.Location().Z() );
  planeVtk->SetNormal( origin.Direction().X(),
                       origin.Direction().Y(),
                       origin.Direction().Z() );

  // Create cutter.
  vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
  cutter->SetCutFunction(planeVtk);
  cutter->SetInputData(meshVtk);
  cutter->Update();

  // Get output.
  vtkPolyData* outputVtk = cutter->GetOutput();

  // Get number of cells.
  const int numCells = int( outputVtk->GetNumberOfCells() );
  //
  if ( numCells == 0 )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "No cells in the output.");
    return false;
  }
  //
  m_progress.SendLogMessage(LogInfo(Normal) << "Generated output with %1 cells." << numCells);

  // Get output points.
  Handle(asiAlgo_BaseCloud<double>) cloud = new asiAlgo_BaseCloud<double>;
  //
  for ( vtkIdType cellId = 0; cellId < numCells; ++cellId )
  {
    vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();

    // Get the list of points for this cell.
    outputVtk->GetCellPoints(cellId, ptIds);
    //
    for ( vtkIdType k = 0; k < ptIds->GetNumberOfIds(); ++k )
    {
      const vtkIdType pid = ptIds->GetId(k);

      double coords[3];
      outputVtk->GetPoint(pid, coords);

      cloud->AddElement(coords[0], coords[1], coords[2]);
    }
  }

  m_plotter.DRAW_POINTS(cloud->GetCoordsArray(), Color_Red, "pnts");
  return true;
}
