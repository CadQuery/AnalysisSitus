//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeDataSource_h
#define asiVisu_ShapeDataSource_h

// asiVisu includes
#include <asiVisu.h>

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

  //! Type of faceter.
  enum FaceterType
  {
    Faceter_Standard = 1,
    Faceter_Robust
  };

public:

  vtkTypeMacro(asiVisu_ShapeDataSource, IVtkTools_ShapeDataSource);

  asiVisu_EXPORT static asiVisu_ShapeDataSource*
    New();

public:

  void SetFaceter(const FaceterType faceter) { m_faceterType = faceter; }

  FaceterType GetFaceter() const { return m_faceterType; }

protected: //! @name Interface to override

  //! This is called by the superclass.
  //! This is the method you should override if you use this class as ancestor.
  //! Build output polygonal data set from the shape wrapper.
  //! @param theRequest [in] information about data object.
  //! In current implementation it is ignored.
  //! @param theInputVector [in] the input data. As a data source is the start
  //! stage of the VTK pipeline, theInputVector is empty and not used (no input port).
  //! @param theOutputVector [in] the pointer to output data, that is filled in this method.
  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        theRequest,
                vtkInformationVector** theInputVector,
                vtkInformationVector*  theOutputVector);

protected:

  asiVisu_EXPORT
    asiVisu_ShapeDataSource();

  asiVisu_EXPORT
    ~asiVisu_ShapeDataSource();

private:

  asiVisu_EXPORT
    asiVisu_ShapeDataSource(const asiVisu_ShapeDataSource&);

  asiVisu_EXPORT asiVisu_ShapeDataSource&
    operator=(const asiVisu_ShapeDataSource&);

protected:

  FaceterType m_faceterType; //!< Type of faceter to use.

};

#endif
