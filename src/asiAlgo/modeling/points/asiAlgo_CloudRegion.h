//-----------------------------------------------------------------------------
// Created on: 22 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_CloudRegion_h
#define asiAlgo_CloudRegion_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

// STD includes
#include <vector>

//! Region in a base cloud. This is nothing but a set of indices wrapped with
//! a convenience interface.
struct asiAlgo_CloudRegion
{
  TColStd_PackedMapOfInteger indices; //!< Indices of region points.

  asiAlgo_CloudRegion() {}
  //
  asiAlgo_CloudRegion(const int oneSeed) { indices.Add(oneSeed); }
  //
  asiAlgo_CloudRegion(const TColStd_PackedMapOfInteger& set) { indices = set; }
  //
  asiAlgo_CloudRegion(const std::vector<int>& vector)
  {
    for ( size_t k = 0; k < vector.size(); ++k ) indices.Add(vector[k]);
  }

  void add      (const int pidx)       { indices.Add(pidx);                 }
  int  size     ()               const { return indices.Extent();           }
  bool is_empty ()               const { return indices.IsEmpty();          }
  void del      (const int pidx)       { indices.Remove(pidx);              }
  bool contains (const int pidx) const { return indices.Contains(pidx);     }
  int  any      ()               const { return indices.GetMinimalMapped(); }
  void clear    ()                     { indices.Clear();                   }

  asiAlgo_CloudRegion operator-(const asiAlgo_CloudRegion& op) const
  {
    asiAlgo_CloudRegion result(this->indices);
    result.indices.Subtract(op.indices);
    return result;
  }

  typedef TColStd_MapIteratorOfPackedMapOfInteger Iterator;
};

#endif
