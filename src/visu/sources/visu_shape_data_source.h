//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_shape_data_source_h
#define visu_shape_data_source_h

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
class visu_shape_data_source : public IVtkTools_ShapeDataSource
{
public:

  vtkTypeMacro(visu_shape_data_source, IVtkTools_ShapeDataSource);
  static visu_shape_data_source* New();

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

  visu_shape_data_source();
  ~visu_shape_data_source();

private:

  visu_shape_data_source (const visu_shape_data_source&);
  visu_shape_data_source& operator= (const visu_shape_data_source&);

};

#endif
