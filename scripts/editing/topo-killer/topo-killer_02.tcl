#===============================================================================
# Topo killer / Test case 02:
#-------------------------------------------------------------------------------
# This test case runs a sequence of topo reduction operations emulating double
# KEV for a solid.
#-------------------------------------------------------------------------------
# Target CAD file: data/cad/boxblend_01.brep
#===============================================================================

load-brep F:/OCC/CAD_files/blends/boxblend_01.brep

# Initialize naming to better preserve modeling intent.
init-naming

# KEV for edge 5.
kill-edge -name "edge_5"
merge-vertices -name "vertex_1" -name "vertex_5"

# KEV for edge 10.
kill-edge -name "edge_10"
merge-vertices -name "vertex_8" -name "vertex_3"

# Check Euler-Poincare.
check-euler
