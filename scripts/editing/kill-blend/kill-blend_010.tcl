# Set working variables.
set datafile     cad/nist/nist_ctc_01.brep
set ref_ncomp    0
set ref_ncompso  0
set ref_nso      1
set ref_nshe     1
set ref_nf       150
set ref_nw       175
set ref_ne       415
set ref_nv       268

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
if { [check-euler 10] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}
#
init-naming
kev -name edge_155
kev -name edge_16
kef -face face_34 -kedge edge_154 -sedge edge_156
#
if { [check-euler 10] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}

# Apply geometric operators.
rebuild-edge -name edge_154
rebuild-edge -name edge_153
rebuild-edge -name edge_15
rebuild-edge -name edge_17
rebuild-edge -name edge_158
#
if { [check-validity] != 1 } {
  error "Final part is not valid."
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

# Check that tolernace has not significantly degraded.
set finalToler [get-tolerance]
puts "Final tolerance ($finalToler) vs initial tolerance ($initialToler)"
#
if { [expr $finalToler - $initialToler] > 1e-3 } {
  error "Significant tolerance degradation."
}
