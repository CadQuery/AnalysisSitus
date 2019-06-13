# Set working variables.
set datafile points/sampled-surf_01.xyz

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-points pts $datadir/$datafile
fit

# Make average plane.
re-make-average-plane p pts -2 2 -2 2 -mobius; hide p

# Convert plane to a bicubic B-surface.
convert-to-bsurf bp p -cubic

# Invert all points.
re-invert-points bp pts
