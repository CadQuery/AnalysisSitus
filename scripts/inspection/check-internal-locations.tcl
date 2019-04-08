#------------------------------------------------------------------------------
# This script checks for internal locations in all CAD parts located in
# the specified target directory.
#------------------------------------------------------------------------------

set workdir "//master/projects/CADProcessor/test_cases"
set targetExt "brep"

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

# Load each model and check.
set numFaulty 0
foreach inFilename $filenames {
  puts "Next model to check: $inFilename"
  clear
  if { [catch {load-brep $inFilename}] } {
    puts stderr "error: cannot read B-Rep file."
    continue
  }
  set hasInterLocs [check-internal-locations -f]

  if { $hasInterLocs == 1 } {
    puts "\tinternal locations at faces detected."
    incr numFaulty
    lappend faultyModels $inFilename
  } else {
    puts "\tOk"
  }
}

puts "Checked [llength $filenames] files: $numFaulty anomalies detected."
