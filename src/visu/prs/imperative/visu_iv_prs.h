//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_iv_prs_h
#define visu_iv_prs_h

// A-Situs (visualization) includes
#include <visu_prs.h>
#include <visu_utils.h>

DEFINE_STANDARD_HANDLE(visu_iv_prs, visu_prs)

//! Base presentation class for IV.
class visu_iv_prs : public visu_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_iv_prs, visu_prs)

public:

  inline virtual bool IsVisible() const
  {
    return true;
  }

// Callbacks:
private:

  virtual void beforeInitPipelines();
  virtual void afterInitPipelines();
  virtual void beforeUpdatePipelines() const;
  virtual void afterUpdatePipelines() const;
  virtual void highlight(vtkRenderer* theRenderer,
                         const visu_pick_result& thePickRes,
                         const visu_selection_nature& theSelNature) const;
  virtual void unHighlight(vtkRenderer* theRenderer,
                           const visu_selection_nature& theSelNature) const;
  virtual void renderPipelines(vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines(vtkRenderer* theRenderer) const;

protected:

  visu_iv_prs(const Handle(ActAPI_INode)& node) : visu_prs(node) {}

};

#endif
