//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_project_wire_shell_h
#define geom_project_wire_shell_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>

//! Wire-to-Shell projection tool.
class geom_project_wire_shell
{
public:

  ASitus_EXPORT
    geom_project_wire_shell(const TopoDS_Shell& Shell,
                            const double        inaccuracy);

public:

  ASitus_EXPORT void
    operator()(const TopoDS_Wire&   Wire,
               TopoDS_Shape&        Result,
               ActAPI_ProgressEntry Journal);

private:

  TopoDS_Shell m_shell;       //!< Working shell.
  double       m_fInaccuracy; //!< Inaccuracy.

};

#endif
