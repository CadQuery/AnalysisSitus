#===============================================================================
# Topo killer / Test case 03:
#-------------------------------------------------------------------------------
# This test case runs a sequence of topo reduction operations emulating double
# KEV for a solid. In addition, this test case merges two edges emulating the
# first stage of KEF.
#-------------------------------------------------------------------------------
# Target CAD file: data/cad/boxblend_01.brep
#===============================================================================

# Initialize naming to better preserve modeling intent.
init-naming

# KEV for edge 5.
kill-edge -name "edge_5"
merge-vertices -name "vertex_1" -name "vertex_5"

# KEV for edge 10.
kill-edge -name "edge_10"
merge-vertices -name "vertex_3" -name "vertex_8"

# Merge spring edges of the blend face.
merge-edges -name "edge_2" -name "edge_9"
