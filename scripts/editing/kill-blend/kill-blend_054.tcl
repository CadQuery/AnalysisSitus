clear

# Set working variables.
set datafile cad/blends/forklift_seat_grabcad.stp
set ref_ncomp    0
set ref_ncompso  0
set ref_nso      1
set ref_nshe     1
set ref_nf       223
set ref_nw       295
set ref_ne       631
set ref_nv       398

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-step $datadir/$datafile
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

init-naming
kev -name edge_16
kev -name edge_86
kef -face face_19 -kedge edge_81 -sedge edge_85
rebuild-edge -name edge_85
rebuild-edge -name edge_82
rebuild-edge -name edge_88
rebuild-edge -name edge_17
set-tolerance 0.01
# rebuild-edge -name edge_15

# Check Euler-Poincare property of the manifold after modification.
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
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

# Verify cardinal numbers.
get-summary ncomp ncompso nso nshe nf nw ne nv
#
if { $ncomp != $ref_ncomp } {
  error "Unexpected number of compounds."
}
if { $ncompso != $ref_ncompso } {
  error "Unexpected number of compsolids."
}
if { $nso != $ref_nso } {
  error "Unexpected number of solids."
}
if { $nshe != $ref_nshe } {
  error "Unexpected number of shells."
}
if { $nf != $ref_nf } {
  error "Unexpected number of faces."
}
if { $nw != $ref_nw } {
  error "Unexpected number of wires."
}
if { $ne != $ref_ne } {
  error "Unexpected number of edges."
}
if { $nv != $ref_nv } {
  error "Unexpected number of vertices."
}
