#------------------------------------------------------------------------------
# This script checks for internal locations in all CAD parts located in
# the specified target directory.
#------------------------------------------------------------------------------

set filenames []

proc on_visit {path} {
    global filenames
    #puts $path
    lappend filenames $path
}

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

proc main {base} {
    visit $base *.brep [list on_visit]
}

set workdir "W:/CAD_files/test_cases_2018-04-07"

main [lindex $workdir 0]

puts "filenames: $filenames"

# Load each model and check.
foreach inFilename $filenames {
  puts "Next model to check: $inFilename"
  clear
  load-brep $inFilename
  set hasInterLocs [check-internal-locations -skip-first-level]

  if { $hasInterLocs == 1 } {
    puts "\tinternal locations detected."
  }
}
