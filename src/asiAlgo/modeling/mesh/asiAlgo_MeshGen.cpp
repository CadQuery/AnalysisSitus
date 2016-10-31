//-----------------------------------------------------------------------------
// Created on: 15 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_MeshGen.h>

// OCCT includes
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//! Generates surface mesh by native OCCT tools.
//! \param shape                 [in]  shape to tessellate.
//! \param linearDeflection      [in]  chord deflection.
//! \param angularDeflection_deg [in]  angular deflection in degrees.
//! \param info                  [out] mesh summary info.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_MeshGen::DoNative(const TopoDS_Shape& shape,
                               const double        linearDeflection,
                               const double        angularDeflection_deg,
                               asiAlgo_MeshInfo&   info)
{
  // Clean up polygonal data
  BRepTools::Clean(shape);

  // Tessellate
  try
  {
    // Notice that parallel mode is enabled
    BRepMesh_IncrementalMesh MeshGen(shape, linearDeflection, 0, angularDeflection_deg, 1);
  }
  catch ( ... )
  {
    std::cout << "B-Rep mesh crash" << std::endl;
    return false;
  }

  //---------------------------------------------------------------------------
  // Accumulate summary info
  //---------------------------------------------------------------------------

  double maxDeflection = 0.0;
  int    nTriangles    = 0;
  int    nNodes        = 0;
  //
  for ( TopExp_Explorer ex(shape, TopAbs_FACE); ex.More(); ex.Next() )
  {
    const TopoDS_Face&                F = TopoDS::Face( ex.Current() );
    TopLoc_Location                   L;
    const Handle(Poly_Triangulation)& T = BRep_Tool::Triangulation(F, L);
    //
    if ( T.IsNull() )
      continue;

    nTriangles += T->NbTriangles();
    nNodes     += T->NbNodes();
    //
    if ( T->Deflection() > maxDeflection )
      maxDeflection = T->Deflection();
  }

  // Store results
  info.maxDeflection = maxDeflection;
  info.nFacets       = nTriangles;
  info.nNodes        = nNodes;

  return true;
}
