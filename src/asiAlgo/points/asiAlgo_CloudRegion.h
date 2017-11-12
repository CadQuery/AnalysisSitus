//-----------------------------------------------------------------------------
// Created on: 22 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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

  //-------------------------------------------------------------------------//

  asiAlgo_CloudRegion()
  {
    indices = new TColStd_HPackedMapOfInteger;
  }
  //
  asiAlgo_CloudRegion(const int oneSeed)
  {
    indices = new TColStd_HPackedMapOfInteger;
    indices->ChangeMap().Add(oneSeed);
  }
  //
  asiAlgo_CloudRegion(const TColStd_PackedMapOfInteger& set)
  {
    indices = new TColStd_HPackedMapOfInteger;
    indices->ChangeMap() = set;
  }
  //
  asiAlgo_CloudRegion(const Handle(TColStd_HPackedMapOfInteger)& set)
  {
    indices = new TColStd_HPackedMapOfInteger;
    indices->ChangeMap() = set->Map();
  }
  //
  asiAlgo_CloudRegion(const std::vector<int>& vector)
  {
    indices = new TColStd_HPackedMapOfInteger;
    //
    for ( size_t k = 0; k < vector.size(); ++k )
      indices->ChangeMap().Add(vector[k]);
  }

  //-------------------------------------------------------------------------//

  void add(const int pidx)
  {
    if ( !indices.IsNull() )
      indices->ChangeMap().Add(pidx);
  }

  //-------------------------------------------------------------------------//

  int size() const
  {
    if ( indices.IsNull() )
      return 0;

    return indices->ChangeMap().Extent();
  }

  //-------------------------------------------------------------------------//

  bool is_empty() const
  {
    if ( indices.IsNull() )
      return true;

    return indices->ChangeMap().IsEmpty();
  }

  //-------------------------------------------------------------------------//

  void del(const int pidx)
  {
    if ( !indices.IsNull() )
      indices->ChangeMap().Remove(pidx);
  }

  //-------------------------------------------------------------------------//

  bool contains(const int pidx) const
  {
    if ( indices.IsNull() )
      return false;

    return indices->ChangeMap().Contains(pidx);
  }

  //-------------------------------------------------------------------------//

  int any() const
  {
    if ( indices.IsNull() )
      return -1;

    return indices->ChangeMap().GetMinimalMapped();
  }

  //-------------------------------------------------------------------------//

  void clear()
  {
    if ( !indices.IsNull() )
      indices->ChangeMap().Clear();
  }

  //-------------------------------------------------------------------------//

  asiAlgo_CloudRegion operator-(const asiAlgo_CloudRegion& op) const
  {
    asiAlgo_CloudRegion result(this->indices);
    result.indices->ChangeMap().Subtract( op.indices->ChangeMap() );
    return result;
  }

  //-------------------------------------------------------------------------//

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
