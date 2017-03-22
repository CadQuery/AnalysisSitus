//-----------------------------------------------------------------------------
// Created on: 21 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeRobustSource_h
#define asiVisu_ShapeRobustSource_h

// asiVisu includes
#include <asiVisu_ShapeData.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>

// VTK includes
#include <vtkInformationIdTypeKey.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkType.h>
#include <vtkSmartPointer.h>

//! Source of polygonal VTK data representing OCCT shape.
class asiVisu_ShapeRobustSource : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_ShapeRobustSource, vtkPolyDataAlgorithm);

  asiVisu_EXPORT static asiVisu_ShapeRobustSource*
    New();

public:

  asiVisu_EXPORT void
    SetAAG(const Handle(asiAlgo_AAG)& aag);

  asiVisu_EXPORT const Handle(asiAlgo_AAG)&
    GetAAG() const;

protected:

  //! Builds output polygonal data set from the B-Rep shape.
  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        pInfo,
                vtkInformationVector** pInputVector,
                vtkInformationVector*  pOutputVector);

protected:

  asiVisu_EXPORT
    asiVisu_ShapeRobustSource();

  asiVisu_EXPORT
    ~asiVisu_ShapeRobustSource();

private:

  asiVisu_EXPORT
    asiVisu_ShapeRobustSource(const asiVisu_ShapeRobustSource&);

  asiVisu_EXPORT asiVisu_ShapeRobustSource&
    operator=(const asiVisu_ShapeRobustSource&);

protected:

  Handle(asiAlgo_AAG)       m_aag;       //!< AAG of the original B-Rep shape.
  Handle(asiVisu_ShapeData) m_shapeData; //!< Polygonal data container.

};

#endif
