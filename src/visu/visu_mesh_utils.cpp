//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_utils.h>

//! Initializes VTK lookup table charged with default color scheme for meshes.
//! \return VTK lookup table.
vtkSmartPointer<vtkLookupTable> visu_mesh_utils::InitLookupTable()
{
  vtkSmartPointer<vtkLookupTable> aLookup = vtkSmartPointer<vtkLookupTable>::New();

  double aRange[2] = {0, MeshItem_FreeNode};
  aLookup->SetRange(aRange);
  aLookup->SetNumberOfColors(MeshItem_Last);
  
  aLookup->SetTableValue(MeshItem_Triangle,   0.1, 0.4, 0.8);
  aLookup->SetTableValue(MeshItem_Quadrangle, 0.2, 0.6, 1.0);
  aLookup->SetTableValue(MeshItem_FreeNode,   1.0, 0.0, 0.0);

  return aLookup;
}

//! Initializes the passed VTK mapper with the given Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theLookup         [in]     Lookup Table to initialize the mapper with.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
void visu_mesh_utils::InitMapper(vtkMapper*      theMapper,
                                 vtkLookupTable* theLookup,
                                 const char*     theScalarsArrName)
{
  theMapper->ScalarVisibilityOn();
  theMapper->SetScalarModeToUseCellFieldData();
  theMapper->SelectColorArray(theScalarsArrName);
  theMapper->SetColorModeToMapScalars();
  theMapper->SetScalarRange( theLookup->GetRange() );
  theMapper->SetLookupTable(theLookup);
  theMapper->Update();
}

//! Initializes the passed VTK mapper with the default Lookup Table.
//! \param theMapper         [in/out] mapper to initialize.
//! \param theScalarsArrName [in]     name of the array storing the scalars
//!                                   for colorization.
void visu_mesh_utils::InitMapper(vtkMapper*  theMapper,
                                 const char* theScalarsArrName)
{
  vtkSmartPointer<vtkLookupTable> aLookup = InitLookupTable();
  InitMapper(theMapper, aLookup, theScalarsArrName);
}

//! Returns default shrink factor for meshes.
//! \return shrink factor.
double visu_mesh_utils::DefaultShrinkFactor()
{
  return 0.8;
}

//! Returns default point size used for representation of mesh nodes.
//! \return point size in pixels.
double visu_mesh_utils::DefaultPointSize()
{
  return 5;
}

//! Returns default color for mesh contour.
//! \param fR [out] red component [0;1].
//! \param fG [out] green component [0;1].
//! \param fB [out] blud component [0;1].
void visu_mesh_utils::DefaultContourColor(double& fR,
                                          double& fG,
                                          double& fB)
{
  fR = fG = fB = 0.0; // Everything is BLACK
}

//! Returns default opacity for mesh contour.
//! \return opacity factor.
double visu_mesh_utils::DefaultContourOpacity()
{
  return 0.2;
}
