//-----------------------------------------------------------------------------
// Created on: 23 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// FLANN includes
#pragma warning(push, 0)
#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#pragma warning(pop)

// STD includes
#include <stdio.h>

//-----------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------

//! main().
int main(int argc, char** argv)
{
  int nn = 3;
  flann::Matrix<float> dataset;
  flann::Matrix<float> query;
  flann::load_from_file(dataset, "dataset.hdf5","dataset");
  flann::load_from_file(query, "dataset.hdf5","query");
  flann::Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
  flann::Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

  // construct an randomized kd-tree index using 4 kd-trees
  flann::Index<flann::L2<float> > index(dataset, flann::KDTreeIndexParams(4));
  index.buildIndex();

  // do a knn search, using 128 checks
  index.knnSearch(query, indices, dists, nn, flann::SearchParams(128));
  flann::save_to_file(indices,"result.hdf5","result");

  delete[] dataset.ptr();
  delete[] query.ptr();
  delete[] indices.ptr();
  delete[] dists.ptr();

  return 0;
}
