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
#include <asiEngine_IV.h>
#include <asiEngine_Octree.h>
#include <asiEngine_Part.h>
#include <asiEngine_Triangulation.h>

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

  // Max cell size.
  double maxSize = minSize*100.;
  interp->GetKeyValue(argc, argv, "max", maxSize);

  // Get Data Model instance.
  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Get owner.
  Handle(ActAPI_INode) ownerNode;
  //
  ActAPI_DataObjectId ownerId;
  interp->GetKeyValue(argc, argv, "owner", ownerId);
  //
  if ( ownerId.IsEmpty() )
    ownerNode = M->GetPartNode();
  else
    ownerNode = M->FindNode(ownerId);
  //
  if ( ownerNode.IsNull() || !ownerNode->IsWellFormed() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Owner node is null or inconsistent.");
    return TCL_ERROR;
  }

  // Check the owner type (it should have BVH facets).
  Handle(asiAlgo_BVHFacets) bvh;
  int                       bvhPid;
  //
  if ( ownerNode->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
  {
    bvh    = Handle(asiData_PartNode)::DownCast(ownerNode)->GetBVH();
    bvhPid = asiData_PartNode::PID_BVH;

    // Construct BVH right here.
    M->OpenCommand();
    {
      bvh = asiEngine_Part(M).BuildBVH();
    }
    M->CommitCommand();
  }
  else if ( ownerNode->IsKind( STANDARD_TYPE(asiData_TriangulationNode) ) )
  {
    bvh    = Handle(asiData_TriangulationNode)::DownCast(ownerNode)->GetBVH();
    bvhPid = asiData_TriangulationNode::PID_BVH;

    // Construct BVH right here.
    M->OpenCommand();
    {
      bvh = asiEngine_Triangulation(M).BuildBVH();
    }
    M->CommitCommand();
  }
  else if ( ownerNode->IsKind( STANDARD_TYPE(asiData_IVTopoItemNode) ) )
  {
    Handle(asiData_IVTopoItemNode)
      topoItemNode = Handle(asiData_IVTopoItemNode)::DownCast(ownerNode);
    //
    bvh    = topoItemNode->GetBVH();
    bvhPid = asiData_IVTopoItemNode::PID_BVH;

    // Construct BVH right here.
    M->OpenCommand();
    {
      bvh = asiEngine_IV(M).BuildBVH(topoItemNode);
    }
    M->CommitCommand();
  }
  else
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Unexpected type of owner.");
    return TCL_ERROR;
  }

  // Precision.
  double prec = 1.0;
  interp->GetKeyValue(argc, argv, "prec", prec);

  // Is in cube.
  const bool isCube = interp->HasKeyword(argc, argv, "cube");

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Voxelization with min cell size %1 and precision %2."
                                                       << minSize << prec);
  // Show voxels.
  Handle(asiData_OctreeNode) octreeNode;
  //
  M->OpenCommand();
  {
    octreeNode = asiEngine_Octree(M).CreateOctree(ownerNode, bvhPid);

    const BVH_Vec3d& cornerMin = bvh->Box().CornerMin();
    const BVH_Vec3d& cornerMax = bvh->Box().CornerMax();

    // Initialize.
    octreeNode->SetMinCellSize     ( minSize );
    octreeNode->SetMaxCellSize     ( maxSize );
    octreeNode->SetPrecision       ( prec );
    octreeNode->SetDomainCube      ( isCube );
    octreeNode->SetDomainMinCorner ( cornerMin.x(), cornerMin.y(), cornerMin.z() );
    octreeNode->SetDomainMaxCorner ( cornerMax.x(), cornerMax.y(), cornerMax.z() );

    // Execute tree functions.
    M->FuncExecuteAll();
  }
  M->CommitCommand();

  // Update UI.
  cmdDDF::cf->ObjectBrowser->Populate();
  cmdDDF::cf->ViewerPart->PrsMgr()->Actualize(octreeNode);

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
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Find octree.
  Handle(asiData_OctreeNode)
    octreeNode = Handle(asiData_OctreeNode)::DownCast( M->FindNode(argv[1]) );
  //
  if ( octreeNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find octree with the id %1."
                                                        << argv[1]);
    return TCL_ERROR;
  }

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
  writer->SetFileName( argv[2] );
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
  if ( argc < 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Find octree.
  Handle(asiData_OctreeNode)
    octreeNode = Handle(asiData_OctreeNode)::DownCast( M->FindNode(argv[1]) );
  //
  if ( octreeNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find octree with the id %1."
                                                        << argv[1]);
    return TCL_ERROR;
  }

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
    octreeNode->SetOctree(pNode);
  }
  M->CommitCommand();
  //
  cmdDDF::cf->ViewerPart->PrsMgr()->Actualize(octreeNode);

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
  if ( argc != 5 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Find octree.
  Handle(asiData_OctreeNode)
    octreeNode = Handle(asiData_OctreeNode)::DownCast( M->FindNode(argv[1]) );
  //
  if ( octreeNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find octree with the id %1."
                                                        << argv[1]);
    return TCL_ERROR;
  }

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
  const double x = atof(argv[2]);
  const double y = atof(argv[3]);
  const double z = atof(argv[4]);
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

int DDF_Polygonize(const Handle(asiTcl_Interp)& interp,
                   int                          argc,
                   const char**                 argv)
{
#if defined USE_MOBIUS
  if ( argc < 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  // Get level of the function to polygonize.
  double level = 0.;
  interp->GetKeyValue(argc, argv, "level", level);

  // Get number of slices for a regular grid.
  int numSlices = 128;
  interp->GetKeyValue(argc, argv, "slices", numSlices);

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Find octree.
  Handle(asiData_OctreeNode)
    octreeNode = Handle(asiData_OctreeNode)::DownCast( M->FindNode(argv[2]) );
  //
  if ( octreeNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find octree with the id %1."
                                                        << argv[2]);
    return TCL_ERROR;
  }

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
  if ( argc < 4 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Find octree.
  Handle(asiData_OctreeNode)
    octreeNode = Handle(asiData_OctreeNode)::DownCast( M->FindNode(argv[2]) );
  //
  if ( octreeNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find octree with the id %1."
                                                        << argv[2]);
    return TCL_ERROR;
  }

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
  for ( int k = 3; k < argc; ++k )
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
    octreeNode->SetOctree(pNode);
  }
  M->CommitCommand();
  //
  cmdDDF::cf->ViewerPart->PrsMgr()->Actualize(octreeNode);

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
  if ( argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  Handle(asiEngine_Model)
    M = Handle(asiEngine_Model)::DownCast( interp->GetModel() );

  // Find octree.
  Handle(asiData_OctreeNode)
    octreeNode = Handle(asiData_OctreeNode)::DownCast( M->FindNode(argv[2]) );
  //
  if ( octreeNode.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "Cannot find octree with the id %1."
                                                        << argv[2]);
    return TCL_ERROR;
  }

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

  t_ptr<poly_Mesh> resMesh = new poly_Mesh;

  // Run marching cubes in each leaf.
  for ( size_t k = 0; k < leaves.size(); ++k )
  {
    // Run marching cubes reconstruction at a single voxel.
    t_ptr<poly_Mesh>
      localMesh = poly_MarchingCubes::PolygonizeVoxel( leaves[k]->GetP0(), leaves[k]->GetP7(),
                                                       df, 0. );

    if ( !localMesh->GetNumTriangles() )
    {
      interp->GetProgress().SendLogMessage(LogWarn(Normal) << "Empty local mesh.");
    }
    else
    {
      for ( poly_Mesh::TriangleIterator tit(localMesh); tit.More(); tit.Next() )
      {
        poly_TriangleHandle ht = tit.Current();
        poly_VertexHandle   hv[3], res_hv[3];

        // Get vertices.
        poly_Triangle t;
        if ( !localMesh->GetTriangle(ht, t) )
          continue;
        //
        t.GetVertices(hv[0], hv[1], hv[2]);

        // Add new triangle to the result mesh.
        for ( int kk = 0; kk < 3; ++kk )
        {
          poly_Vertex v;
          localMesh->GetVertex(hv[kk], v);

          // Add vertex to the resulting mesh.
          res_hv[kk] = resMesh->AddVertex(v);
        }

        // Add triangle to the resulting mesh.
        resMesh->AddTriangle(res_hv[0], res_hv[1], res_hv[2]);
      }
    }
  }

  // Convert to OpenCascade's mesh.
  cascade_Triangulation converter(resMesh);
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
    "ddf-build-svo [-min <minSize>] [-max <maxSize>] [-prec <precision>] [-cube] [-owner <ownerId>]\n"
    "\t Builds SVO for the given owner Node. The argument <minSize> controls\n"
    "\t the voxelization resolution. The argument <maxSize> controls the\n"
    "\t voxelization resolution in the empty space (i.e., inside and outside)\n"
    "\t the shape. By default, the value of <maxSize> is 100 times the value of\n"
    "\t <minSize>. The argument <precision> controls the accuracy of the distance\n"
    "\t field approximation in each voxel. If the optional key '-t' is passed, the\n"
    "\t SVO will be constructed on the active triangulation.",
    //
    __FILE__, group, DDF_BuildSVO);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-dump-vtu",
    //
    "ddf-dump-vtu <octreeId> <filename>\n"
    "\t Dumps the octree with the id <octreeId> to a file in the VTU format\n"
    "\t of VTK.",
    //
    __FILE__, group, DDF_DumpVTU);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-set-svo",
    //
    "ddf-set-svo <octreeId> <id0> [<id1> [<id2> [...]]]\n"
    "\t Shrinks the octree with the id <octreeId> to the SVO cell with the specified address.\n"
    "\t The found cell will be set as a new active SVO. To get back to the previous\n"
    "\t state, the modification should be reverted in the data model by 'undo' command.",
    //
    __FILE__, group, DDF_SetSVO);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-eval-svo",
    //
    "ddf-eval-svo <octreeId> <x> <y> <z>\n"
    "\t Evaluates the active SVO node (i.e., its corresponding implicit function)\n"
    "\t in the given point with <x>, <y>, <z> coordinates.",
    //
    __FILE__, group, DDF_EvalSVO);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-polygonize",
    //
    "ddf-polygonize <resName> <octreeId> [-level <isoLevel>] [-slices <numSlices>]\n"
    "\t Polygonizes the distance field stored in the octree <octreeId> at\n"
    "\t <isoLevel> function level set. The keyword '-slices' allows to control\n"
    "\t the number of cells in the uniform grid used internally in the marching\n"
    "\t cubes algorithm.",
    //
    __FILE__, group, DDF_Polygonize);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-polygonize-cell",
    //
    "ddf-polygonize-cell <resName> <octreeId> <id0> [<id1> [<id2> [...]]]\n"
    "\t Polygonizes a single SVO cell.",
    //
    __FILE__, group, DDF_PolygonizeCell);

  //-------------------------------------------------------------------------//
  interp->AddCommand("ddf-polygonize-svo",
    //
    "ddf-polygonize-svo <resName> <octreeId>\n"
    "\t Polygonizes the SVO with the <octreeId> identifier.",
    //
    __FILE__, group, DDF_PolygonizeSVO);
}

// Declare entry point PLUGINFACTORY
ASIPLUGIN(cmdDDF)
