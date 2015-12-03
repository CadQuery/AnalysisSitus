//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_curve_source_h
#define visu_curve_source_h

// Visualization includes
#include <visu_utils.h>

// Active Data (auxiliary) includes
#include <ActAux_Common.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Sequence.hxx>

//! Source of polygonal data representing a 3D curve.
class visu_curve_source : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_curve_source, vtkPolyDataAlgorithm);
  ASitus_EXPORT static visu_curve_source* New();

// Kernel methods:
public:

  ASitus_EXPORT void
    SetInputArrays(const Handle(HRealArray)& theXCoords,
                   const Handle(HRealArray)& theYCoords,
                   const Handle(HRealArray)& theZCoords,
                   const visu_orientation    theOri = VisuOri_Undefined);

public:

  inline void   SetTipSize    (const double size)       { m_fOriTipSize = size; }
  inline double GetTipSize    ()                  const { return m_fOriTipSize; }

  inline void   SetTipTangent (const gp_Vec& vec)       { m_oriT = vec; }
  inline gp_Vec GetTipTangent ()                  const { return m_oriT; }

  inline void   SetTipNorm    (const gp_Vec& vec)       { m_oriN = vec; }
  inline gp_Vec GetTipNorm    ()                  const { return m_oriN; }

protected:

  virtual int RequestData(vtkInformation*        theRequest,
                          vtkInformationVector** theInputVector,
                          vtkInformationVector*  theOutputVector);

protected:

  vtkIdType
    registerGridPoint(const gp_Pnt& thePoint,
                      vtkPolyData*  thePolyData);

  vtkIdType
    registerGridPoint(const int    thePairIndex,
                      vtkPolyData* thePolyData);

  vtkIdType
    registerLine(const gp_Pnt& thePointStart,
                 const gp_Pnt& thePointEnd,
                 vtkPolyData*  thePolyData);

  vtkIdType
    registerLine(const int    thePairIndex,
                 vtkPolyData* thePolyData);

  vtkIdType
    registerVertex(const int    thePairIndex,
                   vtkPolyData* thePolyData);

protected:

  visu_curve_source();
  ~visu_curve_source();

private:

  visu_curve_source(const visu_curve_source&);
  visu_curve_source& operator=(const visu_curve_source&);

private:

  Handle(HRealArray) m_XCoords; //!< X coordinates.
  Handle(HRealArray) m_YCoords; //!< Y coordinates.
  Handle(HRealArray) m_ZCoords; //!< Z coordinates.

// Orientation marker:
private:

  visu_orientation m_ori;         //!< Curve orientation.
  double           m_fOriTipSize; //!< Size of orientation tip (calculated externally).
  gp_Vec           m_oriT;        //!< Orientation vector at the end point.
  gp_Vec           m_oriN;        //!< Normal to the curve at the end point.

};

#endif
