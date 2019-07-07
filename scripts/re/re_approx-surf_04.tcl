# Set working variables.
set datafile points/interior-nodes_11.xyz

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-points pts $datadir/$datafile
fit

re-approx-surf res pts -init initSurf -lambda 0.001 -pinned
