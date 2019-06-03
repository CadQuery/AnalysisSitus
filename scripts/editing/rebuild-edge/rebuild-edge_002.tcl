# This unit test is aimed to verify that metadata is preserved on edge-rebuild
# operation in case if there are more than one trailing face for each extremity
# in the corresponding topological situation. Essentially, this test checks that
# the modification history is propagated to the fan of s-t (support-terminating)
# edges and the corresponding terminating faces.

proc check-metadata {pidShape refNumMeta} {
  # Get IDs of all metadata elements.
  set metaElems [get-metadata-ids]

  if { [llength $metaElems] != $refNumMeta } {
    error "Unexpected number of metadata elements (expected $refNumMeta)."
  }

  # Loop over the metadata elements and check if the part shape
  # contains all sub-shapes stored in these elements.
  foreach elemId $metaElems {
    get-param-shape test $elemId $pidShape

    if { [check-part-contains test] != 1 } {
      error "Part does not contain sub-shape from metadata element $elemId. There should be a problem in history."
    }
  }
}

# Set working variables.
set datafile cad/blends/nist_ctc_01_asme1_ap242.brep

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

# Add some metadata.
set-face-color -fid 104 -color rgb(140,20,50)
#
set-face-color -fid 105 -color rgb(40,85,150)
set-face-color -fid 106 -color rgb(40,85,150)
set-face-color -fid 107 -color rgb(40,85,150)
set-face-color -fid 108 -color rgb(40,85,150)
set-face-color -fid 109 -color rgb(40,85,150)
set-face-color -fid 110 -color rgb(40,85,150)
#
set-face-color -fid 111 -color rgb(60,180,40)
set-face-color -fid 112 -color rgb(60,180,40)
set-face-color -fid 113 -color rgb(60,180,40)
set-face-color -fid 114 -color rgb(60,180,40)
set-face-color -fid 115 -color rgb(60,180,40)
set-face-color -fid 116 -color rgb(60,180,40)

# Initialize naming service
init-naming

# Apply geometric operators.
rebuild-edge -name edge_310; donly

# Check metadata.
set pid 1
set refMetaNum 13
#
check-metadata $pid $refMetaNum

# Check validity.
if { [check-validity] != 1 } {
  error "Final part is not valid."
}

# Check that tolernace has not significantly degraded.
set finalToler [get-tolerance]
puts "Final tolerance ($finalToler) vs initial tolerance ($initialToler)"
#
if { [expr $finalToler - $initialToler] > 1e-3 } {
  error "Significant tolerance degradation."
}

# Check orientations of vertices.
if { [check-vertices-ori] != 1 } {
  error "Some edges have non-distinguishable orientations of vertices."
}

# Check contours of faces.
if { [check-contours] != 1 } {
  error "Some faces have open contours."
}
