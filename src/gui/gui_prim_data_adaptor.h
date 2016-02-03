//-----------------------------------------------------------------------------
// Created on: 10 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_prim_data_adaptor_h
#define gui_prim_data_adaptor_h

// A-Situs includes
#include <gui_data_adaptor.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// Qt includes
#pragma warning(push, 0)
#include <QVariant>
#pragma warning(pop)

DEFINE_STANDARD_HANDLE(gui_prim_data_adaptor, gui_data_adaptor)

//! Adaptor class representing scalar data for Data-View connectors. Client
//! code should provide an actual implementation of this class in order to
//! customize data accessors which are essentially domain-specific.
class gui_prim_data_adaptor : public gui_data_adaptor
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(gui_prim_data_adaptor, gui_data_adaptor)

public:

  //! Initializes Data Adaptor with Node and Parameter ID.
  //! \param N   [in] source Data Node.
  //! \param PID [in] Parameter ID.
  gui_prim_data_adaptor(const Handle(ActAPI_INode)& N,
                        const int                   PID)
  : gui_data_adaptor()
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
