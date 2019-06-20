# Set working variables.
set datafile points/sampled-surf_01.xyz

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-points pts $datadir/$datafile
fit

# Make average plane.
re-make-average-plane p pts -mobius

# Approximate surface.
re-approx-surf res pts p
