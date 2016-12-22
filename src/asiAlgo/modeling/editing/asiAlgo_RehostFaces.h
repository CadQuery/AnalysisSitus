//-----------------------------------------------------------------------------
// Created on: 28 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_RehostFaces_h
#define asiAlgo_RehostFaces_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to re-host faces so that they share the same underlying surface.
class asiAlgo_RehostFaces
{
public:

  asiAlgo_EXPORT
    asiAlgo_RehostFaces(const TopoDS_Shape& masterCAD);

public:

  asiAlgo_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& faces);

public:

  //! \return result.
  const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  void moveWires(const TopoDS_Face& source,
                 TopoDS_Face&       target) const;

protected:

  TopoDS_Shape m_master; //!< Master CAD.
  TopoDS_Shape m_result; //!< Result.

};

#endif
