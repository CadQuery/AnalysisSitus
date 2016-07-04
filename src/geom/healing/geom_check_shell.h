//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_check_shell_h
#define geom_check_shell_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Shell.hxx>

//! Shell checking tool.
class geom_check_shell
{
public:

  ASitus_EXPORT
    geom_check_shell(const TopoDS_Shell& Shell);

public:

  ASitus_EXPORT bool
    Check_isAccurate(const double         inaccuracy,
                     ActAPI_ProgressEntry Journal) const;

  ASitus_EXPORT bool
    Check_isConnected(ActAPI_ProgressEntry Journal) const;

  ASitus_EXPORT bool
    Check_everything(const double         inaccuracy,
                     ActAPI_ProgressEntry Journal) const;

protected:

  TopoDS_Shell m_S; //!< Shell to check.

};

#endif
