//-----------------------------------------------------------------------------
// Created on: 28 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_PointCloud.h>

// Instantiate for allowed types
template class asiAlgo_PointCloud<double>;
template class asiAlgo_PointCloud<float>;

//-----------------------------------------------------------------------------

template <typename TCoordType>
asiAlgo_PointCloud<TCoordType>::asiAlgo_PointCloud() {}

//-----------------------------------------------------------------------------

template <typename TCoordType>
asiAlgo_PointCloud<TCoordType>::asiAlgo_PointCloud(const std::vector<TCoordType>& coords)
  : m_coords(coords) {}

//-----------------------------------------------------------------------------

template <typename TCoordType>
int asiAlgo_PointCloud<TCoordType>::GetNumberOfPoints() const
{
  return (int) ( m_coords.size() / 3 );
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_PointCloud<TCoordType>::AddPoint(const TCoordType x,
                                              const TCoordType y,
                                              const TCoordType z)
{
  m_coords.push_back(x);
  m_coords.push_back(y);
  m_coords.push_back(z);
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
void asiAlgo_PointCloud<TCoordType>::GetPoint(const int   pointIndex,
                                              TCoordType& x,
                                              TCoordType& y,
                                              TCoordType& z) const
{
  x = m_coords[3*pointIndex + 0];
  y = m_coords[3*pointIndex + 1];
  z = m_coords[3*pointIndex + 2];
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
const std::vector<TCoordType>& asiAlgo_PointCloud<TCoordType>::GetCoords() const
{
  return m_coords;
}

//-----------------------------------------------------------------------------

template <typename TCoordType>
std::vector<TCoordType>& asiAlgo_PointCloud<TCoordType>::ChangeCoords()
{
  return m_coords;
}

//-----------------------------------------------------------------------------

//! Reads point cloud recorded in the input file with common XYZ format. That
//! is, the file contains just point coordinates without any additional
//! structuring information.
//! \param filename [in] file to read.
//! \return true in case of success, false -- otherwise.
template <typename TCoordType>
bool asiAlgo_PointCloud<TCoordType>::Load(const char* filename)
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

    this->AddPoint(x, y, z);
  }

  FILE.close();
  return true;
}

//-----------------------------------------------------------------------------

//! Writes point cloud to file with given filename.
//! \param filename [in] file to write into.
//! \return true in case of success, false -- otherwise.
template <typename TCoordType>
bool asiAlgo_PointCloud<TCoordType>::SaveAs(const char* filename) const
{
  std::ofstream FILE(filename);
  if ( !FILE.is_open() )
    return false;

  for ( int p = 0; p < this->GetNumberOfPoints(); ++p )
  {
    TCoordType xp, yp, zp;
    this->GetPoint(p, xp, yp, zp);

    FILE << xp << " " << yp << " " << zp << "\n";
  }

  FILE.close();
  return true;
}
