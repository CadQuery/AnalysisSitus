//-----------------------------------------------------------------------------
// Created on: 23 September 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_BVHIterator_h
#define asiAlgo_BVHIterator_h

// Analysis Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <BVH_Types.hxx>
#include <BVH_PrimitiveSet.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

//! Depth-first iterator for BVH structure.
//!
//! Stored data of each BVH node has the following meaning:
//! <pre>
//! ========================================================
//!  coord |         leaf        |        sub-volume       |
//! ========================================================
//!    x   |          !0         |             0           |
//! -------+---------------------+-------------------------+
//!    y   | idx of start grain  | idx of left child node  |
//! -------+---------------------+-------------------------+
//!    z   |  idx of end grain   | idx of right child node |
//! ========================================================
//! </pre>
class asiAlgo_BVHIterator
{
  typedef int BVH_Item; // Local jargon (node in BVH).

public:

  asiAlgo_EXPORT
    asiAlgo_BVHIterator(const NCollection_Handle<BVH_Tree<double, 4> >& bvh);

public:

  asiAlgo_EXPORT bool
    More() const;

  asiAlgo_EXPORT const BVH_Vec4i&
    Current() const;

  asiAlgo_EXPORT int
    CurrentIndex() const;

  asiAlgo_EXPORT void
    Next();

  asiAlgo_EXPORT bool
    IsLeaf() const;

  asiAlgo_EXPORT void
    BlockLeft();

  asiAlgo_EXPORT void
    BlockRight();

protected:

  NCollection_Handle< BVH_Tree<double, 4> > m_bvh; //!< Structure to iterate over.

  // Iteration state variables
  BVH_Item                   m_stack[96];   //!< Non-traversed nodes to return.
  int                        m_iStackHead;  //!< Pointer to the stack head.
  BVH_Item                   m_currentNode; //!< Current node.
  TColStd_PackedMapOfInteger m_blocked;     //!< Nodes to stop traverse (their
                                            //!< children will be skipped).

};

#endif 
