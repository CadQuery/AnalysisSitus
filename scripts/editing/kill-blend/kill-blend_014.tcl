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

# Apply topological operators.
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}
#
init-naming
kev -name edge_10
kev -name edge_20
kef -face face_5 -kedge edge_21 -sedge edge_19
#
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}

# Apply geometric operators.
rebuild-edge -name edge_11
rebuild-edge -name edge_19
rebuild-edge -name edge_25
rebuild-edge -name edge_9
rebuild-edge -name edge_30
#
if { [check-validity] != 1 } {
  error "Final part is not valid."
}
