//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef xde_model_h
#define xde_model_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#pragma warning(push, 0)
#include <TDF_LabelSequence.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#pragma warning(pop)

DEFINE_STANDARD_HANDLE(xde_model, Standard_Transient)

//! Base class for XDE-based model.
class xde_model : public Standard_Transient
{
public:

  //! Data model sections.
  enum Section
  {
    Section_XDE        = 1,
    Section_Properties = 2
  };

  //! Sub-sections of the Properties group.
  enum PropertyGroup
  {
    Property_Mesh = 1
  };

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(xde_model, Standard_Transient)

public:

  //! \return Data Model instance.
  static Handle(xde_model) Instance()
  {
    Handle(xde_model) Res = new xde_model;
    if ( Res->NewEmpty() )
      return Res;
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

// Accessors to data:
public:

  ASitus_EXPORT virtual TDF_Label
    GetRootLabel() const;

  ASitus_EXPORT virtual TDF_Label
    GetPropertiesRootLabel() const;

  ASitus_EXPORT virtual TDF_Label
    GetPropertiesLabel(const PropertyGroup group) const;

  ASitus_EXPORT TopoDS_Shape
    GetOneShape() const;

  ASitus_EXPORT void
    GetLabelsOfParts(TDF_LabelSequence& labels);

public:

  //! Accessor for the underlying OCAF Document.
  //! \return OCAF Document.
  inline Handle(TDocStd_Document)& GetDocument()
  {
    return m_doc;
  }

  //! \return shape tool.
  inline Handle(XCAFDoc_ShapeTool) GetShapeTool() const
  {
    return XCAFDoc_DocumentTool::ShapeTool( m_doc->Main() );
  }

  //! \return color tool.
  inline Handle(XCAFDoc_ColorTool) GetColorTool() const
  {
    return XCAFDoc_DocumentTool::ColorTool( m_doc->Main() );
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
