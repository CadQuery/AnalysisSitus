//-----------------------------------------------------------------------------
// Created on: 15 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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

// Own include
#include <asiAlgo_PointWithAttr.h>

// Instantiate for allowed types
template class asiAlgo_PointWithAttrCloud<gp_XY>;

//-----------------------------------------------------------------------------

template <typename TCoord>
asiAlgo_PointWithAttrCloud<TCoord>::asiAlgo_PointWithAttrCloud()
{}

//-----------------------------------------------------------------------------

template <typename TCoord>
void asiAlgo_PointWithAttrCloud<TCoord>::Clear()
{
  m_pts.clear();
}

//-----------------------------------------------------------------------------

template <typename TCoord>
int asiAlgo_PointWithAttrCloud<TCoord>::GetNumberOfElements() const
{
  return int( m_pts.size() );
}

//-----------------------------------------------------------------------------

template <typename TCoord>
const asiAlgo_PointWithAttr<TCoord>&
  asiAlgo_PointWithAttrCloud<TCoord>::GetElement(const int zeroBasedIndex) const
{
  return m_pts[zeroBasedIndex];
}

//-----------------------------------------------------------------------------

template <typename TCoord>
asiAlgo_PointWithAttr<TCoord>&
  asiAlgo_PointWithAttrCloud<TCoord>::ChangeElement(const int zeroBasedIndex)
{
  return m_pts[zeroBasedIndex];
}

//-----------------------------------------------------------------------------

template <typename TCoord>
void asiAlgo_PointWithAttrCloud<TCoord>::AddElement(const asiAlgo_PointWithAttr<TCoord>& coord)
{
  m_pts.push_back(coord);
}
