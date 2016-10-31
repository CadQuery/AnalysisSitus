//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_pdomain_source_h
#define visu_pdomain_source_h

// OCCT includes
#include <TopoDS_Face.hxx>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! Source of polygonal data representing face domain.
class visu_pdomain_source : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_pdomain_source, vtkPolyDataAlgorithm);
  static visu_pdomain_source* New();

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

  visu_pdomain_source();
  ~visu_pdomain_source();

private:

  visu_pdomain_source(const visu_pdomain_source&);
  visu_pdomain_source& operator=(const visu_pdomain_source&);

private:

  TopoDS_Face m_face; //!< Face to convert t.

};

#endif
