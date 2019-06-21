# Set working variables.
set datafile points/interior-nodes_02.xyz

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
load-points pts $datadir/$datafile
fit

re-approx-surf initSurf pts 3 3 -lambda 0.1
insert-knot-surf-u initSurf initSurf 0.1
insert-knot-surf-u initSurf initSurf 0.2
insert-knot-surf-u initSurf initSurf 0.3
insert-knot-surf-u initSurf initSurf 0.4
insert-knot-surf-u initSurf initSurf 0.5
insert-knot-surf-u initSurf initSurf 0.6
insert-knot-surf-u initSurf initSurf 0.7
insert-knot-surf-u initSurf initSurf 0.8
insert-knot-surf-u initSurf initSurf 0.9
insert-knot-surf-v initSurf initSurf 0.1
insert-knot-surf-v initSurf initSurf 0.2
insert-knot-surf-v initSurf initSurf 0.3
insert-knot-surf-v initSurf initSurf 0.4
insert-knot-surf-v initSurf initSurf 0.5
insert-knot-surf-v initSurf initSurf 0.6
insert-knot-surf-v initSurf initSurf 0.7
insert-knot-surf-v initSurf initSurf 0.8
insert-knot-surf-v initSurf initSurf 0.9
re-approx-surf res pts -init initSurf -lambda 0.001 -pinned
#re-approx-surf res pts -init initSurf -lambda 0.001
