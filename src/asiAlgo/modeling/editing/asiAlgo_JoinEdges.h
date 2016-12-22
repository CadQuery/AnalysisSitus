//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_JoinEdges_h
#define asiAlgo_JoinEdges_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to join edge chains.
class asiAlgo_JoinEdges
{
public:

  asiAlgo_EXPORT
    asiAlgo_JoinEdges(const TopoDS_Shape& masterCAD);

public:

  asiAlgo_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& edges,
            const TopoDS_Face&                face);

public:

  //! \return result shape.
  const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  void chooseOrder(const TopTools_IndexedMapOfShape& edges,
                   TopoDS_Edge&                      eFirst,
                   TopoDS_Edge&                      eSecond) const;

  bool joinEdges(const TopoDS_Edge& eFirst,
                 const TopoDS_Edge& eSecond,
                 const TopoDS_Face& face,
                 TopoDS_Edge&       eResult) const;

protected:

  TopoDS_Shape m_master; //!< Master model.
  TopoDS_Shape m_result; //!< Result.

};

#endif
