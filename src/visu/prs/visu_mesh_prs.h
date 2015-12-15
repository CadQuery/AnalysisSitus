//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_mesh_prs_h
#define visu_mesh_prs_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (mesh) includes
#include <mesh_node.h>

// A-Situs (visualization) includes
#include <visu_prs.h>
#include <visu_utils.h>

// OCCT includes
#include <Standard_Type.hxx>

// Qt includes
#include <QColor>

DEFINE_STANDARD_HANDLE(visu_mesh_prs, visu_prs)

//! Presentation class for Mesh Node.
class visu_mesh_prs : public visu_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(visu_mesh_prs, visu_prs)

  // Allows to register this Presentation class in runtime.
  // Notice that the first parameter is NODE TYPE, not the Presentation one
  DEFINE_PRESENTATION_FACTORY(mesh_node, Instance)

public:

  //! Pipelines of Presentation.
  enum PipelineId
  {
    Pipeline_Mesh = 1,
    Pipeline_MeshContour
  };

public:

  static Handle(visu_prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  virtual bool
    IsVisible() const;

// Visualization commands:
public:

  void doShading() const;
  void doWireframe() const;

  void doColor(const QColor& theColor) const;
  void doUnColor() const;

private:

  //! Allocation is allowed only via Instance method.
  visu_mesh_prs(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void beforeInitPipelines   ();
  virtual void afterInitPipelines    ();
  virtual void beforeUpdatePipelines ()                                          const;
  virtual void afterUpdatePipelines  ()                                          const;
  virtual void highlight             (vtkRenderer*                 theRenderer,
                                      const visu_pick_result&      thePickRes,
                                      const visu_selection_nature& theSelNature) const;
  virtual void unHighlight           (vtkRenderer*                 theRenderer,
                                      const visu_selection_nature& theSelNature) const;
  virtual void renderPipelines       (vtkRenderer*                 theRenderer)  const;
  virtual void deRenderPipelines     (vtkRenderer*                 theRenderer)  const;

};

#endif
