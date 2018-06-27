# Set working variables.
set datafile     cad/situ/situ04.brep
set ref_isClosed 1

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-brep $datadir/$datafile
fit

# Check closeness.
set isClosed [check-contours]

# Verify result.
if { $isClosed != $ref_isClosed } {
  error "Unexpected closeness property ($ref_isClosed expected while $isClosed returned)."
}
