#===============================================================================
# KEV / Test case 02:
#-------------------------------------------------------------------------------
# This test case applies KEV (Kill Edge-Vertex) Euler operator to solid body.
#-------------------------------------------------------------------------------
# Target CAD file: data/cad/boxblend_01.brep
#===============================================================================

set datafile cad/blends/0007_boxblend_01.brep

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-brep $datadir/$datafile
fit

# Initialize naming to better preserve modeling intent.
init-naming

# KEV for edge 5.
kev -edge "edge_5" -vertex "vertex_1"

# KEV for edge 10.
kev -edge "edge_10" -vertex "vertex_8"

# Check Euler-Poincare property.
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}

# Check orientations of vertices.
if { [check-vertices-ori] != 1 } {
  error "Some edges have non-distinguishable orientations of vertices."
}
