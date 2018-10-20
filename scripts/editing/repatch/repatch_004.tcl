# Set working variables.
set datafile cad/situ/repatch/repatch_004.brep

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-brep $datadir/$datafile
fit
#
print-summary
#
set initialToler [get-tolerance]

# Repatch faces.
repatch 7 8
donly
