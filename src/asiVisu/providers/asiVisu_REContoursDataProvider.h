//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_REContoursDataProvider_h
#define asiVisu_REContoursDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

// asiData includes
#include <asiData_REContoursNode.h>

// OCCT includes
#include <TopoDS_Wire.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_REContoursDataProvider, asiVisu_DataProvider)

//! Data provider for re-engineering contours.
class asiVisu_REContoursDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_REContoursDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_REContoursDataProvider(const Handle(asiData_REContoursNode)& contours_n);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  asiVisu_EXPORT int
    GetNumOfContours() const;

  asiVisu_EXPORT TopoDS_Wire
    GetContour(const int oneBased_idx) const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_REContoursNode) m_contours; //!< Contours Node.

};

#endif
