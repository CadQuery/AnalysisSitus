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

// Active Data includes
#include <ActAPI_IAlgorithm.h>

//-----------------------------------------------------------------------------

//! Utility to invert orientations of faces.
class asiAlgo_InvertFaces : public ActAPI_IAlgorithm
{
public:

  asiAlgo_EXPORT
    asiAlgo_InvertFaces(const Handle(asiAlgo_AAG)& aag,
                        ActAPI_ProgressEntry       progress = NULL,
                        ActAPI_PlotterEntry        plotter = NULL);

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

  asiAlgo_EXPORT void
    buildTopoGraphLevel(const TopoDS_Shape&               root,
                        const TColStd_PackedMapOfInteger& faces2Invert,
                        TopoDS_Shape&                     result) const;

protected:

  Handle(asiAlgo_AAG)  m_aag;      //!< Master AAG.
  TopoDS_Shape         m_result;   //!< Result model.
  ActAPI_ProgressEntry m_progress; //!< Progress entry.
  ActAPI_PlotterEntry  m_plotter;  //!< Plotter entry.

};

#endif
