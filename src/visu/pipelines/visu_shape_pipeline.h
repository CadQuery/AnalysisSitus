//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_shape_pipeline_h
#define visu_shape_pipeline_h

// A-Situs includes
#include <visu_data_provider.h>
#include <visu_pipeline.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_shape_pipeline, visu_pipeline)

//! Visualization pipeline for OCCT shapes.
class visu_shape_pipeline : public visu_pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_shape_pipeline, visu_pipeline)

public:

  visu_shape_pipeline(const bool            isOCCTColorScheme = true,
                      const bool            isBound2Node      = true,
                      const bool            isSecondary       = false,
                      const bool            isTrianglesMode   = false,
                      vtkPolyDataAlgorithm* pSource           = NULL);

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& theDataProvider);

public:

  bool IsShadingMode() const;
  void ShadingModeOn();

  bool IsWireframeMode() const;
  void WireframeModeOn();

  void VoidSubShapesOn();
  void VoidSubShapesOff();

  void SharedVerticesOn();
  void SharedVerticesOff();

  vtkPolyDataAlgorithm* DataSource() const;

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  visu_shape_pipeline(const visu_shape_pipeline&);

  //! Assignment prohibited.
  visu_shape_pipeline& operator=(const visu_shape_pipeline&);

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
  bool m_bOCCTColorScheme;

  //! Indicates whether this pipeline must propagate the corresponding Node
  //! ID to its actor via vtkInformation hook.
  bool m_bIsBound2Node;

  //! Indicates whether the pipeline is secondary or not.
  bool m_bIsSecondary;

  //! Technical flag indicating whether a GL-mapper is initialized with the
  //! custom color scheme or not.
  bool m_bMapperColorsSet;

  //! True if SHADING mode if turned ON, false -- otherwise. This flag is
  //! mutually exclusive with m_bWireframeMode.
  bool m_bShadingMode;

  //! True if WIREFRAME mode if turned ON, false -- otherwise. This flag is
  //! mutually exclusive with m_bShadingMode.
  bool m_bWireframeMode;

  //! Indicates whether sub-shapes filter is forced to block data regardless
  //! of the contents of Data Provider.
  bool m_bSubShapesVoid;

  //! Indicates whether to show triangulation.
  bool m_bShowTriangles;

};

#endif
