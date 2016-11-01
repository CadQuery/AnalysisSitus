//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_REContoursDataProvider_h
#define asiVisu_REContoursDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

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

  asiVisu_REContoursDataProvider();

public:

  virtual ActAPI_DataObjectId GetNodeID() const;

public:

  int
    GetNumOfContours() const;

  TopoDS_Wire
    GetContour(const int oneBased_idx) const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

};

#endif
