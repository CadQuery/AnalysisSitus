//-----------------------------------------------------------------------------
// Created on: 08 March 2017
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoVisu_LimbPrs_h
#define ottoVisu_LimbPrs_h

// ottoVisu includes
#include <ottoVisu.h>

// ottoData includes
#include <ottoData_LimbNode.h>

// asiVisu includes
#include <asiVisu_DefaultPrs.h>
#include <asiVisu_Utils.h>

// Qt includes
#include <QColor>

// VTK includes
#include <vtkActor.h>
#include <vtkCellPicker.h>

//! Presentation class for Limb Node.
class ottoVisu_LimbPrs : public asiVisu_DefaultPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(ottoVisu_LimbPrs, asiVisu_Prs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(ottoData_LimbNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Triangulation = 1,  //!< Shaded faces.
    Pipeline_TriangulationLinks, //!< Links.
    Pipeline_OBB,                //!< Oriented bounding box.
    Pipeline_Sections,           //!< Sections.
    Pipeline_Surface             //!< Surface pipeline.
  };

public:

  ottoVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& N);

public:

  bool IsVisible() const { return true; }

private:

  //! Allocation is allowed only via Instance method.
  ottoVisu_LimbPrs(const Handle(ActAPI_INode)& N);

};

#endif
