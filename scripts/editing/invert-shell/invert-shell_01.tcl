# Set working variables.
set datafile cad/ANC101.brep

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-brep $datadir/$datafile
fit

# Check finiteness.
set finiteness [check-finite]
#
if { $finiteness != 1 } {
  error "Unexpected finiteness: the solid is expected to be finite initially."
}

# Now invert shells...
invert-shells

# ... and check finiteness again.
set finiteness [check-finite]
#
if { $finiteness != 0 } {
  error "Unexpected finiteness: the solid is expected to be infinite after inversion."
}

# And now invert solid back to make it finite again...
invert-shells

# ... and check finiteness last time.
set finiteness [check-finite]
#
if { $finiteness != 1 } {
  error "Unexpected finiteness: the solid is expected to be finite after last inversion."
}
