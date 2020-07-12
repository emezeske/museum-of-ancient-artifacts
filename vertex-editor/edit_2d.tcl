#/usr/bin/wish

set screen_w 800
set screen_h 600
set image_file big-monster.gif
set status_text ""
set sel_seg -1

pack [ canvas .c -width $screen_w -height $screen_h -bg white ]
image create photo tex -file $image_file
.c create image [ expr $screen_w / 2 ] [ expr $screen_h / 2 ] -image tex -tag bg_image
label .status_bar -textvariable status_text

grid .c -row 0 -column 0 -sticky nswe
grid .status_bar -row 1 -column 0 -sticky ns
grid rowconfig . 0 -weight 1
grid columnconfig . 0 -weight 1

set vertices {}

proc add_vertex { canvas x y } {
	global vertices 

	set num_verts [ llength $vertices ]

	if { $num_verts > 0 } {

		set last_vert [ lindex $vertices end ]

		$canvas create line 	[ lindex $last_vert 0 ] [ lindex $last_vert 1 ] $x $y \
					-fill red -width 2.0 -tag line$num_verts -arrow last

		if { $num_verts > 1 } { $canvas itemconfigure line[ expr $num_verts - 1 ] -arrow none }

		$canvas lower line$num_verts
		$canvas raise line$num_verts bg_image
	}

	lappend vertices [ list $x $y ]

	$canvas create rectangle 	[ expr $x - 2 ] [ expr $y - 2 ] \
					[ expr $x + 2 ] [ expr $y + 2 ] \
					-tag vertex$num_verts

	deselect_vertex $canvas $num_verts
}

proc save { file } {
	global vertices image_file

	if { ![ catch { open $file w } fd ] } {
		puts $fd "#image section"
		puts $fd "#-----------------"
		puts $fd $image_file
		puts $fd ""
		puts $fd "#vertices section"
		puts $fd "#\tx\ty"
		puts $fd "#-----------------"
		set i 0
		foreach v $vertices { puts $fd "$i\t[ lindex $v 0 ]\t[ lindex $v 1 ]" ; incr i}
		close $fd
	} else { error "Couldn't open $file for writing: $fd" }
}

# Binding to check if anything is selected
bind .c <Motion> {
	do_selection %W [ %W canvasx %x ] [ %W canvasy %y ] 
}

# Binding for exit
bind . <KeyPress-Escape> exit

# Save the info to file
bind . <Control-KeyPress-s> { save vertices.txt }

# Bindings for panning via middle mouse button
bind .c <ButtonPress-2> {%W scan mark %x %y}
bind .c <B2-Motion> {%W scan dragto %x %y 1}

# Bindings for adding vertices
bind .c <ButtonPress-1> {
	if { $sel_seg == -1 } {
		add_vertex %W [ %W canvasx %x ] [ %W canvasy %y ]
		do_selection %W [ %W canvasx %x ] [ %W canvasy %y ]
	}
}

bind .c <B1-Motion> {
	if { $sel_seg != -1 } { move_selection %W [ %W canvasx %x ] [ %W canvasy %y ] }
}

bind .c <ButtonPress-3> {
	if { $sel_seg != -1 } { delete_vertex %W $sel_seg }
}

# Bindings to update the status bar when input is taken from the mouse
set status_update {+ set status_text "image: $image_file\tx: [ %W canvasx %x ]\ty: [ %W canvasy %y ]\tvertices: [ llength $vertices ]" }
bind .c <Motion> $status_update
bind .c <ButtonPress> $status_update

proc delete_vertex { canvas n } {
	global vertices
}

proc select_vertex { canvas n } {
	$canvas itemconfigure vertex$n -fill blue -outline blue -width 4.0
}

proc deselect_vertex { canvas n } {
	$canvas itemconfigure vertex$n -fill green -outline green -width 2.0
}

proc move_selection { canvas x y } {
	global sel_seg vertices

	set prev_seg [ expr $sel_seg - 1 ]
	set next_seg [ expr $sel_seg + 1 ]

	set prev_vert [ lindex $vertices $prev_seg ]
	set next_vert [ lindex $vertices $next_seg ]

	$canvas coords vertex$sel_seg [ expr $x - 2 ] [ expr $y - 2 ] [ expr $x + 2 ] [ expr $y + 2 ]

	if { $prev_vert != "" } { $canvas coords line$sel_seg [ lindex $prev_vert 0 ] [ lindex $prev_vert 1 ] $x $y }
	if { $next_vert != "" } { $canvas coords line$next_seg $x $y [ lindex $next_vert 0 ] [ lindex $next_vert 1 ] }

	set vertices [ lreplace $vertices $sel_seg $sel_seg [ list $x $y ] ]
}

proc do_selection { canvas x y } {
	global sel_seg

	if { $sel_seg != -1 } { 

		deselect_vertex $canvas $sel_seg
		set sel_seg -1
	}

	set id_tags {}
	set ids [ $canvas find overlapping [ expr $x - 0 ] [ expr $y - 0 ] [ expr $x + 0 ] [ expr $y + 0 ] ]
	foreach id $ids { lappend id_tags [ $canvas gettags $id ] }

	foreach tags $id_tags {

		foreach t $tags {

			if { [ regexp {^vertex([0-9]+)} $t junk n ] == 1 } {

				set sel_seg $n
				select_vertex $canvas $n
				return
			}
		}
	}
}
