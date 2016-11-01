//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeDataSource_h
#define asiVisu_ShapeDataSource_h

// OCCT includes
#include <IVtkTools_ShapeDataSource.hxx>
#include <IVtkOCC_Shape.hxx>
#include <IVtkVTK_ShapeData.hxx>

// VTK includes
#include <vtkInformationIdTypeKey.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkType.h>
#include <vtkSmartPointer.h>

//! \todo This class is a blueprint of IVtkTools_ShapeDataSource shipped
//!       with OCCT. We have injected this source here in order to have
//!       ability to play with meshing parameters. Later on it can be pushed
//!       back to the original IVtk component of OCCT.
class asiVisu_ShapeDataSource : public IVtkTools_ShapeDataSource
{
public:

  vtkTypeMacro(asiVisu_ShapeDataSource, IVtkTools_ShapeDataSource);
  static asiVisu_ShapeDataSource* New();

protected: //! @name Interface to override

  //! This is called by the superclass.
  //! This is the method you should override if you use this class as ancestor.
  //! Build output polygonal data set from the shape wrapper.
  //! @param theRequest [in] information about data object.
  //! In current implementation it is ignored.
  //! @param theInputVector [in] the input data. As adata source is the start
  //! stage of the VTK pipeline, theInputVector is empty and not used (no input port).
  //! @param theOutputVector [in] the pointer to output data, that is filled in this method.
  virtual int RequestData(vtkInformation* theRequest,
                          vtkInformationVector** theInputVector,
                          vtkInformationVector* theOutputVector);

protected:

  asiVisu_ShapeDataSource();
  ~asiVisu_ShapeDataSource();

private:

  asiVisu_ShapeDataSource (const asiVisu_ShapeDataSource&);
  asiVisu_ShapeDataSource& operator= (const asiVisu_ShapeDataSource&);

};

#endif
