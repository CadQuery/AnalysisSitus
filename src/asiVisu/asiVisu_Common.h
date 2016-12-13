//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_Common_h
#define asiVisu_Common_h

// Visualization includes
#include <asiVisu.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// OCCT includes
#include <NCollection_SparseArray.hxx>

//-----------------------------------------------------------------------------

//! Elementary data chunk representing vector value.
struct asiVisu_VectorTuple
{
  double F[3];

  asiVisu_VectorTuple()
  {
    F[0] = 0.0; F[1] = 0.0; F[2] = 0.0;
  }

  asiVisu_VectorTuple(const double _F1,
                      const double _F2,
                      const double _F3)
  {
    F[0] = _F1; F[1] = _F2; F[2] = _F3;
  }
};

//! Collection of vector values associated with sparse integer IDs.
typedef NCollection_SparseArray<asiVisu_VectorTuple> asiVisu_VectorMap;
typedef NCollection_Shared<asiVisu_VectorMap>        asiVisu_HVectorMap;

//-----------------------------------------------------------------------------

#endif
