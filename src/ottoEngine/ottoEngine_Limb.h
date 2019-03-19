//-----------------------------------------------------------------------------
// Created on: 08 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoEngine_Limb_h
#define ottoEngine_Limb_h

// ottoEngine includes
#include <ottoEngine.h>

// asiEngine includes
#include <asiEngine_Base.h>

// ottoData includes
#include <ottoData_LimbNode.h>

//-----------------------------------------------------------------------------

//! Data Model API for limbs.
class ottoEngine_Limb : public asiEngine_Base
{
public:

  //! Ctor.
  //! \param[in] model    Data Model instance.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  ottoEngine_Limb(const Handle(asiEngine_Model)& model,
                  ActAPI_ProgressEntry           progress = NULL,
                  ActAPI_PlotterEntry            plotter  = NULL)
  //
  : asiEngine_Base (model, progress, plotter) {}

public:

  //! Creates new Limb Node in the Data Model.
  //! \return newly created Limb Node.
  ottoEngine_EXPORT Handle(ottoData_LimbNode)
    CreateLimb();

};

#endif
