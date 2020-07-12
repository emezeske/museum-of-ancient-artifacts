# Evan Mezeske's text file splitter.  This script
# provides a simple Tk interface to split a
# single file into a number of smaller files.

set split_fname ""
set split_dirname ""
set fragment_size 4
set fuzzy_fsz 1
set fsz_units Kilobytes

proc split_file {} {

	global split_fname split_dirname fragment_size fuzzy_fsz fsz_units

	if { [ string length $split_fname ] } {

		if { [ file exists $split_fname ] } {

			if { [ string length $split_dirname ] } {

				set fname_tail [ file tail $split_fname ]
				set fname_base [ file join $split_dirname [ file rootname $fname_tail ] ]
				set fname_ext [ file extension $fname_tail ]

				if  { [ regexp {[0-9]+} $fragment_size ] } {

					set frag_sz_bytes ""

					switch $fsz_units {
						Bytes { set frag_sz_bytes $fragment_size }
						Kilobytes { set frag_sz_bytes [ expr $fragment_size * 1024 ] }
						Megabytes { set frag_sz_bytes [ expr $fragment_size * 1024 * 1024 ] }
						default { bad_error "Wacky fsz_units: $fsz_units" }
					}

					if { ![ catch { open $split_fname r } main_fd ] } {

						fconfigure $main_fd -translation binary

						set leftover_bytes ""
						set bytes_needed $frag_sz_bytes
						set iter_count 0

						while { ![ eof $main_fd ] } {

							set bytes_needed [ expr $frag_sz_bytes - [ string length $leftover_bytes ] ]
							set next_block $leftover_bytes[ read $main_fd $bytes_needed ]

							if { $fuzzy_fsz != 0 && ![ eof $main_fd ] } {

								set fuzzy_block_size [ string last . $next_block ]  

								if { $fuzzy_block_size != -1 } {

									set leftover_bytes [ string range $next_block [ expr $fuzzy_block_size + 1 ] end ]
									set next_block [ string range $next_block 0 $fuzzy_block_size ]	
								}
							}	

							if { ![ catch { open ${fname_base}_${iter_count}_$fname_ext w } frag_fd ] } {

								fconfigure $frag_fd -translation binary
								puts -nonewline $frag_fd $next_block
								if { [ catch { close $frag_fd } errstr ] } { fatal_error "Error closing fragment file: $frag_fd" }
								incr iter_count

							} else { fatal_error "Couldn't open fragment file for writing: $frag_fd" }
						}

						alert "Done splitting [ file tail $split_fname ] into $iter_count parts!"
						catch { close $split_fname }
					
					} else { fatal_error "Couldn't open $split_fname for writing: $main_fd" } 

				} else { alert "Maximum fragment size must be an integer." }

			} else { alert "Please supply a directory to contain the new files!!" }

		} else { alert "No such file '$split_fname" }

	} else { alert "Please supply a file to split!!" }
}

proc fatal_error { last_words } {
	toplevel .say_bye_window
	wm title .say_bye_window "FATAL ERROR"
	label .say_bye_window.lab -text "$last_words"
	button .say_bye_window.ok -text "Fine... I guess" -command { exit }
	pack .say_bye_window.lab .say_bye_window.ok
	focus .say_bye_window
	lower .
	wm protocol .say_bye_window WM_DELETE_WINDOW { exit }
	vwait forever
}

proc bad_error { err } {
	error "$err. This should not have happened.  Please send detailed information about this error to Evan."
}

proc alert { info } {
	global wait_on_me
	toplevel .alert_window
	wm title .alert_window "Listen up!!"
	label .alert_window.lab -text $info
	button .alert_window.ok -text "Understood, your Majesty." -command { destroy .alert_window}
	pack .alert_window.lab .alert_window.ok
	focus .alert_window
	lower .
}

proc select_file { } {
	global split_fname
	set ftypes [ list [ list "Regular Text Files" [ list .txt ] ] ]
	set t_fname [ tk_getOpenFile -filetypes $ftypes -parent . ]
	if { [ string length $t_fname ] } { set split_fname $t_fname }
}

proc select_dir { } {
	global split_dirname
	set t_dirname [ tk_chooseDirectory -title "Directory for new files" -parent . -mustexist 1 ]
	if { [ string length $t_dirname ] } { set split_dirname $t_dirname }
}

wm title . "Evan's Magical File Splitter"

frame .browse_frame -borderwidth 4 -relief raised
label .browse_frame.file_lab -text "File to split:"
entry .browse_frame.file_entry -width 40 -relief sunken -bd 2 -textvariable split_fname
button .browse_frame.file_browse -text "Browse..." -command { select_file }

label .browse_frame.dir_lab -text "Directory to place new files in:"
entry .browse_frame.dir_entry -width 40 -relief sunken -bd 2 -textvariable split_dirname
button .browse_frame.dir_browse -text "Browse..." -command { select_dir }

frame .frag_sz_frame -borderwidth 4 -relief raised
label .frag_sz_frame.lab -text "Maximum size of fragments:"
entry .frag_sz_frame.entry -width 5 -relief sunken -bd 2 -textvariable fragment_size

menubutton .frag_sz_frame.units -textvariable fsz_units -relief raised -menu .frag_sz_frame.units.menu
menu .frag_sz_frame.units.menu
.frag_sz_frame.units.menu add command -label Bytes -command { set fsz_units Bytes }
.frag_sz_frame.units.menu add command -label Kilobytes -command { set fsz_units Kilobytes }
.frag_sz_frame.units.menu add command -label Megabytes -command { set fsz_units Megabytes }
.frag_sz_frame.units.menu add command -label Phantasmabytes -command { tk_messageBox -title "Silly" -message "Phantasmabytes aren't even real!" }

frame .fuzzy_fsz_frame -borderwidth 4 -relief raised
checkbutton .fuzzy_fsz_frame.button -text "Use fuzzy file size" -variable fuzzy_fsz
label .fuzzy_fsz_frame.explanation -wraplength 400 -text "When fuzzy file size is enabled, an attempt will be made to end the fragment files on a period as opposed to truncating the files strictly at the maximun fragment size."

frame .split_frame -borderwidth 4 -relief raised
button .split_frame.split_button -text "Split the file!" -command { split_file }

pack .browse_frame -fill both
grid .browse_frame.file_lab .browse_frame.file_entry .browse_frame.file_browse
grid .browse_frame.dir_lab .browse_frame.dir_entry .browse_frame.dir_browse

pack .frag_sz_frame -fill both
grid .frag_sz_frame.lab .frag_sz_frame.entry .frag_sz_frame.units

pack .fuzzy_fsz_frame -fill both
pack .fuzzy_fsz_frame.button .fuzzy_fsz_frame.explanation -side top

pack .split_frame -fill both
pack .split_frame.split_button 
