//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_IVPrs_h
#define asiVisu_IVPrs_h

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

DEFINE_STANDARD_HANDLE(asiVisu_IVPrs, asiVisu_Prs)

//! Base presentation class for IV.
class asiVisu_IVPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVPrs, asiVisu_Prs)

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
                         const asiUI_PickResult& thePickRes,
                         const asiUI_SelectionNature& theSelNature) const;
  virtual void unHighlight(vtkRenderer* theRenderer,
                           const asiUI_SelectionNature& theSelNature) const;
  virtual void renderPipelines(vtkRenderer* theRenderer) const;
  virtual void deRenderPipelines(vtkRenderer* theRenderer) const;

protected:

  asiVisu_IVPrs(const Handle(ActAPI_INode)& node) : asiVisu_Prs(node) {}

};

#endif
