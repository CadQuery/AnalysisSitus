//-----------------------------------------------------------------------------
// Created on: 22 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_PointCloudNormals.h>

// PCL includes
//#include <pcl/io/pcd_io.h>
//#include <pcl/point_cloud.h>
//#include <pcl/point_types.h>
//#include <pcl/features/normal_3d.h>

//! Calculates normal field for the point cloud.
//! \param points [in] input points.
void asiAlgo_PointCloudNormals::Calculate(const Handle(asiAlgo_PointCloud)& /*points*/)
{
  //pcl::PointCloud<pcl::PointXYZ>::Ptr cloud( new pcl::PointCloud<pcl::PointXYZ> );

  //// Fill pcl data structure
  //Handle(TColStd_HArray1OfReal) coords = points->GetPoints();
  ////
  //for ( int cidx = coords->Lower(); cidx <= coords->Upper(); cidx += 3 )
  //{
  //  const double x = coords->Value(cidx);
  //  const double y = coords->Value(cidx + 1);
  //  const double z = coords->Value(cidx + 2);
  //  //
  //  cloud->push_back( pcl::PointXYZ(x, y, z) );
  //}

  //pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
  //std::string filename("C:/Work/nnnp/pcl/pcl-1.7.2-src/test/car6.pcd");
  //if (pcl::io::loadPCDFile<pcl::PointXYZ> (filename, *cloud) == -1) // load the file
  //{
  //  PCL_ERROR ("Couldn't read file");
  //  return;
  //}

  //std::cout << "points: " << cloud->points.size () << std::endl;

  //// Create the normal estimation class, and pass the input dataset to it
  //pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> ne;
  //ne.setInputCloud (cloud);

  //// Create an empty kd-tree representation, and pass it to the normal estimation object.
  //// Its content will be filled inside the object, based on the given input dataset (as no other search surface is given).
  //pcl::search::KdTree<pcl::PointXYZ>::Ptr tree( new pcl::search::KdTree<pcl::PointXYZ>()) ;
  //ne.setSearchMethod(tree);

  //// Output datasets
  //pcl::PointCloud<pcl::Normal>::Ptr cloud_normals(new pcl::PointCloud<pcl::Normal>);

  //// Use all neighbors in a sphere of radius 3cm
  //ne.setRadiusSearch(0.03);

  //// Compute the features
  //ne.compute(*cloud_normals);
}
