//-----------------------------------------------------------------------------
// Created on: 13 January 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_RENormalsPipeline_h
#define asiVisu_RENormalsPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_RENormalsPipeline, asiVisu_Pipeline)

//! Visualization pipeline for normal vectors in reverse engineering context.
class asiVisu_RENormalsPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RENormalsPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_RENormalsPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_RENormalsPipeline(const asiVisu_RENormalsPipeline&);

  //! Assignment prohibited.
  asiVisu_RENormalsPipeline& operator=(const asiVisu_RENormalsPipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_Vector = 1, //!< Filter for populating cell vector array.
    Filter_Trsf,       //!< Transformation filter to prepare for glyphs.
    Filter_Glyph3D,    //!< Filter for generating glyphs.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

};

#endif
