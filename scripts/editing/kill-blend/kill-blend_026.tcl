clear

# Set working variables.
set datafile cad/blends/isolated_blends_test_01.brep

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-brep $datadir/$datafile
fit
#
if { [check-validity] != 1 } {
  error "Initial part is not valid."
}
#
print-summary
#
set initialToler [get-tolerance]

# Check Euler-Poincare property of the manifold before modification.
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}

# Recognize blends.
set fids [recognize-blends]
#
puts "Detected face IDs: $fids"

# Try to kill blends.
kill-blends {*}$fids
