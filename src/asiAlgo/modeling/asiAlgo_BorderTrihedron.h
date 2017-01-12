//-----------------------------------------------------------------------------
// Created on: 11 January 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_BorderTrihedron_h
#define asiAlgo_BorderTrihedron_h

// OCCT includes
#include <gp_Dir.hxx>

//-----------------------------------------------------------------------------

//! Border trihedron associated with a sheet face at connection zones.
class asiAlgo_BorderTrihedron
{
public:

  //! Default ctor.
  asiAlgo_BorderTrihedron() {}

  //! Creates border trihedron.
  //! \param[in] _V_origin local origin of the trihedron.
  //! \param[in] _V_x      local OX axis of the trihedron.
  //! \param[in] _V_y      local OY axis of the trihedron.
  //! \param[in] _V_z      local OZ axis of the trihedron.
  asiAlgo_BorderTrihedron(const gp_Pnt& _V_origin,
                          const gp_Dir& _V_x,
                          const gp_Dir& _V_y,
                          const gp_Dir& _V_z)
  {
    V_origin = _V_origin;
    V_x      = _V_x;
    V_y      = _V_y;
    V_z      = _V_z;
  }

public:

  gp_Pnt V_origin;
  gp_Dir V_x;
  gp_Dir V_y;
  gp_Dir V_z;

};

#endif
