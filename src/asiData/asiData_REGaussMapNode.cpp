//-----------------------------------------------------------------------------
// Created on: 25 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiData_REGaussMapNode.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_REGaussMapNode::asiData_REGaussMapNode() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name,      PID_Name);
  REGISTER_PARAMETER(RealArray, PID_Vectors);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) asiData_REGaussMapNode::Instance()
{
  return new asiData_REGaussMapNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_REGaussMapNode::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
  //
  ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->SetArray(NULL);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_REGaussMapNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_REGaussMapNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! Returns vector by a given zero-based index.
//! \param zeroBasedIndex [in] zero-based index.
//! \return normal vector.
gp_Vec asiData_REGaussMapNode::GetVector(const int zeroBasedIndex) const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->GetArray();

  const int coordIdx = zeroBasedIndex*3;

  return gp_Vec( coords->Value(coordIdx + 0),
                 coords->Value(coordIdx + 1),
                 coords->Value(coordIdx + 2) );
}

//! \return stored vectors in form of base cloud.
Handle(asiAlgo_BaseCloud<double>) asiData_REGaussMapNode::GetVectors() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloud(coords);
}

//! \return stored vectors with float coordinates.
Handle(asiAlgo_BaseCloud<float>) asiData_REGaussMapNode::GetVectorsf() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->GetArray();

  if ( coords.IsNull() )
    return NULL;

  // TODO: get rid of copying here
  return asiAlgo_PointCloudUtils::AsPointCloudf(coords);
}

//! Stores vectors.
//! \param vectors [in] vectors to store.
void asiData_REGaussMapNode::SetVectors(const Handle(asiAlgo_BaseCloud<double>)& vectors)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = vectors.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(vectors);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->SetArray(coords);
}

//! Stores vectors with float coordinates.
//! \param vectors [in] normals to store.
void asiData_REGaussMapNode::SetVectorsf(const Handle(asiAlgo_BaseCloud<float>)& vectors)
{
  // TODO: get rid of copying here
  Handle(TColStd_HArray1OfReal)
    coords = vectors.IsNull() ? NULL : asiAlgo_PointCloudUtils::AsRealArray(vectors);

  // Store
  ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->SetArray(coords);
}

//! \return true if the stored vector field is empty, false -- otherwise.
bool asiData_REGaussMapNode::IsEmpty() const
{
  Handle(TColStd_HArray1OfReal)
    coords = ActParamTool::AsRealArray( this->Parameter(PID_Vectors) )->GetArray();

  return coords.IsNull();
}
