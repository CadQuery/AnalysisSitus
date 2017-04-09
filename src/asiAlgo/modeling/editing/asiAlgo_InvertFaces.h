//-----------------------------------------------------------------------------
// Created on: 10 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_InvertFaces_h
#define asiAlgo_InvertFaces_h

// asiAlgo includes
#include <asiAlgo_AAG.h>

//-----------------------------------------------------------------------------

//! Utility to invert orientations of faces.
class asiAlgo_InvertFaces
{
public:

  asiAlgo_EXPORT
    asiAlgo_InvertFaces(const Handle(asiAlgo_AAG)& aag);

public:

  asiAlgo_EXPORT bool
    Perform(const TColStd_PackedMapOfInteger& faceIds);

public:

  //! \return result shape.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

protected:

  Handle(asiAlgo_AAG) m_aag;    //!< Master AAG.
  TopoDS_Shape        m_result; //!< Result model.

};

#endif
