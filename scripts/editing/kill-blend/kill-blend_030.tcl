clear

# Set working variables.
set datafile cad/blends/0036_isolated_blends_test_05.brep

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
if { [check-euler 11] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}

# Recognize and kill all blends.
set fids [recognize-blends]
#
if { [llength $fids] != 160 } {
  error "Unexpected number of recognized blend faces."
}
#
if { [kill-blends {*}$fids] != 32 } {
  error "Unexpected number of suppressed blend chains."
}

# Check contours of faces.
if { [check-contours] != 1 } {
  error "Some faces have open contours."
}

# Check validity of the result.
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
