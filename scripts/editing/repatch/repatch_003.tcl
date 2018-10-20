# Set working variables.
set datafile cad/situ/repatch/repatch_003.brep

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-brep $datadir/$datafile
fit
#
print-summary
#
set initialToler [get-tolerance]

# Repatch faces.
repatch 6 10 8 9
donly
