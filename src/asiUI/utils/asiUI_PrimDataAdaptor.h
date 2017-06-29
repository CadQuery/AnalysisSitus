//-----------------------------------------------------------------------------
// Created on: 10 December 2015
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
