//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_MeshPrs_h
#define asiVisu_MeshPrs_h

// A-Situs includes
#include <asiVisu.h>

// A-Situs (mesh) includes
#include <asiData_TessNode.h>

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

// OCCT includes
#include <Standard_Type.hxx>

// Qt includes
#include <QColor>

DEFINE_STANDARD_HANDLE(asiVisu_MeshPrs, asiVisu_Prs)

//! Presentation class for Mesh Node.
class asiVisu_MeshPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshPrs, asiVisu_Prs)

  // Allows to register this Presentation class in runtime.
  // Notice that the first parameter is NODE TYPE, not the Presentation one
  DEFINE_PRESENTATION_FACTORY(asiData_TessNode, Instance)

public:

  //! Pipelines of Presentation.
  enum PipelineId
  {
    Pipeline_Mesh = 1,
    Pipeline_MeshContour
  };

public:

  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  asiVisu_EXPORT virtual bool
    IsVisible() const;

// Visualization commands:
public:

  asiVisu_EXPORT void doShading() const;
  asiVisu_EXPORT void doWireframe() const;

  asiVisu_EXPORT void doColor(const QColor& theColor) const;
  asiVisu_EXPORT void doUnColor() const;

private:

  //! Allocation is allowed only via Instance method.
  asiVisu_MeshPrs(const Handle(ActAPI_INode)& theNode);

// Callbacks:
private:

  virtual void beforeInitPipelines   ();
  virtual void afterInitPipelines    ();
  virtual void beforeUpdatePipelines ()                                          const;
  virtual void afterUpdatePipelines  ()                                          const;
  virtual void highlight             (vtkRenderer*                 theRenderer,
                                      const asiVisu_PickResult&      thePickRes,
                                      const asiVisu_SelectionNature& theSelNature) const;
  virtual void unHighlight           (vtkRenderer*                 theRenderer,
                                      const asiVisu_SelectionNature& theSelNature) const;
  virtual void renderPipelines       (vtkRenderer*                 theRenderer)  const;
  virtual void deRenderPipelines     (vtkRenderer*                 theRenderer)  const;

};

#endif
