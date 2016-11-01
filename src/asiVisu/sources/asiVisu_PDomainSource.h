//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_PDomainSource_h
#define asiVisu_PDomainSource_h

// OCCT includes
#include <TopoDS_Face.hxx>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! Source of polygonal data representing face domain.
class asiVisu_PDomainSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_PDomainSource, vtkPolyDataAlgorithm);
  static asiVisu_PDomainSource* New();

// Kernel methods:
public:

  void SetFace(const TopoDS_Face& face);

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  double computeTipSize() const;

protected:

  asiVisu_PDomainSource();
  ~asiVisu_PDomainSource();

private:

  asiVisu_PDomainSource(const asiVisu_PDomainSource&);
  asiVisu_PDomainSource& operator=(const asiVisu_PDomainSource&);

private:

  TopoDS_Face m_face; //!< Face to convert t.

};

#endif
