//-----------------------------------------------------------------------------
// Created on: 16 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_InteropVars_h
#define asiAlgo_InteropVars_h

// A-Situs includes
#include <asiAlgo.h>

//---------------------------------------------------------------------------//
// READ
//---------------------------------------------------------------------------//

#define xstep_cascade_unit       "xstep.cascade.unit"
#define xstep_cascade_unit_descr \
"\
 This parameter defines units to which a shape should be converted when translated\n\
 from IGES or STEP to CASCADE. Normally it is MM; only those applications that work\n\
 internally in units other than MM should use this parameter. Default value is MM."

//-----------------------------------------------------------------------------

#define read_step_product_mode       "read.step.product.mode"
#define read_step_product_mode_descr \
"\
 Defines the approach used for selection of top-level STEP entities for translation,\n\
 and for recognition of assembly structures:\n\
 \n\
 - 1 (ON): PRODUCT_DEFINITION entities are taken as top-level ones; assembly structure\n\
   is recognized by NEXT_ASSEMBLY_USAGE_OCCURRENCE entities. This is regular mode for reading\n\
   valid STEP files conforming to AP 214, AP203 or AP 209.\n\
 - 0 (OFF): SHAPE_DEFINITION_REPRESENTATION entities are taken as top-level ones; assembly\n\
   is recognized by CONTEXT_DEPENDENT_SHAPE_REPRESENTATION entities. This is compatibility mode,\n\
   which can be used for reading legacy STEP files produced by older versions of STEP translators\n\
   and having incorrect or incomplete product information."

//-----------------------------------------------------------------------------

#define read_step_product_context       "read.step.product.context"
#define read_step_product_context_descr \
"\
 When reading AP 209 STEP files, allows selecting either only 'design' or 'analysis', or both\n\
 types of products for translation:\n\
 \n\
 - 1 (all): translates all products,\n\
 - 2 (design): translates only products that have PRODUCT_DEFINITION_CONTEXT with field\n\
   life_cycle_stage set to 'design'.\n\
 - 3 (analysis): translates only products associated with PRODUCT_DEFINITION_CONTEXT entity\n\
   whose field life_cycle_stage set to 'analysis'.\n\
 \n\
 Note that in AP 203 and AP214 files all products should be marked as 'design', so if this mode\n\
 is set to 'analysis', nothing will be read."

//-----------------------------------------------------------------------------

#define read_step_shape_repr       "read.step.shape.repr"
#define read_step_shape_repr_descr \
"\
 Specifies preferred type of representation of the shape of the product, in case\n\
 if a STEP file contains more than one representation (i.e. multiple\n\
 PRODUCT_DEFINITION_SHAPE entities) for a single product:\n\
 \n\
 - 1 (All): translate all representations (if more than one, put in compound).\n\
 - 2 (ABSR): prefer ADVANCED_BREP_SHAPE_REPRESENTATION.\n\
 - 3 (MSSR): prefer MANIFOLD_SURFACE_SHAPE_REPRESENTATION.\n\
 - 4 (GBSSR): prefer GEOMETRICALLY_BOUNDED_SURFACE_SHAPE_REPRESENTATION.\n\
 - 5 (FBSR): prefer FACETTED_BREP_SHAPE_REPRESENTATION.\n\
 - 6 (EBWSR): prefer EDGE_BASED_WIREFRAME_SHAPE_REPRESENTATION.\n\
 - 7 (GBWSR): prefer GEOMETRICALLY_BOUNDED_WIREFRAME _SHAPE_REPRESENTATION.\n\
 \n\
 When this option is not equal to 1, for products with multiple representations\n\
 the representation having a type closest to the selected one in this list\n\
 will be translated."

//-----------------------------------------------------------------------------

#define read_step_assembly_level       "read.step.assembly.level"
#define read_step_assembly_level_descr \
"\
 Specifies which data should be read for the products found in the STEP file:\n\
 \n\
 - 1 (All): translate both the assembly structure and all associated shapes. If both\n\
   shape and sub-assemblies are associated with the same product, all of them are read\n\
   and put in a single compound. Note that this situation is confusing, as semantics of\n\
   such configuration is not defined clearly by the STEP standard (whether this shape is an\n\
   alternative representation of the assembly or is an addition to it), therefore warning\n\
   will be issued in such case.\n\
 - 2 (assembly): translate the assembly structure and shapes associated with parts only\n\
   (not with sub-assemblies).\n\
 - 3 (structure): translate only the assembly structure without shapes (a structure of empty\n\
   compounds). this mode can be useful as an intermediate step in applications requiring\n\
   specialized processing of assembly parts.\n\
 - 4 (shape): translate only shapes associated with the product, ignoring the assembly\n\
   structure (if any). This can be useful to translate only a shape associated with specific\n\
   product, as a complement to assembly mode."

//-----------------------------------------------------------------------------

#define read_step_shape_relationship       "read.step.shape.relationship"
#define read_step_shape_relationship_descr \
"\
 Defines whether shapes associated with the main SHAPE_DEFINITION_REPRESENTATION entity\n\
 of the product via SHAPE_REPRESENTATIONSHIP_RELATION should be translated. This kind of\n\
 association is used for the representation of hybrid models (i.e. models whose shape is\n\
 composed of different types of representations) in AP 203 files since 1998, but it can be\n\
 also used to associate auxiliary data with the product. This parameter allows to avoid\n\
 translation of such auxiliary data.\n\
 \n\
 - 1 (ON): translate.\n\
 - 0 (OFF): do not translate."

//-----------------------------------------------------------------------------

#define read_step_shape_aspect       "read.step.shape.aspect"
#define read_step_shape_aspect_descr \
"\
 Defines whether shapes associated with the PRODUCT_DEFINITION_SHAPE entity of the\n\
 product via SHAPE_ASPECT should be translated. This kind of association was used for\n\
 the representation of hybrid models (i.e. models whose shape is composed of different\n\
 types of representations) in AP 203 files before 1998, but it is also used to associate\n\
 auxiliary information with the sub-shapes of the part. Though STEP translator tries to\n\
 recognize such cases correctly, this parameter may be useful to avoid unconditionally\n\
 translation of shapes associated via SHAPE_ASPECT entities.\n\
 \n\
 - 1 (ON): translate.\n\
 - 0 (OFF): do not translate."

//-----------------------------------------------------------------------------

#define read_precision_mode       "read.precision.mode"
#define read_precision_mode_descr \
"\
 Defines which precision value will be used during translation.\n\
 \n\
 - File (0): precision is set to length_measure in uncertainty_measure_with_unit from STEP file.\n\
 - User (1): precision is that of the read.precision.val parameter."

//-----------------------------------------------------------------------------

#define read_precision_val       "read.precision.val"
#define read_precision_val_descr \
"\
 User defined precision value. This parameter gives the precision for shape\n\
 construction when the read.precision.mode parameter value is 1. By default it\n\
 is 0.0001, but can be any real positive (non null) value. This value is a basic\n\
 value of tolerance in the processor. The value is in millimeters, independently\n\
 of the length unit defined in the STEP file.\n\
 \n\
 The value given to this parameter is a basic value for Shape Healing algorithms\n\
 and the processor. It does its best to reach it. Under certain circumstances, the\n\
 value you give may not be attached to all of the entities concerned at the end of\n\
 processing. STEP-to-OpenCascade translation does not improve the quality of the\n\
 geometry in the original STEP file. This means that the value you enter may be\n\
 impossible to attach to all shapes with the given quality of the geometry in the\n\
 STEP file."

//-----------------------------------------------------------------------------

#define read_maxprecision_val       "read.maxprecision.val"
#define read_maxprecision_val_descr \
"\
 Defines the maximum allowed tolerance (in mm) of the shape. It should be not\n\
 less than the basic value of tolerance set in the processor (either the uncertainty\n\
 from the file or read.precision.val). Actually, the maximum between read.maxprecision.val\n\
 and the basis tolerance is used to define the maximum allowed tolerance.\n\
 \n\
 Default value is 1. Note that maximum tolerance even explicitly defined by the\n\
 user may be insufficient to ensure the validity of the shape (if real geometry is\n\
 of bad quality). Therefore the user is provided with an additional parameter,\n\
 which allows him to choose: either he prefers to ensure the shape validity or he\n\
 rigidly sets the value of maximum tolerance. In the first case there is a possibility\n\
 that the tolerance will not have any upper limit, in the second case the shape may be invalid."

//-----------------------------------------------------------------------------

#define read_maxprecision_mode       "read.maxprecision.mode"
#define read_maxprecision_mode_descr \
"\
 Defines the mode of applying the maximum allowed tolerance. Its possible values are:\n\
 \n\
 - 0 (Preferred): max tolerance is used as a limit but sometimes it can be exceeded\n\
   (currently, only for deviation of a 3D curve and pcurves of an edge, and vertices\n\
   of such edge) to ensure the shape validity.\n\
 - 1 (Forced): max tolerance is used as a rigid limit, i.e. no tolerance can exceed\n\
   it and if it is the case, the tolerance is trimmed by the maximum tolerance.\n\
 \n\
 Default value is 0 ('Preferred')."

//-----------------------------------------------------------------------------

#define read_surfacecurve_mode       "read.surfacecurve.mode"
#define read_surfacecurve_mode_descr \
"\
 Indicates whether to read or not p-curves. If both 2D and 3D representation of\n\
 the entity are present, the computation of these curves depends on the following\n\
 values of parameter:\n\
 \n\
 - Default (0): no preference, both curves are taken (default value).\n\
 - 3DUse_Preferred (3): 3D curves are used to rebuild 2D ones."

//-----------------------------------------------------------------------------

#define read_stdsameparameter_mode       "read.stdsameparameter.mode"
#define read_stdsameparameter_mode_descr \
"\
 Defines the use of BRepLib::SameParameter. Its possible values are:\n\
 \n\
 - 0 (Off): BRepLib::SameParameter is not called.\n\
 - 1 (On): BRepLib::SameParameter is called. The functionality of BRepLib::SameParameter\n\
   is used through ShapeFix_Edge::SameParameter. It ensures that the resulting edge will\n\
   have the lowest tolerance taking pcurves either unmodified from the STEP file or modified\n\
   by BRepLib::SameParameter."

//-----------------------------------------------------------------------------

#define read_encoderegularity_angle       "read.encoderegularity.angle"
#define read_encoderegularity_angle_descr \
"\
 This parameter is used for call to BRepLib::EncodeRegularity() function which\n\
 is called for the shape read from an IGES or a STEP file at the end of translation\n\
 process. This function sets the regularity flag of the edge in the shell when this\n\
 edge is shared by two faces. This flag shows the continuity these two faces are\n\
 connected with at that edge. Default value is 0.01."

//-----------------------------------------------------------------------------

#define read_step_resource_name "read.step.resource.name"
#define read_step_sequence      "read.step.sequence"
#define read_step_sequence_descr \
"\
 These two parameters define the name of the resource file and the name of the\n\
 sequence of operators (defined in that file) for shape processing, which is\n\
 automatically performed by the STEP translator. Shape processing is a user-configurable\n\
 step, which is performed after translation and consists in applying a set of operators\n\
 to a resulting shape. This is a very powerful tool allowing customizing the shape and\n\
 adapting it to the needs of a receiving application. By default the sequence consists of a\n\
 single operator ShapeFix (that is how Shape Healing is called from the STEP translator).\n\
 \n\
 Ypu may find an example of the resource file for STEP (which defines parameters corresponding\n\
 to the sequence applied by default, i.e. if the resource file is not found) in the OpenCascade\n\
 installation, by the path CASROOT%/src/XSTEPResource/STEP.\n\
 \n\
 In order for the STEP translator to use that file, you have to define the CSF_STEPDefaults\n\
 environment variable, which should point to the directory where the resource file resides. Note\n\
 that if you change parameter read.step.resource.name, you will change the name of the resource\n\
 file and the environment variable correspondingly.\n\
 \n\
 Default values are as follows:\n\
 \n\
 - read.step.resource.name: STEP,\n\
 - read.step.sequence: FromSTEP."

//-----------------------------------------------------------------------------

#define read_step_nonmanifold      "read.step.nonmanifold"
#define read_step_nonmanifold_descr \
"Indicates whether to read the model in the non-manifold mode."

//---------------------------------------------------------------------------//
// WRITE
//---------------------------------------------------------------------------//

#define write_step_product_name       "write.step.product.name"
#define write_step_product_name_descr \
"\
 Defines the text string that will be used for field 'name' of PRODUCT entities\n\
 written to the STEP file. The default value is 'OCCT STEP translator' plus\n\
 current OCCT version number."

//-----------------------------------------------------------------------------

#define write_step_schema       "write.step.schema"
#define write_step_schema_descr \
"\
 Defines the version of schema used for the output STEP file.\n\
 \n\
 - 1 or 'AP214CD' (default): AP214, CD version (dated 26 November 1996).\n\
 - 2 or 'AP214DIS': AP214, DIS version (dated 15 September 1998).\n\
 - 3 or 'AP203': AP203, possibly with modular extensions (depending on data written to a file).\n\
 - 4 or 'AP214IS': AP214, IS version (dated 2002)."

//-----------------------------------------------------------------------------

#define write_step_unit       "write.step.unit"
#define write_step_unit_descr \
"\
 Defines a unit in which the STEP file should be written. If set to unit other\n\
 than MM, the model is converted to these units during the translation.\n\
 Default value is MM."

//-----------------------------------------------------------------------------

#define write_step_assembly       "write.step.assembly"
#define write_step_assembly_descr \
"\
 Controls assembly writing mode.\n\
 \n\
 - 0 (Off): (default) writes STEP files without assemblies.\n\
 - 1 (On): writes all shapes in the form of STEP assemblies.\n\
 - 2 (Auto): writes shapes having a structure of (possibly nested)\n\
   TopoDS_Compounds in the form of STEP assemblies (single shapes are\n\
   written without assembly structures)."

//-----------------------------------------------------------------------------

#define write_step_resource_name "write.step.resource.name"
#define write_step_sequence      "write.step.sequence"
#define write_step_sequence_descr \
"\
 Two parameters (write.step.resource.name and write.step.sequence) define the\n\
 name of the resource file and the name of the sequence of operators (defined\n\
 in that file) for shape processing, which is automatically performed by the\n\
 STEP translator before translating a shape to a STEP file. Shape processing is\n\
 a user-configurable step, which is performed before the translation and consists\n\
 in applying a set of operators to a resulting shape. This is a very powerful tool\n\
 allowing customizing the shape and adapting it to the needs of a receiving\n\
 application. By default the sequence consists of two operators: SplitCommonVertex\n\
 and DirectFaces, which convert some geometry and topological constructs valid in\n\
 OpenCascade but not in STEP to equivalent definitions conforming to STEP format.\n\
 \n\
 Default values are as follows:\n\
 \n\
 - read.step.resource.name: STEP,\n\
 - read.step.sequence: ToSTEP."

//-----------------------------------------------------------------------------

#define write_surfacecurve_mode       "write.surfacecurve.mode"
#define write_surfacecurve_mode_descr \
"Indicates whether to write or not p-curves."

//-----------------------------------------------------------------------------

#define write_precision_mode       "write.precision.mode"
#define write_precision_mode_descr \
"\
 Controls which precision value to write.\n\
 \n\
 - Least (-1): the uncertainty value is set to the minimum tolerance of an OCCT shape.\n\
 - Average (0): the uncertainty value is set to the average tolerance of an OCCT shape.\n\
 - Greatest (1): the uncertainty value is set to the maximum tolerance of an OCCT shape.\n\
 - Session (2): the uncertainty value is that of the write.precision.val parameter."

//-----------------------------------------------------------------------------

#define write_precision_val       "write.precision.val"
#define write_precision_val_descr \
"\
 User-defined precision value. This parameter gives the uncertainty for STEP entities\n\
 constructed from OCCT shapes when the write.precision.mode parameter value is 2."

//-----------------------------------------------------------------------------

#define write_step_vertex_mode "write.step.vertex.mode"
#define write_step_vertex_mode_descr \
"\
 This parameter defines the writing mode for the free vertices.\n\
 \n\
 - 0 (One Compound): (default) all free vertices are united into one compound\n\
   and exported in one SHAPE DEFINITION REPRESENTATION (vertex name and style are lost).\n\
 - 1 (Single Vertex): each vertex exported in its own SHAPE DEFINITION REPRESENTATION\n\
   (vertex name and style are not lost, but size of the output STEP file increases)."

//-----------------------------------------------------------------------------

#define write_step_nonmanifold "write.step.nonmanifold"
#define write_step_nonmanifold_descr \
"Indicates whether to write the model in the non-manifold mode."

//-----------------------------------------------------------------------------

#endif
