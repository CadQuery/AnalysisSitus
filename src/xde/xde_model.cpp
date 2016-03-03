//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <xde_model.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopoDS_Compound.hxx>
#include <XCAFApp_Application.hxx>

//----------------------------------------------------------------------------
// Construction methods
//----------------------------------------------------------------------------

//! Default constructor. Created DETACHED Model instance.
xde_model::xde_model() : Standard_Transient() {}

//----------------------------------------------------------------------------
// Persistence basic methods
//----------------------------------------------------------------------------

//! Creates empty Data Model attached to the new CAF Document.
//! \return true in case of success, false -- otherwise.
bool xde_model::NewEmpty()
{
  // Initialize consistent Data Model structure
  Handle(TDocStd_Document) doc = this->newDocument();
  this->init(doc);
  //
  return true;
}

//! Initializes the Model instance with the CAF Document opened from
//! file with the given filename.
//! \param filename [in] name of the file containing CAF Document to open.
//! \return true in case of success, false -- otherwise.
bool xde_model::Open(const TCollection_AsciiString& filename)
{
  /* ==========================
   *  Prepare the CAF Document
   * ========================== */

  if ( !m_doc.IsNull() )
    this->Release();

  Handle(XCAFApp_Application) A = XCAFApp_Application::GetApplication();
  //
  Handle(TDocStd_Document) Doc = this->newDocument();

  /* =======================
   *  Open the CAF Document
   * ======================= */

  PCDM_ReaderStatus status = PCDM_RS_OpenError;
  //
  try
  {
    status = A->Open(filename, Doc);
  }
  catch ( Standard_Failure exc )
  {
    std::cout << "OCCT exception:"         << std::endl;
    std::cout << exc.DynamicType()->Name() << std::endl;
    std::cout << exc.GetMessageString()    << std::endl;
    return false;
  }

  // Check status
  if ( status != PCDM_RS_OK )
  {
    std::cout << "Status of reader is not OK" << std::endl;
    return false;
  }

  // Initialize Data Model
  this->init(Doc);

  // Success
  return true;
}

//! Releases the Data Model.
void xde_model::Release()
{
  if ( m_doc.IsNull() )
    return;

  /* ================
   *  Close Document
   * ================ */

  Handle(XCAFApp_Application) A = XCAFApp_Application::GetApplication();
  //
  A->Close(m_doc);
  //
  m_doc.Nullify();
}

//! Saves the model into the file with the given name.
//! \param filename [in] filename.
//! \return true in case of success, false -- otherwise.
bool xde_model::SaveAs(const TCollection_AsciiString& filename)
{
  if ( m_doc.IsNull() )
    return false;

  Handle(XCAFApp_Application) A = XCAFApp_Application::GetApplication();
  //
  PCDM_StoreStatus status = PCDM_SS_WriteFailure;
  //
  try
  {
    status = A->SaveAs(m_doc, filename);
  }
  catch ( Standard_Failure exc )
  {
    std::cout << "OCCT exception:"         << std::endl;
    std::cout << exc.DynamicType()->Name() << std::endl;
    std::cout << exc.GetMessageString()    << std::endl;
    return false;
  }

  // Check status
  if ( status != PCDM_SS_OK )
  {
    std::cout << "Status of writer is not OK" << std::endl;
    return false;
  }

  // Success
  return true;
}

//----------------------------------------------------------------------------
// Transaction mechanism
//----------------------------------------------------------------------------

//! Starts new transactional scope.
void xde_model::OpenCommand()
{
  if ( this->HasOpenCommand() )
    Standard_ProgramError::Raise("Nested transactions are prohibited");

  m_doc->OpenCommand();
}

//! Returns true if any command is opened, false -- otherwise.
//! \return true/false.
bool xde_model::HasOpenCommand() const
{
  return m_doc->HasOpenCommand() > 0;
}

//! Rolls back current transaction.
void xde_model::AbortCommand()
{
  m_doc->AbortCommand();
}

//! Commits current transaction.
void xde_model::CommitCommand()
{
  m_doc->CommitCommand();
}

//! Undoes last transaction.
void xde_model::Undo()
{
  m_doc->Undo();
}

//! Redoes last transaction.
void xde_model::Redo()
{
  m_doc->Redo();
}

//----------------------------------------------------------------------------
// Accessors to data
//----------------------------------------------------------------------------

//! Accessor for the root Label.
//! \return root Label.
TDF_Label xde_model::GetRootLabel() const
{
  return m_doc->Main().Root();
}

//! Accessor for the root Label of Properties section.
//! \return root Label for Properties section.
TDF_Label xde_model::GetPropertiesRootLabel() const
{
  TDF_Label root = m_doc->Main().Root();
  return root.FindChild(2);
}

//! Accessor for the root Label of a specific Properties group.
//! \param group [in] group of interest.
//! \return requested Label.
TDF_Label xde_model::GetPropertiesLabel(const PropertyGroup group) const
{
  TDF_Label groupLab = this->GetPropertiesRootLabel().FindChild(group);

  // Configure name
  Handle(TDataStd_Name) nameAttr;
  if ( !groupLab.FindAttribute(TDataStd_Name::GetID(), nameAttr) )
  {
    switch ( group )
    {
      case Property_Mesh:
        nameAttr->Set(groupLab, "Mesh");
        break;
      //
      default:
        break;
    }
  }

  return groupLab;
}

//! \return geometry only.
TopoDS_Shape xde_model::GetOneShape() const
{
  // Get all parts
  TDF_LabelSequence labels;
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( m_doc->Main() );
  STool->GetFreeShapes(labels);
  //
  if ( !labels.Length() )
    return TopoDS_Shape();
  //
  if ( labels.Length() ==1 )
    return STool->GetShape( labels.First() );

  // Put everything into compound and return
  TopoDS_Compound C;
  BRep_Builder B;
  B.MakeCompound(C);
  for ( int i = 1; i<= labels.Length(); ++i )
  {
    TopoDS_Shape S = STool->GetShape( labels(i) );
    B.Add(C, S);
  }
  return C;
}

//! Returns OCAF labels corresponding to individual parts.
//! \param labels [out] labels of parts.
void xde_model::GetLabelsOfParts(TDF_LabelSequence& labels)
{
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool( m_doc->Main() );
  STool->GetFreeShapes(labels);
}

//----------------------------------------------------------------------------
// Initialization
//----------------------------------------------------------------------------

//! Initializes Data Model with the passed CAF Document and prepares integral
//! Data Model Engines.
//! \param theDoc [in] CAF Document to initialize the Model with.
void xde_model::init(const Handle(TDocStd_Document)& theDoc)
{
  // Initialize data containers
  m_doc = theDoc;
}

//! Creates new CAF Document.
//! \return just allocated CAF Document.
Handle(TDocStd_Document) xde_model::newDocument()
{
  Handle(TDocStd_Document) Doc;
  Handle(XCAFApp_Application) A = XCAFApp_Application::GetApplication();
  A->NewDocument(TCollection_ExtendedString("BinXCAF"), Doc);
  return Doc;
}
