//-----------------------------------------------------------------------------
// Created on: 25 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
//    * Neither the name of the copyright holder(s) nor the
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

#ifndef asiUI_ParameterEditorListener_h
#define asiUI_ParameterEditorListener_h

// asiUI includes
#include <asiUI_CommonFacilities.h>
#include <asiUI_ParameterEditor.h>

// Qt includes
#include <QObject>

#pragma warning(disable : 4251)

//-----------------------------------------------------------------------------

//! Basic slots for parameter editor. This class is responsible for performing
//! the Data Model modification.
class asiUI_EXPORT asiUI_ParameterEditorListener : public QObject
{
  Q_OBJECT

public:

  //! Constructor accepting all necessary facilities.
  //! \param[in] cf common facilities.
  asiUI_ParameterEditorListener(const Handle(asiUI_CommonFacilities)& cf);

  //! Dtor.
  virtual
    ~asiUI_ParameterEditorListener();

public:

  //! Connects this listener to the controls widget.
  virtual void
    Connect();

protected slots:

  //! Stores the modified parameter values in the data model. This virtual
  //! method can be overridden in the sub-classes to add custom processing.
  //! \param[in] pid   ID of the parameter.
  //! \param[in] value new value of the modified parameter.
  virtual void
    onParameterChanged(const int pid, const QVariant& value);

signals:

  //! Emitted just after the transactional modification in the data model
  //! is done.
  void parameterStored();

protected:

  //! Performs actual modification of Data Model. Must be executed in
  //! transactional scope only.
  //! \param[in] pid   ID of the parameter.
  //! \param[in] value new value of the modified parameter.
  //! \return true in case of success, false -- otherwise.
  virtual bool
    applyParameter(const int pid, const QVariant& value);

  //! Callback invoked by the base class of listener before the Data Model
  //! is modified. The derived classes can override this method to provide
  //! their custom pre-threatment. At this stage, it is possible to return
  //! false in case if the default Data Model modification should not be done.
  //! \param[in]  N              Data Node which has been modified.
  //! \param[in]  pid            local ID of a Parameter which has been modified.
  //! \param[in]  value          new value of the modified parameter.
  //! \param[out] proceedDefault false if the derived class wants to avoid the
  //!                            default treatment, true -- if the derived
  //!                            class is Ok that the base class continues
  //!                            its job.
  virtual void
    beforeParameterChanged(const Handle(ActAPI_INode)& N,
                           const int                   pid,
                           const QVariant&             value,
                           bool&                       proceedDefault)
  {
    asiUI_NotUsed(N);
    asiUI_NotUsed(pid);
    asiUI_NotUsed(value);
    asiUI_NotUsed(proceedDefault);
  }

  //! Callback invoked by the base class of listener once the Data Model
  //! is modified. The derived classes can override this method to provide
  //! their custom post-threatment. At this stage, since the Data Model
  //! is modified, the overridden method may consult OCAF directly.
  //! \param[in] N   Data Node which has been modified.
  //! \param[in] pid local ID of a Parameter which has been modified.
  virtual void
    afterParameterChanged(const Handle(ActAPI_INode)& N,
                          const int                   pid)
  {
    asiUI_NotUsed(N);
    asiUI_NotUsed(pid);
  }

protected:

  Handle(asiUI_CommonFacilities) m_cf; //!< Common facilities.

};

#pragma warning(default : 4251)

#endif
