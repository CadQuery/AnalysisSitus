//-----------------------------------------------------------------------------
// Created on: 02 June 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_Cloudify_h
#define asiAlgo_Cloudify_h

// A-Situs includes
#include <asiAlgo.h>

// Qr includes
#include <QrGeom3D_PositionCloud.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <BRepAdaptor_Surface.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to build a point cloud from a CAD model.
class asiAlgo_Cloudify : public ActAPI_IAlgorithm
{
public:

  asiAlgo_EXPORT
    asiAlgo_Cloudify(const double         uv_step,
                     ActAPI_ProgressEntry progress,
                     ActAPI_PlotterEntry  plotter);

public:

  asiAlgo_EXPORT bool
    Sample_Faces(const TopoDS_Shape& model,
                 QrPtr<pcloud>&      point_cloud);

  asiAlgo_EXPORT bool
    Sample_Facets(const TopoDS_Shape& model,
                  QrPtr<pcloud>&      point_cloud);

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
