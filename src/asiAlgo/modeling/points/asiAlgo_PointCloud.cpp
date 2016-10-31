//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_PointCloud.h>

// Qr (geometry) includes
#include <QrGeom3D_SectionCloud.h>

// Qr (tools) includes
#include <QrTools_ReadXYZ.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Vertex.hxx>

//-----------------------------------------------------------------------------

//! Performs point cloud import.
//! \param filename [in]  file to read.
//! \param result   [out] retrieved shape.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_PointCloud::ReadSections(const TCollection_AsciiString& filename,
                                      TopoDS_Shape&                  result)
{
  // Read points distributed by sections
  QrTools_ReadXYZ ReadXYZ;
  QrPtr<QrGeom3D_PointCloud> Cloud;
  if ( !ReadXYZ(filename.ToCString(), QrTools_ReadXYZ::Type_CoherentSections, Cloud) )
  {
    std::cout << "Cannot read sections from file" << std::endl;
    return false;
  }

  // Convert to sections cloud
  QrPtr<QrGeom3D_SectionCloud> SectionCloud = QrPtr<QrGeom3D_SectionCloud>::DownCast(Cloud);
  if ( SectionCloud.IsNull() )
  {
    std::cout << "Cannot treat point cloud as sections cloud" << std::endl;
    return false;
  }

  // Convert to topology
  BRep_Builder BB;
  TopoDS_Compound CloudShape;
  BB.MakeCompound(CloudShape);
  //
  for ( int s = 0; s < SectionCloud->NumberOfSections(); ++s )
  {
    QrPtr<QrGeom3D_PositionCloud> section = SectionCloud->SectionByIndex(s)->Pts;
    //
    TopoDS_Compound SectionShape;
    BB.MakeCompound(SectionShape);

    for ( int p = 0; p < section->NumberOfPoints(); ++p )
    {
      TopoDS_Vertex v = BRepBuilderAPI_MakeVertex( gp_Pnt( section->Point(p).X(),
                                                           section->Point(p).Y(),
                                                           section->Point(p).Z() ) );
      BB.Add(SectionShape, v);
    }
    BB.Add(CloudShape, SectionShape);
  }

  // Set result
  result = CloudShape;
  return true;
}

//! Performs point cloud import.
//! \param filename [in]  file to read.
//! \param result   [out] retrieved shape.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_PointCloud::ReadScattered(const TCollection_AsciiString& filename,
                                       TopoDS_Shape&                  result)
{
  // Read points as positional (scattered) cloud
  QrTools_ReadXYZ ReadXYZ;
  QrPtr<QrGeom3D_PointCloud> Cloud;
  if ( !ReadXYZ(filename.ToCString(), QrTools_ReadXYZ::Type_Positions, Cloud) )
  {
    std::cout << "Cannot read scattered points from file" << std::endl;
    return false;
  }

  // Convert to positional cloud
  QrPtr<QrGeom3D_PositionCloud> PositionCloud = QrPtr<QrGeom3D_PositionCloud>::DownCast(Cloud);
  if ( PositionCloud.IsNull() )
  {
    std::cout << "Cannot treat point cloud as scattered cloud" << std::endl;
    return false;
  }

  // Convert to topology
  BRep_Builder BB;
  TopoDS_Compound CloudShape;
  BB.MakeCompound(CloudShape);
  //
  for ( int p = 0; p < PositionCloud->NumberOfPoints(); ++p )
  {
    TopoDS_Vertex v = BRepBuilderAPI_MakeVertex( gp_Pnt( PositionCloud->Point(p).X(),
                                                         PositionCloud->Point(p).Y(),
                                                         PositionCloud->Point(p).Z() ) );
    BB.Add(CloudShape, v);
  }

  // Set result
  result = CloudShape;
  return true;
}

//! Removes coincident points from the given collection and puts result
//! back in the same argument.
//! \param prec [in]     precision for coincidence test.
//! \param pts  [in/out] points to purify.
void asiAlgo_PointCloud::RemoveCoincidentPoints(const double                  prec,
                                                asiAlgo_IntersectionPointsCC& pts)
{
  asiAlgo_IntersectionPointsCC purifiedList;
  int ip = 1;
  do
  {
    const Handle(asiAlgo_IntersectionPointCC)& p = pts(ip);

    // Traverse till the end skipping all coincident points
    int jp = ip + 1;
    Handle(asiAlgo_IntersectionPointCC) p_next, p_best = p;
    for ( ; ; )
    {
      ip = jp;

      if ( jp > pts.Length() )
        break;

      p_next = pts(jp++);

      if ( p_next->P.Distance(p->P) < prec )
      {
        // If there are coincident points, prefer the one which has
        // more complete definition
        if ( !p_next->pC1.IsNull() && !p_next->pC2.IsNull() )
          p_best = p_next;
      }
      else
        break;
    }

    purifiedList.Append(p_best);
  }
  while ( ip <= pts.Length() );

  // Override the passed list
  pts = purifiedList;
}

//! Creates a point cloud by constructing it from intersection results.
//! \param pts [in] intersection points to create an ordinary point cloud from.
//! \return newly created point cloud.
Handle(asiAlgo_PointCloud)
  asiAlgo_PointCloud::FromIntersection(const asiAlgo_IntersectionPointsCC& pts)
{
  Handle(TColStd_HArray1OfReal) coords = new TColStd_HArray1OfReal(0, pts.Length()*3 - 1);
  //
  for ( int p = 0; p < pts.Length(); ++p )
  {
    coords->ChangeValue( p*3 )     = pts(p + 1)->P.X();
    coords->ChangeValue( p*3 + 1 ) = pts(p + 1)->P.Y();
    coords->ChangeValue( p*3 + 2 ) = pts(p + 1)->P.Z();
  }
  Handle(asiAlgo_PointCloud) result = new asiAlgo_PointCloud(coords);
  return result;
}

//-----------------------------------------------------------------------------

//! Constructor.
//! \param progress [in] progress indication.
//! \param plotter  [in] imperative plotter.
asiAlgo_PointCloud::asiAlgo_PointCloud(ActAPI_ProgressEntry progress,
                                       ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//! Constructor accepting coordinates.
//! \param coords   [in] coordinates.
//! \param progress [in] progress indication.
//! \param plotter  [in] imperative plotter.
asiAlgo_PointCloud::asiAlgo_PointCloud(const Handle(TColStd_HArray1OfReal)& coords,
                                       ActAPI_ProgressEntry                 progress,
                                       ActAPI_PlotterEntry                  plotter)
: ActAPI_IAlgorithm(progress, plotter),
  m_coords(coords)
{}

//-----------------------------------------------------------------------------

//! Saves point cloud in ASCII pts format.
//! \param filename [in] filename for the output file.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_PointCloud::SaveAs(const TCollection_AsciiString& filename)
{
  std::ofstream FILE( filename.ToCString() );
  if ( !FILE.is_open() )
    return false;

  for ( int p = 0; p < this->GetNumOfPoints(); ++p )
  {
    gp_Pnt P = this->GetPoint(p);
    FILE << P.X() << " " << P.Y() << " " << P.Z() << "\n";
  }

  // Close file
  FILE.close();
  return true;
}

//-----------------------------------------------------------------------------

//! Loads points from the given ASCII file.
//! \param filename [in] filename of the input file.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_PointCloud::Load(const TCollection_AsciiString& filename)
{
  std::ifstream FILE( filename.ToCString() );
  if ( !FILE.is_open() )
    return false;

  // Read points
  std::vector<gp_XYZ> pts;
  const bool isOk = this->readPositions(FILE, pts);

  // Store as a plain array of coordinates
  m_coords = new TColStd_HArray1OfReal(0, (int) pts.size()*3 - 1);
  //
  for ( int p = 0; p < pts.size(); ++p )
  {
    m_coords->ChangeValue( p*3 )     = pts[p].X();
    m_coords->ChangeValue( p*3 + 1 ) = pts[p].Y();
    m_coords->ChangeValue( p*3 + 2 ) = pts[p].Z();
  }

  FILE.close();
  return isOk;
}

//-----------------------------------------------------------------------------

//! Reads point cloud recorded in the input file with common XYZ format. That
//! is, the file contains just point coordinates without any additional
//! structuring information.
//! \param FILE  [in]  file handler.
//! \param cloud [out] output point cloud. WARNING: this argument fill be
//!                    filled with data only in case if 
//! \return true in case of success, false -- otherwise.
bool asiAlgo_PointCloud::readPositions(std::ifstream&       FILE,
                                       std::vector<gp_XYZ>& points)
{
  this->Progress().Init(); // Initialize progress notifier

  while ( !FILE.eof() )
  {
    char str[256];
    FILE.getline(str, 256);

    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::copy( std::istream_iterator<std::string>(iss),
               std::istream_iterator<std::string>(),
               std::back_inserter< std::vector<std::string> >(tokens) );

    if ( tokens.empty() )
      continue;

    if ( tokens.size() == 1 ) // One token means that we have the total number
    {                         // of points as the first record. If so, let's
                              // do some initializations
      const int nPoints = QrCore::to_number<int>(tokens[0]);

      // Initialize progress notifier
      this->Progress().Init(nPoints);

      // Initialize array of points
      m_coords = new TColStd_HArray1OfReal(0, nPoints*3 - 1);
    }
    else
    {
      const double x = atof( tokens[0].c_str() ),
                   y = atof( tokens[1].c_str() ),
                   z = atof( tokens[2].c_str() );

      gp_XYZ P(x, y, z);
      points.push_back(P);
      //
      this->Progress().StepProgress(1, 1); // Report progress
    }
  }

  return true;
}
