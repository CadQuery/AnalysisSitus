//-----------------------------------------------------------------------------
// Created on: 22 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
