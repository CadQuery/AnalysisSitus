//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_find_same_hosts_h
#define geom_find_same_hosts_h

// A-Situs includes
#include <asitusGeom.h>

// OCCT includes
#include <NCollection_Sequence.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to find unique host surfaces.
class geom_find_same_hosts
{
public:

  typedef NCollection_Sequence<TColStd_PackedMapOfInteger> t_groups;

public:

  ASitus_EXPORT
    geom_find_same_hosts(const TopTools_IndexedMapOfShape& faces);

public:

  ASitus_EXPORT void
    operator()(t_groups& groups) const;

protected:

  bool findSame(const int                   reference,
                TColStd_PackedMapOfInteger& found) const;

protected:

  const TopTools_IndexedMapOfShape& m_faces;

private:

  geom_find_same_hosts(const geom_find_same_hosts& _h) : m_faces(_h.m_faces) {}
  void operator=(const geom_find_same_hosts&) {}

};

#endif
