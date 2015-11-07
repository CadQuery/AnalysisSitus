//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su
//-----------------------------------------------------------------------------

#ifndef visu_shape_pipeline_h
#define visu_shape_pipeline_h

// A-Situs includes
#include <visu_pipeline.h>
#include <SbmVisu_BasePipelineDataProvider.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------
// Data Provider for Shape VTK Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(SbmVisu_ShapeDataProvider, SbmVisu_BasePipelineDataProvider)

//! \ingroup VISU
//!
//! Data source for Shape Pipeline. Specifies all data necessary for
//! visualization of OCCT topological shapes allowing framework users define
//! the way of how Nodal Parameters are converted to the required data by
//! implementing the pure virtual methods of this class.
class SbmVisu_ShapeDataProvider : public SbmVisu_BasePipelineDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(SbmVisu_ShapeDataProvider, SbmVisu_BasePipelineDataProvider)

public:

  virtual TopoDS_Shape
    GetShape() const = 0;

  virtual Handle(TColStd_HPackedMapOfInteger)
    GetSubShapes() const = 0;

  virtual Standard_Boolean
    HasPosition() const = 0;

  virtual Standard_Boolean
    HasPositionAndRotation() const = 0;

  virtual void
    GetPosition(Standard_Real& thePosX,
                Standard_Real& thePosY,
                Standard_Real& thePosZ) const = 0;

  virtual void
    GetRotation(Standard_Real& theAngleAroundX,
                Standard_Real& theAngleAroundY,
                Standard_Real& theAngleAroundZ) const = 0;
};

//-----------------------------------------------------------------------------
// Shape VTK Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(SbmVisu_ShapePipeline, SbmVisu_BasePipeline)

//! \ingroup VISU
//!
//! Visualization pipeline for OCCT topological shapes.
class SbmVisu_ShapePipeline : public SbmVisu_BasePipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(SbmVisu_ShapePipeline, SbmVisu_BasePipeline)

public:

  SbmVisu_EXPORT
    SbmVisu_ShapePipeline(const Standard_Boolean isOCCTColorScheme = Standard_True,
                          const Standard_Boolean isBound2Node = Standard_True,
                          const Standard_Boolean isSecondary = Standard_False);

public:

  SbmVisu_EXPORT virtual void
    SetInput(const Handle(SbmAPI_IPipelineDataProvider)& theDataProvider);

public:

  SbmVisu_EXPORT Standard_Boolean IsShadingMode() const;
  SbmVisu_EXPORT void ShadingModeOn();

  SbmVisu_EXPORT Standard_Boolean IsWireframeMode() const;
  SbmVisu_EXPORT void WireframeModeOn();

  SbmVisu_EXPORT void VoidSubShapesOn();
  SbmVisu_EXPORT void VoidSubShapesOff();

  SbmVisu_EXPORT void SharedVerticesOn();
  SbmVisu_EXPORT void SharedVerticesOff();

  SbmVisu_EXPORT vtkPolyDataAlgorithm* DataSource() const;

private:

  virtual void addToRendererCallback(vtkRenderer* theRenderer);
  virtual void removeFromRendererCallback(vtkRenderer* theRenderer);
  virtual void updateCallback();

private:

  //! Copying prohibited.
  SbmVisu_ShapePipeline(const SbmVisu_ShapePipeline&);

  //! Assignment prohibited.
  SbmVisu_ShapePipeline& operator=(const SbmVisu_ShapePipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_Source = 1, //!< Shape Data Source.
    Filter_SubShapes,  //!< Sub-shapes filter.
    Filter_DM,         //!< Display Mode filter.
    Filter_Normals     //!< Filter to generate normals for smooth shading.
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

  //! Indicates whether the standard OCCT color mapping is used as a default
  //! color scheme for the visualized shapes.
  Standard_Boolean m_bOCCTColorScheme;

  //! Indicates whether this pipeline must propagate the corresponding Node
  //! ID to its actor via vtkInformation hook.
  Standard_Boolean m_bIsBound2Node;

  //! Indicates whether the pipeline is secondary or not.
  Standard_Boolean m_bIsSecondary;

  //! Technical flag indicating whether a GL-mapper is initialized with the
  //! custom color scheme or not.
  Standard_Boolean m_bMapperColorsSet;

  //! True if SHADING mode if turned ON, false -- otherwise. This flag is
  //! exclusive with m_bWireframeMode.
  Standard_Boolean m_bShadingMode;

  //! True if WIREFRAME mode if turned ON, false -- otherwise. This flag is
  //! exclusive with m_bShadingMode.
  Standard_Boolean m_bWireframeMode;

  //! Indicates whether sub-shapes filter is forced to block data regardless
  //! of the contents of Data Provider.
  Standard_Boolean m_bSubShapesVoid;

};

#endif
