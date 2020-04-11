//-----------------------------------------------------------------------------
// Created on: 23 January 2020
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

#ifndef asiData_OctreeNode_h
#define asiData_OctreeNode_h

// asiData includes
#include <asiData_OctreeParameter.h>
#include <asiData_UniformGridParameter.h>

// asiAlgo includes
#include <asiAlgo_CSG.h>

// Active Data includes
#include <ActData_BaseNode.h>

// Forward declarations.
class asiAlgo_BVHFacets;

//-----------------------------------------------------------------------------

//! Node representing octree spatial decomposition.
class asiData_OctreeNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_OctreeNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_OctreeNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //----------------------//
    PID_Name,             //!< Name of the Node.
    PID_Operation,        //!< CSG operation.
    PID_BVH,              //!< BVH used for distance function evaluation.
    PID_Octree,           //!< Octree decomposition of the space occupied by the part.
    PID_UniformGrid,      //!< Uniform decomposition of the space occupied by the part.
    PID_BuildFunc,        //!< Tree Function Parameter to build the octree.
    PID_OpLeft,           //!< Left operand for a Boolean function.
    PID_OpRight,          //!< Right operand for a Boolean function.
  //----------------------//
    PID_ResolutionGroup,  //!< Group for octree resolution parameters.
    PID_MinCellSize,      //!< Min cell size.
    PID_MaxCellSize,      //!< Max cell size.
    PID_Precision,        //!< Precision.
    PID_IsUniform,        //!< Indicates whether the voxelization is uniform.
    PID_NumRaysSign,      //!< Number of rays to use for sign resolution.
  //----------------------//
    PID_DomainGroup,      //!< Group of domain corner Parameters.
    PID_DomainMinX,       //!< X coordinate of the min domain corner.
    PID_DomainMinY,       //!< Y coordinate of the min domain corner.
    PID_DomainMinZ,       //!< Z coordinate of the min domain corner.
    PID_DomainMaxX,       //!< X coordinate of the max domain corner.
    PID_DomainMaxY,       //!< Y coordinate of the max domain corner.
    PID_DomainMaxZ,       //!< Z coordinate of the max domain corner.
    PID_DomainIsCube,     //!< Indicates whether the domain is a cube.
  //----------------------//
    PID_OutputsGroup,     //!< Group of outputs.
    PID_SamplingStrategy, //!< Sampling strategy.
    PID_ExtractPoints,    //!< Whether to extract points instead of voxels.
    PID_PointSize,        //!< Point size.
    PID_MaxVectorSize,    //!< Max vector size.
    PID_NumElements,      //!< Number of extracted elements (cells or points).
  //----------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  //! Returns new DETACHED instance of the Octree Node ensuring its correct
  //! allocation in a heap.
  //! \return new instance of the Octree Node.
  asiData_EXPORT static Handle(ActAPI_INode)
    Instance();

// Initialization:
public:

  //! Performs initial actions required to make the Octree Node WELL-FORMED.
  asiData_EXPORT void
    Init();

// Generic naming support:
public:

  //! Accessor for the Node's name.
  //! \return name of the Node.
  asiData_EXPORT virtual TCollection_ExtendedString
    GetName();

  //! Sets name for the Node.
  //! \param[in] name name to set.
  asiData_EXPORT virtual void
    SetName(const TCollection_ExtendedString&);

// Handy accessors to the stored data:
public:

  //! \return stored CSG operation type.
  asiData_EXPORT asiAlgo_CSG
    GetOperation() const;

  //! Sets CSG operation type to store.
  //! \param[in] op operation type to set.
  asiData_EXPORT void
    SetOperation(const asiAlgo_CSG op);

  //! \return BVH structure.
  asiData_EXPORT Handle(asiAlgo_BVHFacets)
    GetBVH() const;

  //! Sets BVH structure to store.
  //! \param[in] bvhFacets BVH structure to store.
  asiData_EXPORT void
    SetBVH(const Handle(asiAlgo_BVHFacets)& bvhFacets);

  //! \return void pointer to octree data structure.
  asiData_EXPORT void*
    GetOctree() const;

  //! Sets octree to store.
  //! \param[in] pOctree octree to store.
  asiData_EXPORT void
    SetOctree(void* pOctree);

  //! \return uniform grid.
  asiData_EXPORT Handle(asiAlgo_UniformGrid<float>)
    GetUniformGrid() const;

  //! Sets the uniform grid to store.
  //! \param[in] grid uniform grid to store.
  asiData_EXPORT void
    SetUniformGrid(const Handle(asiAlgo_UniformGrid<float>)& grid);

  //! \return sampling strategy as integer.
  asiData_EXPORT int
    GetSamplingStrategy() const;

  //! Sets sampling strategy.
  //! \param[in] mode value to set.
  asiData_EXPORT void
    SetSamplingStrategy(const int mode);

  //! \return true if the points extraction mode is enabled, false -- otherwise.
  asiData_EXPORT bool
    GetExtractPoints() const;

  //! Sets points extraction mode.
  //! \param[in] toExtract true/false.
  asiData_EXPORT void
    SetExtractPoints(const bool toExtract);

  //! \return point size.
  asiData_EXPORT double
    GetPointSize() const;

  //! Sets the point size.
  //! \param[in] size value to set.
  asiData_EXPORT void
    SetPointSize(const double size);

  //! \return max vector size.
  asiData_EXPORT double
    GetMaxVectorSize() const;

  //! Sets the max vector size.
  //! \param[in] maxSize value to set.
  asiData_EXPORT void
    SetMaxVectorSize(const double maxSize);

  //! \return number of elements (points or cells).
  asiData_EXPORT int
    GetNumElements() const;

  //! Sets the number of elements.
  //! \param[in] numElems the number of elements to store.
  asiData_EXPORT void
    SetNumElements(const int numElems);

  //! Returns the coordindates of the min domain corner.
  //! \param[out] x X coordinate.
  //! \param[out] y Y coordinate.
  //! \param[out] z Z coordinate.
  asiData_EXPORT void
    GetDomainMinCorner(double& x, double& y, double& z) const;

  //! Sets the coordindates of the min domain corner.
  //! \param[in] x X coordinate.
  //! \param[in] y Y coordinate.
  //! \param[in] z Z coordinate.
  asiData_EXPORT void
    SetDomainMinCorner(const double x, const double y, const double z);

  //! Returns the coordindates of the max domain corner.
  //! \param[out] x X coordinate.
  //! \param[out] y Y coordinate.
  //! \param[out] z Z coordinate.
  asiData_EXPORT void
    GetDomainMaxCorner(double& x, double& y, double& z) const;

  //! Sets the coordindates of the max domain corner.
  //! \param[in] x X coordinate.
  //! \param[in] y Y coordinate.
  //! \param[in] z Z coordinate.
  asiData_EXPORT void
    SetDomainMaxCorner(const double x, const double y, const double z);

  //! \return true/false.
  asiData_EXPORT bool
    IsDomainCube() const;

  //! Sets the flag indicating whether the octree definition domain
  //! is a bounding cube.
  //! \param[in] isCube flag to set.
  asiData_EXPORT void
    SetDomainCube(const bool isCube);

  //! \return min cell size.
  asiData_EXPORT double
    GetMinCellSize() const;

  //! Sets min cell size.
  //! \param[in] cellSize value to set.
  asiData_EXPORT void
    SetMinCellSize(const double cellSize);

  //! \return max cell size.
  asiData_EXPORT double
    GetMaxCellSize() const;

  //! Sets max cell size.
  //! \param[in] cellSize value to set.
  asiData_EXPORT void
    SetMaxCellSize(const double cellSize);

  //! \return voxelization precision.
  asiData_EXPORT double
    GetPrecision() const;

  //! Sets precision.
  //! \param[in] value value to set.
  asiData_EXPORT void
    SetPrecision(const double value);

  //! \return true if the uniform voxelization mode is enabled.
  asiData_EXPORT bool
    IsUniform() const;

  //! Enables/disables the uniform voxelization mode.
  //! \param[in] isUniform value to set.
  asiData_EXPORT void
    SetUniform(const bool isUniform);

  //! \return number of rays used to define the signs of the distance values
  //!         in ADF computation.
  asiData_EXPORT int
    GetNumRaysSign() const;

  //! Sets the number of rays used for determining distance sign in each corner
  //! of each voxel.
  //! \param[in] numRays number of rays to set.
  asiData_EXPORT void
    SetNumRaysSign(const int numRays);

protected:

  //! Default ctor. Registers all involved Parameters.
  //! Allocation is allowed only via Instance() method.
  asiData_EXPORT
    asiData_OctreeNode();

};

#endif
