//-----------------------------------------------------------------------------
// Created on: 10 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiUI_PrimDataAdaptor_h
#define asiUI_PrimDataAdaptor_h

// A-Situs includes
#include <asiUI_DataAdaptor.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// Qt includes
#pragma warning(push, 0)
#include <QVariant>
#pragma warning(pop)

DEFINE_STANDARD_HANDLE(asiUI_PrimDataAdaptor, asiUI_DataAdaptor)

//! Adaptor class representing scalar data for Data-View connectors. Client
//! code should provide an actual implementation of this class in order to
//! customize data accessors which are essentially domain-specific.
class asiUI_PrimDataAdaptor : public asiUI_DataAdaptor
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_PrimDataAdaptor, asiUI_DataAdaptor)

public:

  //! Initializes Data Adaptor with Node and Parameter ID.
  //! \param N   [in] source Data Node.
  //! \param PID [in] Parameter ID.
  asiUI_PrimDataAdaptor(const Handle(ActAPI_INode)& N,
                        const int                   PID)
  : asiUI_DataAdaptor()
  {
    this->Init(N, PID);
  }

public:

  //! Initializes Data Adaptor.
  //! \param N   [in] source Data Node.
  //! \param PID [in] Parameter ID.
  virtual void Init(const Handle(ActAPI_INode)& N,
                    const int                   PID)
  {
    m_node = N;
    m_iPID = PID;
  }

public:

  //! Accessor for Node.
  //! \return Node.
  virtual Handle(ActAPI_INode) Node() const
  {
    return m_node;
  }

  //! Returns ID of the Data Adaptor.
  //! \return PID used as Data Adaptor ID.
  virtual int ID() const
  {
    return m_iPID;
  }

public:

  asiUI_EXPORT virtual QVariant
    GetValue() const;

  asiUI_EXPORT virtual void
    SetValue(const QVariant& ui_val) const;

protected:

  Handle(ActAPI_INode) m_node; //!< Data Node.
  int                  m_iPID; //!< ID of the actually sourced Parameter.

};

#endif
