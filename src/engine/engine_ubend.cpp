//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <engine_ubend.h>

// A-Situs (geometry) includes
#include <geom_ubend.h>

// A-Situs (common) includes
#include <common_facilities.h>
#include <common_model.h>

// Active Data includes
#include <ActData_UniqueNodeName.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Compound.hxx>

//! \return newly created U-bend Node.
Handle(geom_ubend_node) engine_ubend::Create_UBend()
{
  //---------------------------------------------------------------------------
  // Create persistent object
  //---------------------------------------------------------------------------

  // Access Model
  Handle(common_model) M = common_facilities::Instance()->Model;

  // Create U-bend Node
  Handle(geom_ubend_node)
    ubend_n = Handle(geom_ubend_node)::DownCast( geom_ubend_node::Instance() );
  //
  M->UBendPartition()->AddNode(ubend_n);

  // Initialize
  ubend_n->Init();
  ubend_n->SetName("U-bend");

  //---------------------------------------------------------------------------
  // Prepare path
  //---------------------------------------------------------------------------

  // Create path curve
  TopoDS_Edge EPath = geom_ubend::CreatePathTopo();
  //
  double path_f, path_l;
  Handle(Geom_Curve) c3d = BRep_Tool::Curve(EPath, path_f, path_l);
  //
  ubend_n->SetPath(EPath);

  //---------------------------------------------------------------------------
  // Prepare evolution laws
  //---------------------------------------------------------------------------

  geom_ubend::t_coord_laws xLaws, yLaws;
  {
    // Add Laws container
    Handle(geom_ubend_laws_node)
      ubend_laws_n = Handle(geom_ubend_laws_node)::DownCast( geom_ubend_laws_node::Instance() );
    //
    M->UBendLawsPartition()->AddNode(ubend_laws_n);

    // Initialize
    ubend_laws_n->Init();
    ubend_laws_n->SetName("Evolution Laws");

    // Add as a child for u-bend
    ubend_n->AddChildNode(ubend_laws_n);

    // Create 24 evolution laws for u-bend (12 for X, 12 for Y)
    for ( int k = geom_ubend::Pole_P1; k < geom_ubend::Pole_Last; ++k )
    {
      const geom_ubend::PoleEnum pole_type = (geom_ubend::PoleEnum) k;

      // Add Law container
      Handle(geom_ubend_law_node)
        ubend_law_n = Handle(geom_ubend_law_node)::DownCast( geom_ubend_law_node::Instance() );
      //
      M->UBendLawPartition()->AddNode(ubend_law_n);

      // Add as a child for u-bend
      ubend_laws_n->AddChildNode(ubend_law_n);

      // Generate unique name
      TCollection_ExtendedString law_name("Law for pole");
      law_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(ubend_law_n, ubend_laws_n), law_name);

      // Initialize
      ubend_law_n->Init();
      ubend_law_n->SetName(law_name);

      {
        // Create law
        Handle(calculus_design_law)
          law = geom_ubend::CreateExplicitLaw(pole_type, geom_ubend::Coord_X, path_l);
        //
        xLaws.Append(law);

        // Add coordinate law for X
        Handle(calculus_design_law_node)
          law_n = Handle(calculus_design_law_node)::DownCast( calculus_design_law_node::Instance() );
        //
        M->CalculusDesignLawPartition()->AddNode(law_n);
        //
        law_n->Init(law);
        law_n->SetName("X Law");
        //
        ubend_law_n->AddChildNode(law_n);
      }

      {
        // Create law
        Handle(calculus_design_law)
          law = geom_ubend::CreateExplicitLaw(pole_type, geom_ubend::Coord_Y, path_l);
        //
        yLaws.Append(law);

        // Add coordinate law for Y
        Handle(calculus_design_law_node)
          law_n = Handle(calculus_design_law_node)::DownCast( calculus_design_law_node::Instance() );
        //
        M->CalculusDesignLawPartition()->AddNode(law_n);
        //
        law_n->Init(law);
        law_n->SetName("Y Law");
        //
        ubend_law_n->AddChildNode(law_n);
      }
    }

    // Add as a child for u-bend
    ubend_n->AddChildNode(ubend_laws_n);
  }

  //---------------------------------------------------------------------------
  // Build sections
  //---------------------------------------------------------------------------

  // Create sections
  TopoDS_Compound CTop, CRight, CBottom, CLeft;
  geom_ubend::CreateSectionsTopo(c3d, UBEND_NUM_SECTIONS, geom_ubend::Side_Top,    xLaws, yLaws, CTop);
  geom_ubend::CreateSectionsTopo(c3d, UBEND_NUM_SECTIONS, geom_ubend::Side_Right,  xLaws, yLaws, CRight);
  geom_ubend::CreateSectionsTopo(c3d, UBEND_NUM_SECTIONS, geom_ubend::Side_Bottom, xLaws, yLaws, CBottom);
  geom_ubend::CreateSectionsTopo(c3d, UBEND_NUM_SECTIONS, geom_ubend::Side_Left,   xLaws, yLaws, CLeft);

  // Build a single compound of all sections
  TopoDS_Compound CSections;
  BRep_Builder().MakeCompound(CSections);
  BRep_Builder().Add(CSections, CTop);
  BRep_Builder().Add(CSections, CRight);
  BRep_Builder().Add(CSections, CBottom);
  BRep_Builder().Add(CSections, CLeft);

  // Store geometry
  ubend_n->SetSections(CSections);

  // Return the just created Node
  return ubend_n;
}
