//-----------------------------------------------------------------------------
// Created on: 23 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_bvh_iterator_h
#define geom_bvh_iterator_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <BVH_Types.hxx>
#include <BVH_PrimitiveSet.hxx>

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
class geom_bvh_iterator
{
  typedef int BVH_Item; // Local jargon (node in BVH).

public:

  geom_bvh_iterator(const NCollection_Handle<BVH_Tree<double, 4> >& bvh);

public:

  bool
    More() const;

  const BVH_Vec4i&
    Current() const;

  int
    CurrentIndex() const;

  void
    Next();

  bool
    IsLeaf() const;

  void
    BlockLeft();

  void
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
