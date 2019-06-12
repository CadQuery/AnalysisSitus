# Set working variables.
set datafile cad/situ/vexity/situ_vexity_017.brep

# -1: undefined
#  0: concave
#  1: convex
#  2: smooth
#  3: smooth concave
#  4: smooth convex
set ref_vexity 4

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-brep $datadir/$datafile
fit

# Initialize naming service for convenient identification.
init-naming

# Check vexity.
set vexity [check-edge-vexity -name "edge_4"]

# Verify result.
if { $vexity != $ref_vexity } {
  error "Unexpected vexity ($ref_vexity expected while $vexity returned)."
}
