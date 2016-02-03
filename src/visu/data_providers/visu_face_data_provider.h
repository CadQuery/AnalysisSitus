//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_face_data_provider_h
#define visu_face_data_provider_h

// A-Situs visualization includes
#include <visu_data_provider.h>

DEFINE_STANDARD_HANDLE(visu_face_data_provider, visu_data_provider)

//! Data provider for face geometry.
class visu_face_data_provider : public visu_data_provider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_face_data_provider, visu_data_provider)

public:

  visu_face_data_provider(const ActAPI_DataObjectId&           theNodeId,
                          const Handle(ActAPI_HParameterList)& theParamList);

public:

  ActAPI_DataObjectId GetNodeID()    const;
  int                 GetFaceIndex() const;
  TopoDS_Face         ExtractFace()  const;

public:

  Handle(visu_face_data_provider)
    Clone() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  //! Source Node ID.
  ActAPI_DataObjectId m_nodeID;

  //! Source Parameters.
  Handle(ActAPI_HParameterList) m_params;

};

#endif
