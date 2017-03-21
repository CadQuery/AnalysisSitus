//-----------------------------------------------------------------------------
// Created on: 21 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeRobustSource_h
#define asiVisu_ShapeRobustSource_h

// asiVisu includes
#include <asiVisu.h>

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
    asiVisu_ShapeDataSource(const asiVisu_ShapeDataSource&);

  asiVisu_EXPORT asiVisu_ShapeDataSource&
    operator=(const asiVisu_ShapeDataSource&);

protected:

  FaceterType m_faceterType; //!< Type of faceter to use.

};

#endif
