//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_ubend_prs_h
#define visu_ubend_prs_h

// A-Situs (visualization) includes
#include <visu_law_pipeline.h>
#include <visu_prs.h>
#include <visu_utils.h>

// A-Situs (geometry) includes
#include <geom_ubend_node.h>

DEFINE_STANDARD_HANDLE(visu_ubend_prs, visu_prs)

//! Presentation class for u-bend modeling.
class visu_ubend_prs : public visu_prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_ubend_prs, visu_prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(geom_ubend_node, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Undefined = 0,
    //
    Pipeline_Path = 1,
    Pipeline_Sections,
    Pipeline_Skin,
    //
    Pipeline_Law1,
    Pipeline_Law2,
    Pipeline_Law3,
    Pipeline_Law4,
    Pipeline_Law5,
    Pipeline_Law6,
    Pipeline_Law7,
    Pipeline_Law8,
    Pipeline_Law9,
    Pipeline_Law10,
    Pipeline_Law11,
    Pipeline_Law12
  };

public:

  static Handle(visu_prs)
    Instance(const Handle(ActAPI_INode)& theNode);

  virtual bool
    IsVisible() const;

protected:

  //! Returns law pipeline by the law index.
  //! \param oneBased_index [in] index of the law == 1-based index of a pole.
  //! \return law pipeline.
  inline Handle(visu_law_pipeline) LawPipeline(const int oneBased_index)
  {
    Handle(visu_law_pipeline) result;
    PipelineId id = Pipeline_Undefined;
    //
    switch ( oneBased_index )
    {
      case 1:  id = Pipeline_Law1;  break;
      case 2:  id = Pipeline_Law2;  break;
      case 3:  id = Pipeline_Law3;  break;
      case 4:  id = Pipeline_Law4;  break;
      case 5:  id = Pipeline_Law5;  break;
      case 6:  id = Pipeline_Law6;  break;
      case 7:  id = Pipeline_Law7;  break;
      case 8:  id = Pipeline_Law8;  break;
      case 9:  id = Pipeline_Law9;  break;
      case 10: id = Pipeline_Law10; break;
      case 11: id = Pipeline_Law11; break;
      case 12: id = Pipeline_Law12; break;
      default: break;
    }

    if ( id == Pipeline_Undefined )
      return NULL;
    //
    return Handle(visu_law_pipeline)::DownCast( this->GetPipeline(id) );
  }

private:

  //! Allocation is allowed only via Instance method.
  visu_ubend_prs(const Handle(ActAPI_INode)& theNode);

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

};

#endif
