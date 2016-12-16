//-----------------------------------------------------------------------------
// Created on: 29 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_DetachFaces_h
#define asiAlgo_DetachFaces_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
//#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to detach faces.
class asiAlgo_DetachFaces
{
public:

  asiAlgo_EXPORT
    asiAlgo_DetachFaces(const TopoDS_Shape& masterCAD);

public:

  asiAlgo_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& faces);

public:

  //! \return result shape.
  const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  TopoDS_Shape                              m_master; //!< Master model.
  TopoDS_Shape                              m_result; //!< Result.
  //TopTools_IndexedDataMapOfShapeListOfShape m_M;      //!< Sub-shapes with ancestors.

};

#endif
