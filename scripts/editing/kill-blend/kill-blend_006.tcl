# Set working variables.
set datafile     cad/blends/boxblend_07.brep
set ref_ncomp    0
set ref_ncompso  0
set ref_nso      1
set ref_nshe     1
set ref_nf       11
set ref_nw       12
set ref_ne       24
set ref_nv       16

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
kfmv -name face_9
kev -edge edge_24
kev -edge edge_21
kev -edge edge_11
kev -edge edge_17
kef -face face_7 -kedge edge_25 -sedge edge_27
kef -face face_6 -kedge edge_2 -sedge edge_23
kef -face face_13 -kedge edge_7 -sedge edge_15
#
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}

# Apply geometric operators.
rebuild-edge -name edge_28
rebuild-edge -name edge_4
rebuild-edge -name edge_25
rebuild-edge -name edge_2
rebuild-edge -name edge_29
rebuild-edge -name edge_33
rebuild-edge -name edge_1
rebuild-edge -name edge_7
rebuild-edge -name edge_6
rebuild-edge -name edge_10
rebuild-edge -name edge_8
rebuild-edge -name edge_14
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

# Check orientations of vertices.
if { [check-vertices-ori] != 1 } {
  error "Some edges have non-distinguishable orientations of vertices."
}

# Check contours of faces.
if { [check-contours] != 1 } {
  error "Some faces has open contours."
}
