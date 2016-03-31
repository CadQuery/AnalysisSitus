//-----------------------------------------------------------------------------
// Created on: 25 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef xde_shape_id_h
#define xde_shape_id_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

DEFINE_STANDARD_HANDLE(xde_shape_id, Standard_Transient)

//! Shape ID in XDE model.
class xde_shape_id : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(xde_shape_id, Standard_Transient)

public:

  inline void Prepend(const TCollection_AsciiString& key)
  {
    m_top2Bottom.Prepend(key);
    //
    m_debug = key + "." + m_debug;
  }

  inline void Add(const TCollection_AsciiString& key)
  {
    m_top2Bottom.Append(key);
    //
    m_debug += ".";
    m_debug += key;
  }

  inline void Add(const Handle(xde_shape_id)& id)
  {
    for ( int i = 1; i <= id->m_top2Bottom.Length(); ++i )
    {
      this->m_top2Bottom.Append( id->m_top2Bottom(i) );
      //
      m_debug += ".";
      m_debug +=  id->m_top2Bottom(i);
    }
  }

  inline const NCollection_Sequence<TCollection_AsciiString>& Top2Bottom() const
  {
    return m_top2Bottom;
  }

  inline const TCollection_AsciiString& Top2Bottom(const int idx) const
  {
    return m_top2Bottom(idx);
  }

  inline bool Contains(const Handle(xde_shape_id)& key)
  {
    bool isOk = true;
    for ( int i = 1; i <= key->m_top2Bottom.Length(); ++i )
    {
      if ( !this->m_top2Bottom(i).IsEqual( key->m_top2Bottom(i) ) )
      {
        isOk = false;
        break;
      }
    }
    return isOk;
  }

private:

  NCollection_Sequence<TCollection_AsciiString> m_top2Bottom;
  TCollection_AsciiString                       m_debug;

};

#endif
