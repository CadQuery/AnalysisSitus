//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// ottoCmd includes
#include <ottoCmd.h>

// ottoAlgo includes
#include <ottoAlgo_Utils.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiEngine includes
#include <asiEngine_Part.h>

// Active Data includes
#include <ActData_Application.h>

// DFBrowser includes
#include <DFBrowser.hxx>

// OCCT includes
#include <OSD_Environment.hxx>

//-----------------------------------------------------------------------------

#define DRAW_DEBUG
#if defined DRAW_DEBUG
  #pragma message("===== warning: DRAW_DEBUG is enabled")
#endif

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

Handle(ottoExe_CommonFacilities) ottoCmd::cf = NULL;

//-----------------------------------------------------------------------------

std::string GetExeFileName()
{
  char buffer[MAX_PATH];
  GetModuleFileName( NULL, buffer, MAX_PATH );
  return std::string(buffer);
}

std::string GetExePath()
{
  std::string f = GetExeFileName();
  return f.substr(0, f.find_last_of( "\\/" ));
}

//-----------------------------------------------------------------------------

//! The single instance of Application class is used to manipulate
//! OCAF Documents which in turn contain the actual application data.
//! OCAF Application is responsible for managing CAF documents, i.e. saving,
//! opening, etc.
class OcafEx_Application : public TDocStd_Application
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(OcafEx_Application, TDocStd_Application)

public:

  //! Accessor for the static instance of OCAF Application. If does not
  //! exist, the instance will be created.
  //! \return instance of OCAF Application.
  static Handle(OcafEx_Application) Instance()
  {
    static Handle(OcafEx_Application) App;
    if ( App.IsNull() )
      App = new OcafEx_Application();

    return App;
  }

public:

  //! Enumerates the formats which to be used for persistence.
  //! \param[out] formats collection of accepted formats to be populated
  //!                     by this method.
  virtual void Formats(TColStd_SequenceOfExtendedString& formats)
  {
    formats.Append( TCollection_ExtendedString("BinOcaf") );
    formats.Append( TCollection_ExtendedString("OcafExBin") );
  }

  //! Name of the resources file containing descriptions of the accepted
  //! formats. The filename is set relatively to `CSF_ResourcesDefaults`
  //! environment variable.
  //! \return filename.
  virtual const char* ResourcesName() { return "Resources"; }

private:

  //! Default ctor which is publicly inaccessible as the Application should
  //! be created and accessed via `Instance()` method.
  //! \sa Instance() method to construct and access the Application.
  OcafEx_Application() {}

};


//-----------------------------------------------------------------------------

int OCAF_Browse(const Handle(asiTcl_Interp)& interp,
                int                          argc,
                const char**                 argv)
{
  if ( argc != 1 && argc != 2 && argc != 3 )
  {
    return interp->ErrorOnWrongArgs(argv[0]);
  }

  /* Show this document */
  if ( argc == 1 )
    DFBrowser::DFBrowserCall( ottoCmd::cf->Model->Document() );

  /* Open document */
  if ( argc >= 2 )
  {
    // Flag to force reloading the document instead of taking it from
    // session.
    const bool reload = interp->HasKeyword(argc, argv, "reload");

    Handle(OcafEx_Application) A = OcafEx_Application::Instance();
    Handle(TDocStd_Document) D;

    // Check if the requested document is already in session.
    const int docIdx = A->IsInSession(argv[1]);
    if ( docIdx > 0 )
    {
      interp->GetProgress().SendLogMessage( LogNotice(Normal) << "The requested document is already in session." );

      // Get existing document either to show or to close & reload it.
      A->GetDocument(docIdx, D);

      if ( !reload )
      {
        interp->GetProgress().SendLogMessage( LogNotice(Normal) << "The document is taken from session." );
      }
      else
      {
        interp->GetProgress().SendLogMessage( LogNotice(Normal) << "The document from session is being closed." );
        A->Close(D);
      }
    }

    if ( !docIdx || reload )
    {
      interp->GetProgress().SendLogMessage( LogNotice(Normal) << "The document is being reloaded." );

      // Read.
      PCDM_ReaderStatus readerStatus = PCDM_RS_OpenError;
      try
      {
        readerStatus = A->Open(argv[1], D);
      }
      catch ( Standard_Failure exc )
      {
        interp->GetProgress().SendLogMessage( LogErr(Normal) << "OCCT exception: %1 with message '%2'."
                                                             << exc.DynamicType()->Name()
                                                             << exc.GetMessageString() );
        return TCL_ERROR;
      }

      // Check status.
      if ( readerStatus != PCDM_RS_OK )
      {
        TCollection_AsciiString statusStr;

             if ( readerStatus == PCDM_RS_NoDriver )                    statusStr = "PCDM_RS_NoDriver";
        else if ( readerStatus == PCDM_RS_UnknownFileDriver )           statusStr = "PCDM_RS_UnknownFileDriver";
        else if ( readerStatus == PCDM_RS_OpenError )                   statusStr = "PCDM_RS_OpenError";
        else if ( readerStatus == PCDM_RS_NoVersion )                   statusStr = "PCDM_RS_NoVersion";
        else if ( readerStatus == PCDM_RS_NoSchema )                    statusStr = "PCDM_RS_NoSchema";
        else if ( readerStatus == PCDM_RS_NoDocument )                  statusStr = "PCDM_RS_NoDocument";
        else if ( readerStatus == PCDM_RS_ExtensionFailure )            statusStr = "PCDM_RS_ExtensionFailure";
        else if ( readerStatus == PCDM_RS_WrongStreamMode )             statusStr = "PCDM_RS_WrongStreamMode";
        else if ( readerStatus == PCDM_RS_FormatFailure )               statusStr = "PCDM_RS_FormatFailure";
        else if ( readerStatus == PCDM_RS_TypeFailure )                 statusStr = "PCDM_RS_TypeFailure";
        else if ( readerStatus == PCDM_RS_TypeNotFoundInSchema )        statusStr = "PCDM_RS_TypeNotFoundInSchema";
        else if ( readerStatus == PCDM_RS_UnrecognizedFileFormat )      statusStr = "PCDM_RS_UnrecognizedFileFormat";
        else if ( readerStatus == PCDM_RS_MakeFailure )                 statusStr = "PCDM_RS_MakeFailure";
        else if ( readerStatus == PCDM_RS_PermissionDenied )            statusStr = "PCDM_RS_PermissionDenied";
        else if ( readerStatus == PCDM_RS_DriverFailure )               statusStr = "PCDM_RS_DriverFailure";
        else if ( readerStatus == PCDM_RS_AlreadyRetrievedAndModified ) statusStr = "PCDM_RS_AlreadyRetrievedAndModified";
        else if ( readerStatus == PCDM_RS_AlreadyRetrieved )            statusStr = "PCDM_RS_AlreadyRetrieved";
        else if ( readerStatus == PCDM_RS_UnknownDocument )             statusStr = "PCDM_RS_UnknownDocument";
        else if ( readerStatus == PCDM_RS_WrongResource )               statusStr = "PCDM_RS_WrongResource";
        else if ( readerStatus == PCDM_RS_ReaderException )             statusStr = "PCDM_RS_ReaderException";
        else if ( readerStatus == PCDM_RS_NoModel )                     statusStr = "PCDM_RS_NoModel";

        interp->GetProgress().SendLogMessage(LogErr(Normal) << "Status of reader is not OK. Error code: %1."
                                                            << statusStr);

        return TCL_ERROR;
      }
    }

    DFBrowser::DFBrowserCall(D);
  }

  return TCL_OK;
}

//-----------------------------------------------------------------------------

void ottoCmd::Factory(const Handle(asiTcl_Interp)&      interp,
                      const Handle(Standard_Transient)& data)
{
  static const char* group = "ottoCmd";

  /* ==============================
   *  Initialize common facilities
   * ============================== */

  ottoCmd::cf = Handle(ottoExe_CommonFacilities)::DownCast(data);
  //
  if ( cf.IsNull() )
  {
    interp->GetProgress().SendLogMessage(LogErr(Normal) << "[ottoCmd] Common facilities are NULL.");
    return;
  }

  // This variable should be initialized with a directory which contains
  // the application executable.
  TCollection_AsciiString appDir = GetExePath().c_str();
  TCollection_AsciiString appDirRes = appDir + "\\resources";

  // Set CSF_PluginDefaults, CSF_ResourcesDefaults variables.
  OSD_Environment( "CSF_PluginDefaults",    appDirRes.ToCString() ).Build();
  OSD_Environment( "CSF_ResourcesDefaults", appDirRes.ToCString() ).Build();

  /* =====================
   *  Add custom commands
   * ===================== */

  //-------------------------------------------------------------------------//
  interp->AddCommand("ocaf-browse",
    //
    "ocaf-browse [filename [-reload]]\n"
    "\t Opens CAF browser.",
    //
    __FILE__, group, OCAF_Browse);
}

// Declare entry point PLUGINFACTORY
ASIPLUGIN(ottoCmd)
