# Set working variables.
set datafile     cad/blends/boxblend_08.brep
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

# Apply topological operators.
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}
#
init-naming
kfmv -name face_6
kev -name edge_22
kev -name edge_10
kev -name edge_6
kef -face face_10 -kedge edge_21 -sedge edge_18
kef -face face_3 -kedge edge_9 -sedge edge_2
kef -face face_2 -kedge edge_1 -sedge edge_7
#
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}

# Apply geometric operators.
rebuild-edge -name edge_18
rebuild-edge -name edge_1
rebuild-edge -name edge_2
rebuild-edge -name edge_19
rebuild-edge -name edge_20
rebuild-edge -name edge_14
rebuild-edge -name edge_12
rebuild-edge -name edge_3
rebuild-edge -name edge_4
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
