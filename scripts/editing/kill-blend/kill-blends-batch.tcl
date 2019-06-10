#------------------------------------------------------------------------------
# This script checks for internal locations in all CAD parts located in
# the specified target directory.
#------------------------------------------------------------------------------

set workdir "C:/Work/AnalysisSitus/data/cad"
set targetExt "stp"
set outputFile "C:/users/ssv/desktop/kill-blends-batch-log.txt"

set filenames []
set faultyModels []

# Callback on visiting a certain file.
proc on_visit {path} {
  global filenames
  puts "Next filename: $path"
  lappend filenames $path
}

# Recursive visiting procedure.
proc visit {base glob func} {
  foreach f [glob -nocomplain -types f -directory $base $glob] {
    if {[catch {eval $func [list [file join $base $f]]} err]} {
      puts stderr "error: $err"
    }
  }
  foreach d [glob -nocomplain -types d -directory $base *] {
    visit [file join $base $d] $glob $func
  }
}

# Procedure to find files of a certain type.
proc find_files {base ext} {
  visit $base *.$ext [list on_visit]
}

# Find files with a certain extension.
find_files [lindex $workdir 0] $targetExt

# Open file for dumping the output.
set FILE [open $outputFile "w"]

# Load each model and check.
set numFaulty 0
set numChainsTotal 0
set numFacesTotal 0
#
foreach inFilename $filenames {
  puts "Next model to check: $inFilename"
  clear
  if { [catch {load-step $inFilename}] } {
    puts stderr "error: cannot read STEP file."
    continue
  }

  set validityBefore [check-validity]
  set gapsBefore     [check-contours]
  set tolerBefore    [get-tolerance]
  #
  get-summary ncompBefore ncompsoBefore nsoBefore nsheBefore nfBefore nwBefore neBefore nvBefore

  # Attempt to kill blends incrementally.
  set numChains 0
  if { [catch {
    set numChains [kill-blends-inc]
  } err] } {
    incr numFaulty
    puts stderr "error: kill-blend-inc returned error $err."
  }

  set validityAfter [check-validity]
  set gapsAfter     [check-contours]
  set tolerAfter    [get-tolerance]
  #
  get-summary ncompAfter ncompsoAfter nsoAfter nsheAfter nfAfter nwAfter neAfter nvAfter

  puts $FILE "$inFilename"

  if { $numChains == 0 } {
    puts $FILE "\t no blend chains were suppressed."
  } else {
    set numFaces [expr $nfBefore - $nfAfter]

    puts $FILE "\t suppressed $numChains blend chain(s) with $numFaces face(s)."
    puts $FILE "\t validity before: $validityBefore"
    puts $FILE "\t gaps before: $gapsBefore"
    puts $FILE "\t tolerance before: $tolerBefore"
    puts $FILE "\t validity after: $validityAfter"
    puts $FILE "\t gaps after: $gapsAfter"
    puts $FILE "\t tolerance after: $tolerAfter"

    if { [expr $tolerAfter - $tolerBefore] > 1e-3 } {
      puts $FILE "\t CAUTION: significant tolerance degradation."
    }
    if { $validityBefore != $validityAfter } {
      puts $FILE "\t CAUTION: final shape becomes invalid."
    }
    if { $gapsBefore != $gapsAfter } {
      puts $FILE "\t CAUTION: final shape contains new gaps."
    }
  }

  set numChainsTotal [expr $numChainsTotal + $numChains]
  set numFacesTotal [expr $numFacesTotal + $numFaces]

  puts $FILE ""
}

puts $FILE "Checked [llength $filenames] files:\n\t $numChainsTotal chain(s) of $numFacesTotal face(s) suppressed;\n\t $numFaulty errors occurred."

close $FILE
