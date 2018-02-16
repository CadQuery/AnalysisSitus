#===============================================================================
# Topo killer / Test case 01:
#-------------------------------------------------------------------------------
# This test case runs a sequence of topo reduction operations emulating double
# KEV for a single planar face.
#-------------------------------------------------------------------------------
# Target CAD file: data/cad/box_face.brep
#===============================================================================

# Initialize naming to better preserve modeling intent.
init-naming

# KEV for edge 1.
kill-edge -name "edge_1"
merge-vertices -name "vertex_1" -name "vertex_2"

# KEV for edge 3.
kill-edge -name "edge_3"
merge-vertices -name "vertex_3" -name "vertex_4"
