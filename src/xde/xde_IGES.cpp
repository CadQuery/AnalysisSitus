//-----------------------------------------------------------------------------
// Created on: 17 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <xde_IGES.h>

// OCCT includes
#include <Interface_InterfaceModel.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <IGESData_IGESModel.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>

//! Performs IGES import.
//! \param filename          [in]  file to read.
//! \param result_model      [out] retrieved data as XDE document.
//! \param result_file_props [out] file properties.
bool xde_IGES::Read(const TCollection_AsciiString& filename,
                    Handle(xde_model)&             result_model,
                    common_cad_properties&         result_file_props)
{
  IGESCAFControl_Reader Reader;
  Handle(XSControl_WorkSession) WS = Reader.WS();

  // Read CAD and associated data from file
  try
  {
    if ( !Reader.ReadFile( filename.ToCString() ) )
    {
      std::cout << "Cannot read IGES file" << std::endl;
      clearSession(WS);
      return false;
    }

    // Now it is time to create Model
    if ( !result_model.IsNull() )
      result_model->Release();
    //
    result_model = xde_model::Instance();

    // Translate model into document
    if ( !Reader.Transfer( result_model->GetDocument() ) )
    {
      std::cout << "Cannot transfer CAD data from IGES file to XDE" << std::endl;
      clearSession(WS);
      return false;
    }

    // Load file information
    Handle(IGESData_IGESModel) IGESModel = Reader.IGESModel();
    //
    if ( !IGESModel.IsNull() )
    {
      IGESData_GlobalSection GlobalSect = IGESModel->GlobalSection();
      //
      result_file_props.Clear();
      result_file_props.Author            = ( GlobalSect.AuthorName().IsNull()       ? "<unspecified>" : GlobalSect.AuthorName()->ToCString()       );
      result_file_props.OriginatingSystem = ( GlobalSect.SendName().IsNull()         ? "<unspecified>" : GlobalSect.SendName()->ToCString()         );
      result_file_props.Organization      = ( GlobalSect.CompanyName().IsNull()      ? "<unspecified>" : GlobalSect.CompanyName()->ToCString()      );
      result_file_props.PreProcessor      = ( GlobalSect.InterfaceVersion().IsNull() ? "<unspecified>" : GlobalSect.InterfaceVersion()->ToCString() );
      //
      Handle(TColStd_HSequenceOfHAsciiString) aFileComments = IGESModel->StartSection();
      for ( int aCommIter = aFileComments->Lower(); aCommIter <= aFileComments->Upper(); ++aCommIter )
      {
        if ( aCommIter != aFileComments->Lower() )
        {
          result_file_props.Comments += "\n";
        }
        //
        result_file_props.Comments += aFileComments->Value(aCommIter)->ToCString();
      }
      if ( result_file_props.Comments.isEmpty() )
        result_file_props.Comments = "<unspecified>";
    }

    clearSession(WS);
  }
  catch ( ... )
  {
    std::cout << "Exception on reading IGES file" << std::endl;
    return false;
  }
  return true;
}

//! Clears the passed Work Session object.
//! \param WS [in] Work Session to release.
void xde_IGES::clearSession(const Handle(XSControl_WorkSession)& WS)
{
  if ( WS.IsNull() )
    return;

  // Clear transfer reader
  Handle(XSControl_TransferReader) TransferReader = WS->TransferReader();
  if ( !TransferReader.IsNull() )
    TransferReader->Clear(-1);
}
