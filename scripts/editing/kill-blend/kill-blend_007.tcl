# Set working variables.
set datafile     cad/blends/0015_bb_boxblend_03.brep
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
kfmv -name face_11
kfmv -name face_15
kfmv -name face_16
kfmv -name face_1
kev -edge edge_20
kev -edge edge_52
kev -edge edge_38
kev -edge edge_57
kev -edge edge_5
kev -edge edge_8
kev -edge edge_17
kev -edge edge_15
kef -face face_13 -kedge edge_41 -sedge edge_42
kef -face face_12 -kedge edge_39 -sedge edge_40
kef -face face_10 -kedge edge_32 -sedge edge_33
kef -face face_14 -kedge edge_45 -sedge edge_46
kef -face face_7 -kedge edge_24 -sedge edge_26
kef -face face_6 -kedge edge_21 -sedge edge_23
kef -face face_19 -kedge edge_59 -sedge edge_58
kef -face face_17 -kedge edge_53 -sedge edge_54
kef -face face_18 -kedge edge_55 -sedge edge_56
kef -face face_2 -kedge edge_6 -sedge edge_7
kef -face face_8 -kedge edge_28 -sedge edge_29
kef -face face_29 -kedge edge_12 -sedge edge_65
kef -face face_3 -kedge edge_9 -sedge edge_10
#
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}

# Apply geometric operators.
rebuild-edge -name edge_24
rebuild-edge -name edge_32
rebuild-edge -name edge_21
rebuild-edge -name edge_45
rebuild-edge -name edge_58
rebuild-edge -name edge_39
rebuild-edge -name edge_62
rebuild-edge -name edge_55
rebuild-edge -name edge_53
rebuild-edge -name edge_61
rebuild-edge -name edge_60
rebuild-edge -name edge_41
rebuild-edge -name edge_9
rebuild-edge -name edge_6
rebuild-edge -name edge_28
rebuild-edge -name edge_64
rebuild-edge -name edge_63
rebuild-edge -name edge_18
rebuild-edge -name edge_13
rebuild-edge -name edge_16
rebuild-edge -name edge_11
rebuild-edge -name edge_12
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
