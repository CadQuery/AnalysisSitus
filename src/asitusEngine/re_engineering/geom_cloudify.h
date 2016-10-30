//-----------------------------------------------------------------------------
// Created on: 02 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_cloudify_h
#define geom_cloudify_h

// A-Situs includes
#include <asitusGeom.h>

// Geometry includes
#include <geom_point_cloud.h>

// OCCT includes
#include <BRepAdaptor_Surface.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to build a point cloud from a CAD model.
class geom_cloudify : public ActAPI_IAlgorithm
{
public:

  ASitus_EXPORT
    geom_cloudify(const double         uv_step,
                  ActAPI_ProgressEntry progress,
                  ActAPI_PlotterEntry  plotter);

public:

  ASitus_EXPORT bool
    Sample_Faces(const TopoDS_Shape&       model,
                 Handle(geom_point_cloud)& point_cloud);

  ASitus_EXPORT bool
    Sample_Facets(const TopoDS_Shape&       model,
                  Handle(geom_point_cloud)& point_cloud);

protected:

  double chooseParametricStep(const BRepAdaptor_Surface& bas,
                              const bool                 isU,
                              const double               uMin,
                              const double               uMax,
                              const double               vMin,
                              const double               vMax) const;

protected:

  double m_fPrecision; //!< Precision of internal classification.
  double m_fLinStep;   //!< Linear step for point sampling.

};

#endif
