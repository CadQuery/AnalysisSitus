//-----------------------------------------------------------------------------
// Created on: 30 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PartDataProvider_h
#define asiVisu_PartDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiData includes
#include <asiData_PartNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_PartDataProvider, asiVisu_DataProvider)

//! Data provider from Part Node to Presentation.
class asiVisu_PartDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_PartDataProvider(const Handle(asiData_PartNode)& partNode);

protected:

  asiVisu_EXPORT
    asiVisu_PartDataProvider();

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

  asiVisu_EXPORT virtual Handle(asiData_PartNode)
    GetPartNode() const;

  asiVisu_EXPORT virtual TopoDS_Shape
    GetShape() const;

  asiVisu_EXPORT virtual Handle(asiAlgo_AAG)
    GetAAG() const;

public:

  asiVisu_EXPORT Handle(asiVisu_PartDataProvider)
    Clone() const;

protected:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  //! Source Node.
  Handle(asiData_PartNode) m_node;

};

#endif
