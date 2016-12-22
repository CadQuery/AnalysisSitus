//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_FindSameHosts_h
#define asiAlgo_FindSameHosts_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to find unique host surfaces.
class asiAlgo_FindSameHosts
{
public:

  typedef NCollection_Sequence<TColStd_PackedMapOfInteger> t_groups;

public:

  asiAlgo_EXPORT
    asiAlgo_FindSameHosts(const TopTools_IndexedMapOfShape& faces);

public:

  asiAlgo_EXPORT void
    operator()(t_groups& groups) const;

protected:

  bool findSame(const int                   reference,
                TColStd_PackedMapOfInteger& found) const;

protected:

  const TopTools_IndexedMapOfShape& m_faces;

private:

  asiAlgo_FindSameHosts(const asiAlgo_FindSameHosts& _h) : m_faces(_h.m_faces) {}
  void operator=(const asiAlgo_FindSameHosts&) {}

};

#endif
