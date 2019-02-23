//-----------------------------------------------------------------------------
// Created on: 28 November 2016
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
#include <asiAlgo_BaseCloud.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>

// Instantiate for allowed types
template class asiAlgo_BaseCloud<double>;
template class asiAlgo_BaseCloud<float>;

//-----------------------------------------------------------------------------

template <typename TCoordType>
asiAlgo_BaseCloud<TCoordType>::asiAlgo_BaseCloud() {}

//-----------------------------------------------------------------------------

template <typename TCoordType>
asiAlgo_BaseCloud<TCoordType>::asiAlgo_BaseCloud(const std::vector<TCoordType>& coords)
: m_coords(coords) {}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_BaseCloud<TCoordType>::CopyTo(asiAlgo_BaseCloud<TCoordType>& copy) const
{
  copy.m_coords = m_coords;
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_BaseCloud<TCoordType>::Reserve(const int nElems)
{
  m_coords.resize(nElems*3);
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
int asiAlgo_BaseCloud<TCoordType>::GetNumberOfElements() const
{
  return (int) (m_coords.size() / 3);
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
bool asiAlgo_BaseCloud<TCoordType>::IsEmpty() const
{
  return m_coords.size() == 0;
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_BaseCloud<TCoordType>::AddElement(const TCoordType x,
                                               const TCoordType y,
                                               const TCoordType z)
{
  m_coords.push_back(x);
  m_coords.push_back(y);
  m_coords.push_back(z);
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_BaseCloud<TCoordType>::AddElement(const gp_XYZ xyz)
{
  this->AddElement( TCoordType( xyz.X() ), TCoordType( xyz.Y() ), TCoordType( xyz.Z() ) );
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_BaseCloud<TCoordType>::SetElement(const int        elemIndex,
                                               const TCoordType x,
                                               const TCoordType y,
                                               const TCoordType z)
{
  m_coords[elemIndex*3 + 0] = x;
  m_coords[elemIndex*3 + 1] = y;
  m_coords[elemIndex*3 + 2] = z;
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_BaseCloud<TCoordType>::GetElement(const int   elemIndex,
                                               TCoordType& x,
                                               TCoordType& y,
                                               TCoordType& z) const
{
  x = m_coords[3*elemIndex + 0];
  y = m_coords[3*elemIndex + 1];
  z = m_coords[3*elemIndex + 2];
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
gp_XYZ asiAlgo_BaseCloud<TCoordType>::GetElement(const int elemIndex) const
{
  TCoordType x, y, z;
  this->GetElement(elemIndex, x, y, z);

  return gp_XYZ(x, y, z);
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
const std::vector<TCoordType>& asiAlgo_BaseCloud<TCoordType>::GetCoords() const
{
  return m_coords;
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
Handle(TColStd_HArray1OfReal) asiAlgo_BaseCloud<TCoordType>::GetCoordsArray() const
{
  return asiAlgo_PointCloudUtils::AsRealArray(this);
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
std::vector<TCoordType>& asiAlgo_BaseCloud<TCoordType>::ChangeCoords()
{
  return m_coords;
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_BaseCloud<TCoordType>::ComputeBoundingBox(TCoordType& xMin, TCoordType& xMax,
                                                       TCoordType& yMin, TCoordType& yMax,
                                                       TCoordType& zMin, TCoordType& zMax) const
{
  TCoordType _xMin = std::numeric_limits<TCoordType>::max();
  TCoordType _yMin = _xMin;
  TCoordType _zMin = _xMin;
  TCoordType _xMax = std::numeric_limits<TCoordType>::min();
  TCoordType _yMax = _xMax;
  TCoordType _zMax = _xMax;

  const int nElems = this->GetNumberOfElements();
  //
  for ( int e = 0; e < nElems; ++e )
  {
    TCoordType x, y, z;
    this->GetElement(e, x, y, z);

    if ( x > _xMax )
      _xMax = x;
    if ( x < _xMin )
      _xMin = x;
    if ( y > _yMax )
      _yMax = y;
    if ( y < _yMin )
      _yMin = y;
    if ( z > _zMax )
      _zMax = z;
    if ( z < _zMin )
      _zMin = z;
  }

  xMin = _xMin;
  xMax = _xMax;
  yMin = _yMin;
  yMax = _yMax;
  zMin = _zMin;
  zMax = _zMax;
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
Handle(asiAlgo_BaseCloud<TCoordType>)
  asiAlgo_BaseCloud<TCoordType>::ExtractRegion(const asiAlgo_CloudRegion& region) const
{
  // Extracted base cloud
  Handle(asiAlgo_BaseCloud<TCoordType>) result  = new asiAlgo_BaseCloud<TCoordType>;

  // Extract
  for ( asiAlgo_CloudRegion::Iterator it(region); it.More(); it.Next() )
  {
    const int pidx = it.Key();

    TCoordType x, y, z;
    this->GetElement(pidx, x, y, z);
    result->AddElement(x, y, z);
  }

  return result;
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_BaseCloud<TCoordType>::Merge(const Handle(asiAlgo_BaseCloud<TCoordType>)& cloud)
{
  const int nElems2Add = cloud->GetNumberOfElements();
  //
  for ( int e = 0; e < nElems2Add; ++e )
  {
    TCoordType x, y, z;
    cloud->GetElement(e, x, y, z);

    this->AddElement(x, y, z);
  }
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_BaseCloud<TCoordType>::Clear()
{
  m_coords.clear();
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
bool asiAlgo_BaseCloud<TCoordType>::Load(const char* filename)
{
  std::ifstream FILE(filename);
  if ( !FILE.is_open() )
    return false;

  while ( !FILE.eof() )
  {
    char str[256];
    FILE.getline(str, 256);

    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::copy( std::istream_iterator<std::string>(iss),
               std::istream_iterator<std::string>(),
               std::back_inserter< std::vector<std::string> >(tokens) );

    if ( tokens.empty() || tokens.size() < 3 )
      continue;

    TCoordType x = (TCoordType) ( atof(tokens[0].c_str()) ),
               y = (TCoordType) ( atof(tokens[1].c_str()) ),
               z = (TCoordType) ( atof(tokens[2].c_str()) );

    this->AddElement(x, y, z);
  }

  FILE.close();
  return true;
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
bool asiAlgo_BaseCloud<TCoordType>::SaveAs(const char* filename) const
{
  std::ofstream FILE(filename);
  if ( !FILE.is_open() )
    return false;

  for ( int e = 0; e < this->GetNumberOfElements(); ++e )
  {
    TCoordType x, y, z;
    this->GetElement(e, x, y, z);

    FILE << x << " " << y << " " << z << "\n";
  }

  FILE.close();
  return true;
}
