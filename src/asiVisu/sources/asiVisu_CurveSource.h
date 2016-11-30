//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_CurveSource_h
#define asiVisu_CurveSource_h

// Visualization includes
#include <asiVisu_Utils.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Edge.hxx>

//! Source of polygonal data representing a 3D curve.
class asiVisu_CurveSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_CurveSource, vtkPolyDataAlgorithm);
  static asiVisu_CurveSource* New();

// Kernel methods:
public:

  bool
    SetInputEdge(const TopoDS_Edge& edge);

  bool
    SetInputCurve(const Handle(Geom_Curve)& curve,
                  const double              first,
                  const double              last,
                  const asiVisu_Orientation ori = VisuOri_Internal);

  bool
    SetInputCurve2d(const Handle(Geom2d_Curve)& curve,
                    const double                first,
                    const double                last,
                    const asiVisu_Orientation   ori = VisuOri_Internal);

  void
    SetInputArrays(const Handle(HRealArray)& xCoords,
                   const Handle(HRealArray)& yCoords,
                   const Handle(HRealArray)& zCoords,
                   const asiVisu_Orientation ori = VisuOri_Undefined);

  void
    GetInputArrays(Handle(HRealArray)&  xCoords,
                   Handle(HRealArray)&  yCoords,
                   Handle(HRealArray)&  zCoords,
                   asiVisu_Orientation& ori) const;

public:

  void          SetTipSize    (const double size)       { m_fOriTipSize = size; }
  double        GetTipSize    ()                  const { return m_fOriTipSize; }

  void          SetTipTangent (const gp_Vec& vec)       { m_oriT = vec; }
  const gp_Vec& GetTipTangent ()                  const { return m_oriT; }

  void          SetTipNorm    (const gp_Vec& vec)       { m_oriN = vec; }
  const gp_Vec& GetTipNorm    ()                  const { return m_oriN; }

  void          SetPedigreeId (const int pid)           { m_iPedigreeId = pid; }
  int           GetPedigreeId ()                  const { return m_iPedigreeId; }

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  vtkIdType
    registerGridPoint(const gp_Pnt& point,
                      vtkPolyData*  polyData);

  vtkIdType
    registerGridPoint(const int    index,
                      vtkPolyData* polyData);

  vtkIdType
    registerLine(const gp_Pnt& pointStart,
                 const gp_Pnt& pointEnd,
                 vtkPolyData*  polyData);

  vtkIdType
    registerLine(const int    index,
                 vtkPolyData* polyData);

  vtkIdType
    registerVertex(const int    index,
                   vtkPolyData* polyData);

protected:

  asiVisu_CurveSource();
  ~asiVisu_CurveSource();

private:

  asiVisu_CurveSource(const asiVisu_CurveSource&);
  asiVisu_CurveSource& operator=(const asiVisu_CurveSource&);

private:

  Handle(HRealArray) m_XCoords;     //!< X coordinates.
  Handle(HRealArray) m_YCoords;     //!< Y coordinates.
  Handle(HRealArray) m_ZCoords;     //!< Z coordinates.
  int                m_iPedigreeId; //!< Pedigree ID.

// Orientation marker:
private:

  asiVisu_Orientation m_ori;         //!< Curve orientation.
  double              m_fOriTipSize; //!< Size of orientation tip (calculated externally).
  gp_Vec              m_oriT;        //!< Orientation vector at the end point.
  gp_Vec              m_oriN;        //!< Normal to the curve at the end point.

};

#endif
