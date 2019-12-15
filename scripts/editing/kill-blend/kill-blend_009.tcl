# Set working variables.
set datafile     cad/blends/0020_boxblend_09.brep
set ref_ncomp    1
set ref_ncompso  0
set ref_nso      1
set ref_nshe     1
set ref_nf       6
set ref_nw       6
set ref_ne       12
set ref_nv       8

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
kfmv -name face_7
kfmv -name face_11
kfmv -name face_17
kfmv -name face_26
kfmv -name face_25
kfmv -name face_23
kfmv -name face_6
kfmv -name face_10
kef -face face_18 -kedge edge_25 -sedge edge_36
kef -face face_19 -kedge edge_26 -sedge edge_46
kef -face face_14 -kedge edge_27 -sedge edge_21
kef -face face_3 -kedge edge_9 -sedge edge_2
kef -face face_5 -kedge edge_4 -sedge edge_16
kef -face face_4 -kedge edge_3 -sedge edge_13
kef -face face_20 -kedge edge_33 -sedge edge_35
kef -face face_22 -kedge edge_51 -sedge edge_37
kef -face face_21 -kedge edge_49 -sedge edge_34
kef -face face_16 -kedge edge_22 -sedge edge_40
kef -face face_15 -kedge edge_23 -sedge edge_32
kef -face face_2 -kedge edge_1 -sedge edge_7
#
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}

# Apply geometric operators.
rebuild-edge -name edge_4
rebuild-edge -name edge_2
rebuild-edge -name edge_1
rebuild-edge -name edge_3
rebuild-edge -name edge_37
rebuild-edge -name edge_26
rebuild-edge -name edge_22
rebuild-edge -name edge_34
rebuild-edge -name edge_33
rebuild-edge -name edge_25
rebuild-edge -name edge_21
rebuild-edge -name edge_23
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
  error "Some faces have open contours."
}
