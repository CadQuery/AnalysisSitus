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

# Kill-blends.
if { [kill-blend 33] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 26] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 24] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 17] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 15] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 8] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 6] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 28] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 384] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 382] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 388] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 386] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 496] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 626] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 494] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 623] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 502] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 620] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 500] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 626] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 624] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 498] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 621] != 1 } {
  error "Unexpected result of command."
}
#
if { [kill-blend 496] != 1 } {
  error "Unexpected result of command."
}

# Check Euler-Poincare property of the manifold.
if { [check-euler 11] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}

# Check orientations of vertices.
if { [check-vertices-ori] != 1 } {
  error "Some edges have non-distinguishable orientations of vertices."
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
