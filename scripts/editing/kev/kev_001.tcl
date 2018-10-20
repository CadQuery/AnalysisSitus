#===============================================================================
# Topo killer / Test case 04:
#-------------------------------------------------------------------------------
# This test case runs a sequence of topo reduction operations emulating double
# KEV and one KEF for a solid.
#-------------------------------------------------------------------------------
# Target CAD file: data/cad/boxblend_01.brep
#===============================================================================

set datafile cad/blends/boxblend_01.brep

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-brep $datadir/$datafile
fit

# Initialize naming to better preserve modeling intent.
init-naming

# KEV for edge 5.
kill-edge -name "edge_5"
merge-vertices -name "vertex_1" -name "vertex_5"

# KEV for edge 10.
kill-edge -name "edge_10"
merge-vertices -name "vertex_3" -name "vertex_8"

# KEF for face 3.
merge-edges -name "edge_2" -name "edge_9"
kill-face -name "face_3"

# Check Euler-Poincare property.
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}
