# Set working variables.
set datafile cad/blends/isolated_blends_test_13.brep

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

# Apply topological operators.
if { [check-euler 3] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}
#
init-naming
kev -name edge_737
kev -name edge_730
kef -face face_229 -kedge edge_735 -sedge edge_724
#
if { [check-euler 3] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}

# Apply geometric operators.
rebuild-edge -name edge_724
#
if { [check-validity] != 1 } {
  error "Final part is not valid."
}

# Check that tolernace has not significantly degraded.
set finalToler [get-tolerance]
puts "Final tolerance ($finalToler) vs initial tolerance ($initialToler)"
#
if { [expr $finalToler - $initialToler] > 1e-3 } {
  error "Significant tolerance degradation."
}
