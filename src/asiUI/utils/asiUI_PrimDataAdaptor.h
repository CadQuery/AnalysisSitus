//-----------------------------------------------------------------------------
// Created on: 10 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
  inline virtual void Init(const Handle(ActAPI_INode)& N,
                           const int                   PID)
  {
    m_node = N;
    m_iPID = PID;
  }

public:

  //! Accessor for Node.
  //! \return Node.
  inline virtual Handle(ActAPI_INode) Node() const
  {
    return m_node;
  }

  //! Returns ID of the Data Adaptor.
  //! \return PID used as Data Adaptor ID.
  inline virtual int ID() const
  {
    return m_iPID;
  }

public:

  virtual QVariant
    GetValue() const;

  virtual void
    SetValue(const QVariant& ui_val) const;

protected:

  Handle(ActAPI_INode) m_node; //!< Data Node.
  int                  m_iPID; //!< ID of the actually sourced Parameter.

};

#endif
