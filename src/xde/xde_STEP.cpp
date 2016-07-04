//-----------------------------------------------------------------------------
// Created on: 03 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <xde_STEP.h>

// OCCT includes
#include <APIHeaderSection_MakeHeader.hxx>
#include <Interface_InterfaceModel.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <StepData_StepModel.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>

//! Performs STEP import.
//! \param filename          [in]  file to read.
//! \param result_model      [out] retrieved data as XDE document.
//! \param result_file_props [out] file properties.
bool xde_STEP::Read(const TCollection_AsciiString& filename,
                    Handle(xde_model)&             result_model,
                    common_cad_properties&         result_file_props)
{
  STEPCAFControl_Reader Reader;
  Handle(XSControl_WorkSession) WS = Reader.Reader().WS();

  // Read CAD and associated data from file
  try
  {
    if ( !Reader.ReadFile( filename.ToCString() ) )
    {
      std::cout << "Cannot read STEP file" << std::endl;
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
      std::cout << "Cannot transfer CAD data from STEP file to XDE" << std::endl;
      clearSession(WS);
      return false;
    }

    // Load file information
    Handle(StepData_StepModel)
      StepModel = Handle(StepData_StepModel)::DownCast( Reader.ChangeReader().Model().get() );
    //
    if ( !StepModel.IsNull() )
    {
      APIHeaderSection_MakeHeader HeaderMaker(StepModel);
      result_file_props.Clear();
      result_file_props.Author            = ( HeaderMaker.NbAuthor() < 1 || HeaderMaker.AuthorValue(1).IsNull()              ? "<unspecified>" : HeaderMaker.AuthorValue(1)->ToCString() );
      result_file_props.OriginatingSystem = ( HeaderMaker.OriginatingSystem().IsNull()                                       ? "<unspecified>" : HeaderMaker.OriginatingSystem()->ToCString() );
      result_file_props.Organization      = ( HeaderMaker.NbOrganization() < 1 || HeaderMaker.OrganizationValue (1).IsNull() ? "<unspecified>" : HeaderMaker.OrganizationValue(1)->ToCString() );
      result_file_props.PreProcessor      = ( HeaderMaker.PreprocessorVersion().IsNull()                                     ? "<unspecified>" : HeaderMaker.PreprocessorVersion()->ToCString() );
      //
      for ( int descIter = 1; descIter <= HeaderMaker.NbDescription(); ++descIter )
      {
        if ( !HeaderMaker.DescriptionValue(descIter).IsNull() )
        {
          if ( !result_file_props.Comments.isEmpty() )
            result_file_props.Comments += "\n";
          //
          result_file_props.Comments += HeaderMaker.DescriptionValue(descIter)->ToCString();
        }
      }
      if ( result_file_props.Comments.isEmpty() )
        result_file_props.Comments = "<unspecified>";
    }

    clearSession(WS);
  }
  catch ( ... )
  {
    std::cout << "Exception on reading STEP file" << std::endl;
    return false;
  }
  return true;
}

//! Clears the passed Work Session object.
//! \param WS [in] Work Session to release.
void xde_STEP::clearSession(const Handle(XSControl_WorkSession)& WS)
{
  if ( WS.IsNull() )
    return;

  // Clear transfer reader
  Handle(XSControl_TransferReader) TransferReader = WS->TransferReader();
  if ( !TransferReader.IsNull() )
    TransferReader->Clear(-1);
}
