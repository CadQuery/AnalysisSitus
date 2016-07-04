//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_check_shell.h>

// OCCT includes
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_Status.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>

#undef DRAW_DEBUG
#if defined DRAW_DEBUG
  #include <common_draw_test_suite.h>
  #pragma message("===== warning: DRAW_DEBUG is enabled")
  #pragma comment(lib, "TKDraw.lib")
#endif

//! Constructor.
//! \param Shell [in] shell to check.
geom_check_shell::geom_check_shell(const TopoDS_Shell& Shell) : m_S(Shell)
{}

//! Checks whether the working shell is accurate enough regarding the
//! passed tolerance.
//! \param inaccuracy [in]     characteristic inaccuracy.
//! \param Journal    [in/out] journal instance.
//! \return true/false.
bool geom_check_shell::Check_isAccurate(const double         inaccuracy,
                                        ActAPI_ProgressEntry Journal) const
{
  // Get max tolerance
  ShapeAnalysis_ShapeTolerance TolAnalysis;
  TolAnalysis.InitTolerance();
  TolAnalysis.AddTolerance(m_S);
  //
  const double maxTol = TolAnalysis.GlobalTolerance(1);

  // If requested precision is 0.1, we will allow tolerance 10, why not... But
  // if the tolerance of the body shape is greater, then we are in a trouble
  if ( maxTol < 100*inaccuracy )
    return true;

  return false;
}

//! Checks whether the working shell is connected.
//! \param Journal [in/out] journal instance.
//! \return true/false.
bool geom_check_shell::Check_isConnected(ActAPI_ProgressEntry ASitus_NotUsed(Journal)) const
{
  BRepCheck_Shell Checker(m_S);
  if ( Checker.Closed() == BRepCheck_NotConnected )
    return false;

  return true;
}

//! Checks the working shell against all OCCT validity criteria.
//! \param inaccuracy [in]     characteristic inaccuracy.
//! \param Journal    [in/out] journal instance.
//! \return true/false.
bool geom_check_shell::Check_everything(const double         inaccuracy,
                                        ActAPI_ProgressEntry Journal) const
{
  BRepCheck_Analyzer Check(m_S, 1);
  const bool isValid = Check.IsValid() > 0;

  return isValid && Check_isAccurate(inaccuracy, NULL);
}
