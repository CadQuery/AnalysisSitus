# This procedure is used to check if metadata in the Part Node is consistent or not.
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
