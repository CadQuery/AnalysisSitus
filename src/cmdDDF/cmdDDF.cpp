//-----------------------------------------------------------------------------
// Created on: 21 January 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

// cmdEngine includes
#include <cmdDDF.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiVisu includes
#include <asiVisu_OctreePipeline.h>
#include <asiVisu_OctreePrs.h>

// asiAlgo includes
#include <asiAlgo_ProjectPointOnMesh.h>
#ifdef USE_MOBIUS
  #include <asiAlgo_MeshDistanceFunc.h>
#endif
//
#include <asiAlgo_Timer.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

#ifdef USE_MOBIUS
  #include <mobius/cascade.h>
  #include <mobius/cascade_Triangulation.h>
  #include <mobius/poly_DistanceField.h>
  #include <mobius/poly_DistanceFunc.h>
  #include <mobius/poly_MarchingCubes.h>
  #include <mobius/poly_SVO.h>

  using namespace mobius;
#endif

// VTK includes
#pragma warning(push, 0)
#include <vtkXMLUnstructuredGridWriter.h>
#pragma warning(pop)

//-----------------------------------------------------------------------------

Handle(asiEngine_Model)        cmdDDF::model = nullptr;
Handle(asiUI_CommonFacilities) cmdDDF::cf    = nullptr;

//-----------------------------------------------------------------------------

void cmdDDF::ClearViewers(const bool repaint)
{
  if ( cf.IsNull() )
    return;

  // Get all presentation managers
  const vtkSmartPointer<asiVisu_PrsManager>& partPM   = cf->ViewerPart->PrsMgr();
  const vtkSmartPointer<asiVisu_PrsManager>& hostPM   = cf->ViewerHost->PrsMgr();
  const vtkSmartPointer<asiVisu_PrsManager>& domainPM = cf->ViewerDomain->PrsMgr();

  // Update viewers
  partPM  ->DeleteAllPresentations();
  hostPM  ->DeleteAllPresentations();
  domainPM->DeleteAllPresentations();

  if ( repaint )
  {
    cf->ViewerPart->Repaint();
    cf->ViewerHost->Repaint();
    cf->ViewerDomain->Repaint();
  }
}

//-----------------------------------------------------------------------------

int DDF_BuildSVO(const Handle(asiTcl_Interp)& interp,
                 int                          argc,
                 const char**                 argv)
{
#if defined USE_MOBIUS
  // Min cell size.
  double minSize = 1.0;
  interp->GetKeyValue(argc, argv, "min", minSize);

  // Precision.
  double prec = 1.0;
  interp->GetKeyValue(argc, argv, "prec", prec);

  // Is in cube.
  const bool isCube = interp->HasKeyword(argc, argv, "cube");

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Voxelization with min cell size %1 and precision %2."
                                                       << minSize << prec);

  // Get part shape.
  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // API for Part.
  asiEngine_Part partApi(M);

  // Get BVH from the Part Node.
  Handle(asiAlgo_BVHFacets) bvh = M->GetPartNode()->GetBVH();
  //
  if ( bvh.IsNull() )
  {
    // Construct BVH right here.
    M->OpenCommand();
    {
      bvh = partApi.BuildBVH();
    }
    M->CommitCommand();
  }

  /* =============================
   *  Construct distance function.
   * ============================= */

  TIMER_NEW
  TIMER_GO

  asiAlgo_MeshDistanceFunc*
    pDistFunc = new asiAlgo_MeshDistanceFunc(bvh, poly_DistanceFunc::Mode_Signed, isCube);

  pDistFunc->asiPlotter  = interp->GetPlotter();
  pDistFunc->asiProgress = interp->GetProgress();

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Construct implicit distance function")

  /* ==========================
   *  Construct distance field.
   * ========================== */

  TIMER_RESET
  TIMER_GO

  poly_DistanceField* pDDF = new poly_DistanceField();
  //
  if ( !pDDF->Build(minSize, prec, pDistFunc) )
  {
    delete pDDF;

    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Failed to build distance field.");
    return TCL_ERROR;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Build SVO for distance field")

  poly_SVO* pRoot = pDDF->GetRoot();

  // Measure SVO.
  int                      numSVONodes;
  const unsigned long long memBytes  = pRoot->GetMemoryInBytes(numSVONodes);
  const double             memMBytes = memBytes / (1024.*1024.);
  //
  interp->GetProgress().SendLogMessage( LogInfo(Normal) << "SVO contains %1 nodes and occupies %2 bytes (%3 MiB) of memory."
                                                        << numSVONodes << int(memBytes) << memMBytes );

  // Show voxels.
  M->OpenCommand();
  {
    partApi.SetOctree(pRoot);
  }
  M->CommitCommand();
  //
  cmdDDF::cf->ObjectBrowser->Populate();
  cmdDDF::cf->ViewerPart->PrsMgr()->Actualize( M->GetOctreeNode() );

  return TCL_OK;
#else
  cmdDDF_NotUsed(argc);
  cmdDDF_NotUsed(argv);

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "SVO is a part of Mobius (not available in open source).");

  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

int DDF_DumpVTU(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  Handle(asiData_OctreeNode) octreeNode = M->GetOctreeNode();

  // Get distance field from the Part Node.
  poly_SVO*
    pSVO = static_cast<poly_SVO*>( octreeNode->GetOctree() );
  //
  if ( !pSVO )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Distance field is not initialized.");
    return TCL_ERROR;
  }

  if ( !cmdDDF::cf->ViewerPart )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Part viewer is not available.");
    return TCL_ERROR;
  }

  // Get octree pipeline to access the data source.
  Handle(asiVisu_OctreePrs)
    partPrs = Handle(asiVisu_OctreePrs)::DownCast( cmdDDF::cf->ViewerPart->PrsMgr()->GetPresentation(octreeNode) );
  //
  Handle(asiVisu_OctreePipeline)
    octreePl = Handle(asiVisu_OctreePipeline)::DownCast( partPrs->GetPipeline(asiVisu_OctreePrs::Pipeline_Main) );
  //
  const vtkSmartPointer<asiVisu_OctreeSource>& octreeSource = octreePl->GetSource();

  // Update and dump to file.
  octreeSource->Update();
  //
  vtkSmartPointer<vtkXMLUnstructuredGridWriter>
    writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
  //
  writer->SetFileName( argv[1] );
  writer->SetInputConnection( octreeSource->GetOutputPort() );
  writer->Write();

  // Dump.
  return TCL_OK;
#else
  cmdDDF_NotUsed(argc);
  cmdDDF_NotUsed(argv);

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "SVO is a part of Mobius (not available in open source).");

  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

int DDF_SetSVO(const Handle(asiTcl_Interp)& interp,
               int                          argc,
               const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc < 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  Handle(asiData_OctreeNode) octreeNode = M->GetOctreeNode();

  // Get distance field from the Part Node.
  poly_SVO* pSVO = static_cast<poly_SVO*>( octreeNode->GetOctree() );
  //
  if ( !pSVO )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Distance field is not initialized.");
    return TCL_ERROR;
  }

  // Prepare path.
  std::vector<size_t> path;
  //
  for ( int k = 1; k < argc; ++k )
    path.push_back( (size_t) atoi(argv[k]) );

  // Access octree node.
  poly_SVO* pNode = pSVO->FindChild(path);
  //
  if ( !pNode )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot access the requested node.");
    return TCL_ERROR;
  }

  // Show voxels.
  M->OpenCommand();
  {
    // Update Part Node.
    asiEngine_Part(M).SetOctree(pNode);
  }
  M->CommitCommand();
  //
  cmdDDF::cf->ViewerPart->PrsMgr()->Actualize( M->GetOctreeNode() );

  return TCL_OK;
#else
  cmdDDF_NotUsed(argc);
  cmdDDF_NotUsed(argv);

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "SVO is a part of Mobius (not available in open source).");

  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

int DDF_EvalSVO(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  Handle(asiData_OctreeNode) octreeNode = M->GetOctreeNode();

  // Get octree from the Part Node.
  poly_SVO* pSVO = static_cast<poly_SVO*>( octreeNode->GetOctree() );
  //
  if ( !pSVO )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Distance field is not initialized.");
    return TCL_ERROR;
  }

  if ( !pSVO->HasScalars() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "The root SVO node is uninitialized.");
    return TCL_ERROR;
  }

  // Get coordinates of the point to evaluate.
  const double x = atof(argv[1]);
  const double y = atof(argv[2]);
  const double z = atof(argv[3]);
  //
  interp->GetPlotter().REDRAW_POINT("P", gp_Pnt(x, y, z), Color_Red);
  //
  t_xyz P(x, y, z);

  // Evaluate.
  const double f = pSVO->Eval(P);
  //
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "f(%1, %2, %3) = %4."
                                                       << x << y << z << f);

  return TCL_OK;
#else
  cmdDDF_NotUsed(argc);
  cmdDDF_NotUsed(argv);

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "SVO is a part of Mobius (not available in open source).");

  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

#if defined USE_MOBIUS
void getCentersOfVoxels(poly_SVO*                                pNode,
                        const bool                               isBoundarySampling,
                        const Handle(asiAlgo_BaseCloud<double>)& pts)
{
  if ( pNode == nullptr )
    return;

  if ( pNode->IsLeaf() )
  {
    if ( (  isBoundarySampling && poly_DistanceField::IsZeroCrossing(pNode) ) ||
         ( !isBoundarySampling && poly_DistanceField::IsIn(pNode) ) )
    {
      gp_XYZ
        center = 0.125 * ( cascade::GetOpenCascadeXYZ( pNode->GetP0() )
                         + cascade::GetOpenCascadeXYZ( pNode->GetP1() )
                         + cascade::GetOpenCascadeXYZ( pNode->GetP2() )
                         + cascade::GetOpenCascadeXYZ( pNode->GetP3() )
                         + cascade::GetOpenCascadeXYZ( pNode->GetP4() )
                         + cascade::GetOpenCascadeXYZ( pNode->GetP5() )
                         + cascade::GetOpenCascadeXYZ( pNode->GetP6() )
                         + cascade::GetOpenCascadeXYZ( pNode->GetP7() ) );

      pts->AddElement(center);
    }
  }
  else
    for ( size_t k = 0; k < 8; ++k )
    {
      getCentersOfVoxels(pNode->GetChild(k), isBoundarySampling, pts);
    }
}
#endif

int DDF_SampleSVOPoints(const Handle(asiTcl_Interp)& interp,
                        int                          argc,
                        const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc != 2 && argc != 3 && argc != 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  const bool isBoundarySampling = interp->HasKeyword(argc, argv, "boundary");
  const bool isBoundaryProj     = interp->HasKeyword(argc, argv, "project");
  //
  if ( isBoundaryProj && !isBoundarySampling )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Projection is only allowed in '-boundary' mode.");
    return TCL_ERROR;
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  Handle(asiData_OctreeNode) octreeNode = M->GetOctreeNode();

  // Get distance field from the Part Node.
  poly_SVO* pOctree = static_cast<poly_SVO*>( octreeNode->GetOctree() );
  //
  if ( !pOctree )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Distance field is not initialized.");
    return TCL_ERROR;
  }

  // Get center points in a point cloud.
  Handle(asiAlgo_BaseCloud<double>) points = new asiAlgo_BaseCloud<double>;
  //
  getCentersOfVoxels(pOctree, isBoundarySampling, points);

  // Project points.
  if ( isBoundaryProj )
  {
    // Get BVH from the Part Node.
    Handle(asiAlgo_BVHFacets) bvh = M->GetPartNode()->GetBVH();

    TIMER_NEW
    TIMER_GO

    // Prepare the projection tool.
    asiAlgo_ProjectPointOnMesh projection( bvh,
                                           interp->GetProgress(),
                                           interp->GetPlotter() );

    // Project each point separately.
    Handle(asiAlgo_BaseCloud<double>) projected = new asiAlgo_BaseCloud<double>;
    //
    for ( int pp = 0; pp < points->GetNumberOfElements(); ++pp )
    {
      gp_Pnt P_center = points->GetElement(pp);
      gp_Pnt P_proj   = projection.Perform(P_center);
      //
      projected->AddElement(P_proj);
    }

    TIMER_FINISH
    TIMER_COUT_RESULT_NOTIFIER(interp->GetProgress(), "Project points on mesh")

    points = projected;
  }

  // Draw.
  interp->GetPlotter().REDRAW_POINTS(argv[1], points->GetCoordsArray(),
                                     isBoundarySampling ? Color_Green : Color_White);

  return TCL_OK;
#else
  cmdDDF_NotUsed(argc);
  cmdDDF_NotUsed(argv);

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "SVO is a part of Mobius (not available in open source).");

  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

int DDF_Polygonize(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc < 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get level of the function to polygonize.
  double level = 0.;
  interp->GetKeyValue(argc, argv, "level", level);

  // Get number of slices for a regular grid.
  int numSlices = 128;
  interp->GetKeyValue(argc, argv, "slices", numSlices);

  // Get Octree Node.
  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );
  //
  Handle(asiData_OctreeNode) octreeNode = M->GetOctreeNode();

  // Get distance field from the Part Node.
  poly_SVO* pOctree = static_cast<poly_SVO*>( octreeNode->GetOctree() );
  //
  if ( !pOctree )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Distance field is not initialized.");
    return TCL_ERROR;
  }

  // Construct distance field to serve as an implicit function for the
  // reconstruction algorithm.
  t_ptr<poly_DistanceField> df = new poly_DistanceField(pOctree);

  // Run marching cubes reconstruction.
  poly_MarchingCubes mcAlgo(df, numSlices);
  //
  if ( !mcAlgo.Perform(level) )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Marching cubes reconstruction failed.");
    return TCL_ERROR;
  }

  // Get the reconstruction result.
  const t_ptr<poly_Mesh>& mesh = mcAlgo.GetResult();

  // Convert to OpenCascade's mesh.
  cascade_Triangulation converter(mesh);
  converter.DirectConvert();
  //
  const Handle(Poly_Triangulation)&
    triangulation = converter.GetOpenCascadeTriangulation();

  // Draw.
  interp->GetPlotter().REDRAW_TRIANGULATION(argv[1], triangulation, Color_Default, 1.);
  return TCL_OK;
#else
  cmdDDF_NotUsed(argc);
  cmdDDF_NotUsed(argv);

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "SVO is a part of Mobius (not available in open source).");

  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

int DDF_PolygonizeCell(const Handle(asiTcl_Interp)& interp,
                       int                          argc,
                       const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc < 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  Handle(asiData_OctreeNode) octreeNode = M->GetOctreeNode();

  // Get distance field from the Part Node.
  poly_SVO* pSVO = static_cast<poly_SVO*>( octreeNode->GetOctree() );
  //
  if ( !pSVO )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Distance field is not initialized.");
    return TCL_ERROR;
  }

  // Prepare path.
  std::vector<size_t> path;
  //
  for ( int k = 2; k < argc; ++k )
    path.push_back( (size_t) atoi(argv[k]) );

  // Access octree node.
  poly_SVO* pNode = pSVO->FindChild(path);
  //
  if ( !pNode )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot access the requested node.");
    return TCL_ERROR;
  }

  // Show voxels.
  M->OpenCommand();
  {
    // Update Part Node.
    asiEngine_Part(M).SetOctree(pNode);
  }
  M->CommitCommand();
  //
  cmdDDF::cf->ViewerPart->PrsMgr()->Actualize( M->GetOctreeNode() );

  // Construct distance field to serve as an implicit function for the
  // reconstruction algorithm.
  t_ptr<poly_DistanceField> df = new poly_DistanceField(pNode);

  // Run marching cubes reconstruction at a single voxel.
  t_ptr<poly_Mesh>
    mesh = poly_MarchingCubes::PolygonizeVoxel( pNode->GetP0(), pNode->GetP7(),
                                                df, 0. );

  if ( !mesh->GetNumTriangles() )
  {
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Empty polygon.");
  }
  else
  {
    // Convert to OpenCascade's mesh.
    cascade_Triangulation converter(mesh);
    converter.DirectConvert();
    //
    const Handle(Poly_Triangulation)&
      triangulation = converter.GetOpenCascadeTriangulation();

    // Draw.
    interp->GetPlotter().REDRAW_TRIANGULATION(argv[1], triangulation, Color_Default, 1.);
  }

  return TCL_OK;
#else
  cmdDDF_NotUsed(argc);
  cmdDDF_NotUsed(argv);

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "SVO is a part of Mobius (not available in open source).");

  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

#if defined USE_MOBIUS
void getLeaves(poly_SVO*               pNode,
               std::vector<poly_SVO*>& leaves)
{
  if ( pNode == nullptr )
    return;

  if ( pNode->IsLeaf() )
  {
    leaves.push_back(pNode);
  }
  else
    for ( size_t k = 0; k < 8; ++k )
    {
      getLeaves(pNode->GetChild(k), leaves);
    }
}
#endif

//-----------------------------------------------------------------------------

int DDF_PolygonizeSVO(const Handle(asiTcl_Interp)& interp,
                      int                          argc,
                      const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc != 2 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  Handle(asiData_OctreeNode) octreeNode = M->GetOctreeNode();

  // Get distance field from the Part Node.
  poly_SVO* pSVO = static_cast<poly_SVO*>( octreeNode->GetOctree() );
  //
  if ( !pSVO )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Distance field is not initialized.");
    return TCL_ERROR;
  }

  // Construct distance field to serve as an implicit function for the
  // reconstruction algorithm.
  t_ptr<poly_DistanceField> df = new poly_DistanceField(pSVO);

  // Gather leaves.
  std::vector<poly_SVO*> leaves;
  //
  getLeaves(pSVO, leaves);

  // Run marching cubes in each leaf.
  for ( size_t k = 0; k < leaves.size(); ++k )
  {
    // Run marching cubes reconstruction at a single voxel.
    t_ptr<poly_Mesh>
      mesh = poly_MarchingCubes::PolygonizeVoxel( leaves[k]->GetP0(), leaves[k]->GetP7(),
                                                  df, 0. );

    if ( !mesh->GetNumTriangles() )
    {
      interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Empty polygon.");
    }
    else
    {
      // Convert to OpenCascade's mesh.
      cascade_Triangulation converter(mesh);
      converter.DirectConvert();
      //
      const Handle(Poly_Triangulation)&
        triangulation = converter.GetOpenCascadeTriangulation();

      // Draw.
      interp->GetPlotter().DRAW_TRIANGULATION(triangulation, Color_Default, 1., argv[1]);
    }
  }

  return TCL_OK;
#else
  cmdDDF_NotUsed(argc);
  cmdDDF_NotUsed(argv);

  interp->GetProgress().SendLogMessage(LogErr(Normal) << "SVO is a part of Mobius (not available in open source).");

  return TCL_ERROR;
#endif
}

//-----------------------------------------------------------------------------

void cmdDDF::Factory(const Handle(asiTcl_Interp)&      interp,
                     const Handle(Standard_Transient)& data)
{
  static const char* group = "cmdDDF";

  /* ==========================
   *  Initialize UI facilities.
   * ========================== */

  // Get common facilities
  Handle(asiUI_CommonFacilities)
    passedCF = Handle(asiUI_CommonFacilities)::DownCast(data);
  //
  if ( passedCF.IsNull() )
    interp->GetProgress().SendLogMessage(LogWarn(Normal) << "[cmdDDF] UI facilities are not available. GUI may not be updated.");
  else
    cf = passedCF;

  /* ================================
   *  Initialize Data Model instance.
   * ================================ */

  model = Handle(asiEngine_Model)::DownCast( interp->GetModel() );
  //
  if ( model.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "[cmdDDF] Data Model instance is null or not of asiEngine_Model kind.");
    return;
  }

  /* ==================
   *  Add Tcl commands.
   * ================== */

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-build-svo",
    //
    "ddf-build-svo [-min <minSize>] [-prec <precision>] [-cube]\n"
    "\t Builds SVO for the active part. The argument <minSize> controls\n"
    "\t the voxelization resolution. The argument <precision> controls\n"
    "\t the accuracy of distance field approximation.",
    //
    __FILE__, group, DDF_BuildSVO);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-dump-vtu",
    //
    "ddf-dump-vtu <filename>\n"
    "\t Dumps the octree cells of the active Part to a file in the VTU format"
    "\t of VTK.",
    //
    __FILE__, group, DDF_DumpVTU);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-set-svo",
    //
    "ddf-set-svo <id0> [<id1> [<id2> [...]]]\n"
    "\t Shrinks the stored octree to the SVO cell with the specified address."
    "\t The found cell will be set as a new active SVO. To get back to the previous\n"
    "\t state, the modification should be reverted in the data model by 'undo' command.",
    //
    __FILE__, group, DDF_SetSVO);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-eval-svo",
    //
    "ddf-eval-svo <x> <y> <z>\n"
    "\t Evaluates the active SVO node (i.e., its corresponding implicit function)\n"
    "\t in the given point with <x>, <y>, <z> coordinates.",
    //
    __FILE__, group, DDF_EvalSVO);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-sample-svo-points",
    //
    "ddf-sample-svo-points <resName> [-boundary [-project]]\n"
    "\t Samples points in the cells of the active SVO. The result is set\n"
    "\t as a point cloud with name <resName>. If the '-boundary' key is\n"
    "\t passed, only the zero-crossing voxels will be sampled. If the\n"
    "\t '-boundary' key is followed with the '-project' key, the boundary\n"
    "\t points will be precised by projection to the initial facets.",
    //
    __FILE__, group, DDF_SampleSVOPoints);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-polygonize",
    //
    "ddf-polygonize <resName> [-level <isoLevel>] [-slices <numSlices>]\n"
    "\t Polygonizes the active distance field at <isoLevel> function\n"
    "\t level set.",
    //
    __FILE__, group, DDF_Polygonize);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-polygonize-cell",
    //
    "ddf-polygonize-cell <resName> <id0> [<id1> [<id2> [...]]]\n"
    "\t Polygonizes a single SVO cell.",
    //
    __FILE__, group, DDF_PolygonizeCell);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-polygonize-svo",
    //
    "ddf-polygonize-svo <resName>\n"
    "\t Polygonizes the active SVO.",
    //
    __FILE__, group, DDF_PolygonizeSVO);
}

// Declare entry point PLUGINFACTORY
ASIPLUGIN(cmdDDF)
