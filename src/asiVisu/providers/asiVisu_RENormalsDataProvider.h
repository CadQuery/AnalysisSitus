//-----------------------------------------------------------------------------
// Created on: 08 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_RENormalsDataProvider_h
#define asiVisu_RENormalsDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

// asiData includes
#include <asiData_RENormalsNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_RENormalsDataProvider, asiVisu_DataProvider)

//! Data provider for re-engineering normals.
class asiVisu_RENormalsDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RENormalsDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_RENormalsDataProvider(const Handle(asiData_RENormalsNode)& normals_n);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_PointCloud<float>)
    GetNormals() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_RENormalsNode) m_normals; //!< Normals Data Node.

};

#endif
