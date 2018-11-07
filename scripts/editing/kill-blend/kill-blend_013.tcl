# Set working variables.
set datafile     cad/blends/isolated_blends_test_01.brep
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

# Check Euler-Poincare property of the manifold before modification.
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}

# Perform topological operator.
kill-blend 5

# Check Euler-Poincare property of the manifold.
if { [check-euler 0] != 1 } {
  error "Euler-Poincare property does not hold after topological modification."
}

# Check orientations of vertices.
if { [check-vertices-ori] != 1 } {
  error "Some edges have non-distinguishable orientations of vertices."
}

# Check contours of faces.
if { [check-contours] != 1 } {
  error "Some faces has open contours."
}

# Check validity of the result.
if { [check-validity] != 1 } {
  error "Final part is not valid."
}
