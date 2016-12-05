//-----------------------------------------------------------------------------
// Created on: 06 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeAsBuilt_PointCloudNormals.h>

// asiAlgo includes
#include <asiAlgo_PlaneOnPoints.h>

// k-d tree
#include <exeAsBuilt_FlannKdTree.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

#define KDTREE_K 40

//-----------------------------------------------------------------------------

//! Constructs the tool without initializing it.
//! \param progress [in] progress indicator.
//! \param plotter  [in] imperative plotter.
exeAsBuilt_PointCloudNormals::exeAsBuilt_PointCloudNormals(ActAPI_ProgressEntry progress,
                                                           ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  this->Init(NULL);
}

//-----------------------------------------------------------------------------

//! Constructs the tool and performs all useful job.
//! \param kdTree   [in] k-d tree for the working point cloud.
//! \param progress [in] progress indicator.
//! \param plotter  [in] imperative plotter.
exeAsBuilt_PointCloudNormals::exeAsBuilt_PointCloudNormals(const Handle(exeAsBuilt_FlannKdTree)& kdTree,
                                                           ActAPI_ProgressEntry                  progress,
                                                           ActAPI_PlotterEntry                   plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  this->Init(kdTree);
  this->Perform();
}

//-----------------------------------------------------------------------------

//! Initializes the tool with the given k-d tree. This method also resets
//! the execution status to 'not done'.
//! \param kdTree [in] k-d tree to set.
void exeAsBuilt_PointCloudNormals::Init(const Handle(exeAsBuilt_FlannKdTree)& kdTree)
{
  m_kdTree = kdTree;
  m_bDone  = false;
}

//-----------------------------------------------------------------------------

//! Builds a normal field for all points in the passed cloud. The output
//! collection of normals is represented with another point cloud, where
//! each "point" is actually a vector associated with a host point by an
//! index. It is the job of the client code to correlate a normal element
//! with a point element assuming that the orders of both collections are
//! kept consistent.
void exeAsBuilt_PointCloudNormals::Perform()
{
  const Handle(asiAlgo_PointCloud<float>)& pointCloud = m_kdTree->GetPointCloud();
  const int                                      nPts = pointCloud->GetNumberOfPoints();

  // Allocate enough space in the output cloud
  m_normals = new asiAlgo_PointCloud<float>;
  m_normals->Reserve(nPts);

  // Prepare a tool to fit local planes. These planes will define the
  // normal vectors
  asiAlgo_PlaneOnPoints PlaneOnPoints( this->Progress(), this->Plotter() );

  // Calculate normal vector for each single point
  for ( size_t ip = 0; ip < nPts; ++ip )
  {
    // Take the next point
    float x, y, z;
    pointCloud->GetPoint(ip, x, y, z);
    gp_Pnt P(x, y, z);

    // Find K nearest neighbors
    std::vector<gp_Pnt> neighbors(KDTREE_K);
    std::vector<int>    indices(KDTREE_K);
    std::vector<float>  distances(KDTREE_K);
    //
    m_kdTree->Search(P, KDTREE_K, indices, distances);

    // Get neighbor point coordinates
    for ( size_t k = 0; k < KDTREE_K; ++k )
    {
      float x, y, z;
      pointCloud->GetPoint(indices[k], x, y, z);
      neighbors[k] = gp_Pnt(x, y, z);
    }

    // Build a fitting plane
    gp_Pln plane;
    //
    float nx, ny, nz;
    if ( !PlaneOnPoints.Build(neighbors, plane) )
    {
      std::cout << "Warning: cannot build fitting plane, normal will be nullified" << std::endl;
      nx = ny = nz = 0;
    }
    else
    {
      const gp_Dir& norm = plane.Axis().Direction();
      nx = (float) norm.X();
      ny = (float) norm.Y();
      nz = (float) norm.Z();
    }

    // Set normal for the current point
    m_normals->SetPoint(ip, nx, ny, nz);
  }

  // Set success execution status
  this->setDone(true);
}

//-----------------------------------------------------------------------------

//! \return execution status.
bool exeAsBuilt_PointCloudNormals::IsDone() const
{
  return m_bDone;
}

//-----------------------------------------------------------------------------

//! \return normal field. Raises an exception if the computation was not done.
const Handle(asiAlgo_PointCloud<float>)&
  exeAsBuilt_PointCloudNormals::GetResult() const
{
  if ( !this->IsDone() )
    Standard_ProgramError::Raise("Not done");

  return m_normals;
}
