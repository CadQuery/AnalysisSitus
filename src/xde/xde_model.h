//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef xde_model_h
#define xde_model_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TDocStd_Document.hxx>

DEFINE_STANDARD_HANDLE(xde_model, Standard_Transient)

//! Base class for XDE-based model.
class xde_model : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(xde_model, Standard_Transient)

public:

  //! Creates clone of Data Model of the given type.
  //! \return cloned instance.
  template <typename ModelType>
  static Handle(ModelType) CloneInstance()
  {
    Handle(ModelType) aRes = new ModelType;
    if ( aRes->NewEmpty() )
      return aRes;
    return NULL;
  }

// Persistence basic:
public:

  ASitus_EXPORT virtual bool
    NewEmpty();

  ASitus_EXPORT virtual bool
    Open(const TCollection_AsciiString& filename);

  ASitus_EXPORT virtual void
    Release();

  ASitus_EXPORT virtual bool
    SaveAs(const TCollection_AsciiString& filename);

// Transactions:
public:

  ASitus_EXPORT virtual void
    OpenCommand();

  ASitus_EXPORT virtual bool
    HasOpenCommand() const;

  ASitus_EXPORT virtual void
    AbortCommand();

  ASitus_EXPORT virtual void
    CommitCommand();

  ASitus_EXPORT virtual void
    Undo();

  ASitus_EXPORT virtual void
    Redo();

// Accessors to raw CAF data & model sections
public:

  ASitus_EXPORT virtual TDF_Label
    RootLabel() const;

  //! Accessor for the underlying OCAF Document.
  //! \return OCAF Document.
  inline const Handle(TDocStd_Document)& Document() const
  {
    return m_doc;
  }

// Construction & initialization:
protected:

  ASitus_EXPORT
    xde_model();

  ASitus_EXPORT void
    init(const Handle(TDocStd_Document)& doc);

  ASitus_EXPORT static Handle(TDocStd_Document)
    newDocument();

// Data containers:
private:

  //! Underlying OCAF document.
  Handle(TDocStd_Document) m_doc;

};

#endif
