#===============================================================================
# KEV / Test case 01:
#-------------------------------------------------------------------------------
# This test case applies KEV (Kill Edge-Vertex) Euler operator to solid body.
#-------------------------------------------------------------------------------
# Target CAD file: data/cad/boxblend_01.brep
#===============================================================================

load-brep F:/OCC/CAD_files/blends/boxblend_01.brep

# Initialize naming to better preserve modeling intent.
init-naming

# KEV for edge 5.
kev -edge "edge_5" -vertex "vertex_1"

# KEV for edge 10.
kev -edge "edge_10" -vertex "vertex_8"
