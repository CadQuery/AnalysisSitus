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
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

// STD includes
#include <vector>

//! Region in a base cloud. This is nothing but a set of indices wrapped with
//! a convenience interface.
struct asiAlgo_CloudRegion
{
  Handle(TColStd_HPackedMapOfInteger) indices; //!< Indices of region points.

  asiAlgo_CloudRegion() {}
  //
  asiAlgo_CloudRegion(const int oneSeed) { indices->ChangeMap().Add(oneSeed); }
  //
  asiAlgo_CloudRegion(const TColStd_PackedMapOfInteger& set) { indices->ChangeMap() = set; }
  //
  asiAlgo_CloudRegion(const Handle(TColStd_HPackedMapOfInteger)& set) { indices = set; }
  //
  asiAlgo_CloudRegion(const std::vector<int>& vector)
  {
    for ( size_t k = 0; k < vector.size(); ++k ) indices->ChangeMap().Add(vector[k]);
  }

  void add      (const int pidx)       { indices->ChangeMap().Add(pidx);                        }
  int  size     ()               const { return indices->ChangeMap().Extent();                  }
  bool is_empty ()               const { return indices->ChangeMap().IsEmpty();                 }
  void del      (const int pidx)       { indices->ChangeMap().Remove(pidx);                     }
  bool contains (const int pidx) const { return indices->ChangeMap().Contains(pidx);            }
  int  any      ()               const { return indices->ChangeMap().GetMinimalMapped();        }
  void clear    ()                     { if ( !indices.IsNull() ) indices->ChangeMap().Clear(); }

  asiAlgo_CloudRegion operator-(const asiAlgo_CloudRegion& op) const
  {
    asiAlgo_CloudRegion result(this->indices);
    result.indices->ChangeMap().Subtract( op.indices->ChangeMap() );
    return result;
  }

  class Iterator
  {
  public:

    Iterator(const asiAlgo_CloudRegion& region)
    {
      m_it.Initialize( region.indices->Map() );
    }

    bool More() const
    {
      return m_it.More();
    }

    void Next()
    {
      m_it.Next();
    }

    int Key() const
    {
      return m_it.Key();
    }

  protected:

    TColStd_MapIteratorOfPackedMapOfInteger m_it;

  };

};

#endif
