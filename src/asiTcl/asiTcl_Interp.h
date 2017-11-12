//-----------------------------------------------------------------------------
// Created on: 22 August 2017
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiTcl_Interp_h
#define asiTcl_Interp_h

// asiTcl includes
#include <asiTcl_CommandInfo.h>

// asiAlgo includes
#include <asiAlgo_Variable.h>

// Active Data includes
#include <ActAPI_IModel.h>
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// OCCT includes
#include <TCollection_AsciiString.hxx>

#define TCL_ERROR 1
#define TCL_OK 0

struct Tcl_Interp; // Forward declaration for Tcl C structure.

//! Tcl interpreter interface in Analysis Situs.
class asiTcl_Interp : public Standard_Transient
{
friend class asiTcl_Plugin;

public:

  //! Type definition for function pointer representing C++ implementation
  //! of Tcl command.
  typedef int (*t_user_func)(const Handle(asiTcl_Interp)& interp,
                             int                          argc,
                             const char**                 argv);

  //! Callback for Tcl.
  struct t_tcl_callback
  {
    //! Ctor accepting Tcl interpreter and user function.
    //! \param[in] interp Tcl interpreter instance.
    //! \param[in] func   user function.
    t_tcl_callback(const Handle(asiTcl_Interp)& interp,
                   t_user_func                  func)
    : Interp (interp),
      Func   (func)
    {}

    //! Invokes user command for the given command line arguments.
    //! \param[in] argc number of arguments.
    //! \param[in] argv arguments.
    //! \return TCL error code (0 == TCL_OK, 1 == TCL_ERROR).
    int Invoke(int          argc,
               const char** argv)
    {
      return this->Func != NULL ? this->Func(this->Interp, argc, argv) : 1;
    }

    Handle(asiTcl_Interp) Interp; //!< Tcl interpreter.
    t_user_func           Func;   //!< User function.
  };

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiTcl_Interp, Standard_Transient)

public:

  //! Constructor.
  asiTcl_EXPORT
    asiTcl_Interp();

  //! Destructor.
  asiTcl_EXPORT
    ~asiTcl_Interp();

public:

  //! Sets Data Model.
  //! \param[in] model Data Model instance.
  void SetModel(const Handle(ActAPI_IModel)& model)
  {
    m_model = model;
  }

  //! \return Data Model instance.
  const Handle(ActAPI_IModel)& GetModel() const
  {
    return m_model;
  }

  //! Sets imperative plotter.
  //! \param[in] plotter imperative plotter to set.
  void SetPlotter(const Handle(ActAPI_IPlotter)& plotter)
  {
    m_plotter = ActAPI_PlotterEntry(plotter);
  }

  //! \return plotter.
  ActAPI_PlotterEntry& GetPlotter()
  {
    return m_plotter;
  }

  //! Sets progress notifier.
  //! \param[in] progress progress notifier to set.
  void SetProgress(const Handle(ActAPI_IProgressNotifier)& progress)
  {
    m_progress = ActAPI_ProgressEntry(progress);
  }

  //! \return progress notifier.
  ActAPI_ProgressEntry& GetProgress()
  {
    return m_progress;
  }

public:

  //! Initializes interpreter.
  asiTcl_EXPORT void
    Init();

  //! Evaluates command.
  //! \param[in] cmd command to evaluate.
  //! \return error code (0 == TCL_OK, 1 == TCL_ERROR).
  asiTcl_EXPORT int
    Eval(const TCollection_AsciiString& cmd);

  //! Adds command to Tcl interpreter.
  //! \param[in] name     command name.
  //! \param[in] help     help string.
  //! \param[in] filename filename where command is implemented.
  //! \param[in] group    group the command to add belongs to.
  //! \param[in] func     pointer to function implementing command.
  //! \return true in case of success, false -- otherwise.
  asiTcl_EXPORT bool
    AddCommand(const TCollection_AsciiString& name,
               const TCollection_AsciiString& help,
               const TCollection_AsciiString& filename,
               const TCollection_AsciiString& group,
               t_user_func                    func);

  //! Takes all available commands from Tcl interpreter and pushes their
  //! names together with additional info to the provided output list.
  //! \param[out] commands output list.
  asiTcl_EXPORT void
    GetAvailableCommands(std::vector<asiTcl_CommandInfo>& commands) const;

  //! Default reaction of interpreter when wrong arguments are passed to
  //! a command.
  //! \param[in] cmd name of the command being executed.
  //! \return Tcl error code (1 == TCL_ERROR).
  asiTcl_EXPORT int
    ErrorOnWrongArgs(const char* cmd);

protected:

  //! Internal method to add command to Tcl interpreter.
  //! \param[in] name     command name.
  //! \param[in] help     help string.
  //! \param[in] filename filename where command is implemented.
  //! \param[in] group    group the command to add belongs to.
  //! \param[in] callback callback.
  //! \return true in case of success, false -- otherwise.
  asiTcl_EXPORT bool
    addCommand(const TCollection_AsciiString& name,
               const TCollection_AsciiString& help,
               const TCollection_AsciiString& filename,
               const TCollection_AsciiString& group,
               t_tcl_callback*                callback);

protected:

  Tcl_Interp*                          m_pInterp; //!< Internal pointer to Tcl interpreter.
  std::vector<TCollection_AsciiString> m_plugins; //!< List of loaded plugins.
  //
  Handle(ActAPI_IModel) m_model;    //!< Data Model instance.
  ActAPI_ProgressEntry  m_progress; //!< Progress sentry.
  ActAPI_PlotterEntry   m_plotter;  //!< Imperative plotter sentry.

};

#endif
