# Set working variables.
set datafile     cad/nist/nist_ctc_04.brep
set ref_ncomp    0
set ref_ncompso  0
set ref_nso      1
set ref_nshe     1
set ref_nf       505
set ref_nw       610
set ref_ne       1242
set ref_nv       710

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
if { [check-euler 34] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}
#
init-naming
kfmv -name face_340
kfmv -name face_342
kfmv -name face_336
kfmv -name face_338
kev -name edge_784
kev -name edge_789
kev -name edge_786
kev -name edge_782
kef -face face_329 -kedge edge_825 -sedge edge_827
kef -face face_335 -kedge edge_839 -sedge edge_826
kef -face face_337 -kedge edge_843 -sedge edge_834
kef -face face_334 -kedge edge_833 -sedge edge_835
kef -face face_339 -kedge edge_836 -sedge edge_847
kef -face face_331 -kedge edge_831 -sedge edge_829
kef -face face_341 -kedge edge_851 -sedge edge_830
kef -face face_327 -kedge edge_822 -sedge edge_823
#
if { [check-euler 34] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}
