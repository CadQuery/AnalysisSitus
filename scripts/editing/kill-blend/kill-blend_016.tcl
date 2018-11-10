# Set working variables.
set datafile cad/nist/nist_ctc_02.brep

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
if { [check-euler 8] != 1 } {
  error "Euler-Poincare property is not equal to the expected value."
}

# Diable notifications from the interpretor.
disable-notifier

# Perform topological operator.
if { [kill-blend 175] != 0 } {
  error "Unexpected result of command."
}

# Enable notifications from the interpretor.
enable-notifier
