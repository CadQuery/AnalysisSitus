# Set working variables.
set datafile cad/situ/repatch/repatch_002.brep

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-brep $datadir/$datafile
fit
#
print-summary
#
set initialToler [get-tolerance]

# Repatch faces.
repatch 3 4
donly
