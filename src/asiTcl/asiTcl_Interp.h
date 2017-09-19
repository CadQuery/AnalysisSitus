//-----------------------------------------------------------------------------
// Created on: 22 August 2017
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiTcl_Interp_h
#define asiTcl_Interp_h

// asiTcl includes
#include <asiTcl.h>

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
  //! \param[in] func     pointer to function implementing command.
  //! \return true in case of success, false -- otherwise.
  asiTcl_EXPORT bool
    AddCommand(const TCollection_AsciiString& name,
               const TCollection_AsciiString& help,
               const TCollection_AsciiString& filename,
               t_user_func                    func);

  //! Takes all available commands from Tcl interpreter and pushes their
  //! names together with additional info to the provided output list.
  //! \param[out] commands output list.
  asiTcl_EXPORT void
    GetAvailableCommandNames(std::vector<asiAlgo_Variable>& commands) const;

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
  //! \param[in] callback callback.
  //! \return true in case of success, false -- otherwise.
  asiTcl_EXPORT bool
    addCommand(const TCollection_AsciiString& name,
               const TCollection_AsciiString& help,
               const TCollection_AsciiString& filename,
               t_tcl_callback*                callback);

protected:

  Tcl_Interp*           m_pInterp;  //!< Internal pointer to Tcl interpreter.
  //
  Handle(ActAPI_IModel) m_model;    //!< Data Model instance.
  ActAPI_ProgressEntry  m_progress; //!< Progress sentry.
  ActAPI_PlotterEntry   m_plotter;  //!< Imperative plotter sentry.

};

#endif
