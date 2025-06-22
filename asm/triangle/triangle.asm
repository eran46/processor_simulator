					add $t1, $zero, $imm, 255				# Set $t1 to 255 (white color)
					add $t0, $zero, $imm, 1					# Set $t0 to 1 (to trigger drawing)
					
					add $a0, $zero, $zero, 0				# Initialize dy to 0 (used to be $s0)
					add $a1, $zero, $zero, 0				# Initialize dx to 0 (used to be $s1)

					lw $s0, $imm, $zero, 256				# Load address of A (frame buffer) into $s0 (was $a0)
					add $a3, $s0, $zero, 0					# Set $a3 to address of A

					lw $s1, $imm, $zero, 257				# Load address of B (frame buffer) into $s1 (was $a1)
					lw $s2, $imm, $zero, 258				# Load address of C (frame buffer) into $s2 (was $a2)
					
dy:
					beq $imm, $a3, $s1, dx				    # If $a3 equals B’s address ($s1), jump to dx
					add $a3, $a3, $imm, 256					# Move to next row (y = y + 1)
					add $a0, $a0, $imm, 1					# Increment dy (was $s0)
					beq $imm, $zero, $zero, dy			    # Loop until $a3 equals B’s address
dx:
					beq $imm, $a3, $s2, initialize_fill		# If $a3 equals C’s address ($s2), jump to initialize_fill
					add $a3, $a3, $imm, 1					# Move to next column (x = x + 1)
					add $a1, $a1, $imm, 1					# Increment dx (was $s1)
					beq $imm, $zero, $zero, dx			    # Loop until $a3 equals C’s address
initialize_fill:
					add $a3, $s0, $zero, 0					# Reset $a3 to A’s address ($s0)
					add $v0, $a3, $zero, 0					# Start filling at A
					blt $imm, $a0, $a1, shallow_slope		# If dy < dx, go to shallow_slope (was $s0 and $s1)
					mul $a2, $a1, $imm, 2					# Calculate 2 * dx (was $s2)
					sub $a2, $a2, $a0, 0					# 2 * dx - dy (D) according to Beresenham (was $s0 and $s2)
draw_steep_lines:
					
					ble $imm, $a2, $zero, draw_next_line	# If D <= 0, go to draw_next_line
					add $v0, $v0, $imm, 1					# Move right (v0++)
					mul $t2, $a0, $imm, 2					# Calculate 2 * dy (was $s0)
					sub $a2, $a2, $t2, 0					# D = D - 2 * dy (was $s2)			
draw_next_line:					
					mul $t2, $a1, $imm, 2					# Add 2 * dx to D (was $s1)
					add $a2, $a2, $t2, 0					# Update D (was $s2)
					add $v0, $v0, $imm, 256					# Move down to next row (v0 = v0 + 256)
					add $a3, $a3, $imm, 256					# Move $a3 to the next row
					add $t2, $a3, $zero, 0					# Set starting point 
fill_steep_lines:		
					beq $imm, $t2, $v0, draw_steep_lines	# If reached end of row, go to draw_steep_lines
					out $t2, $zero, $imm, 20				# Set pixel address
					out $t1, $zero, $imm, 21				# Set pixel color to 255
					out $t0, $zero, $imm, 22				# Trigger the pixel draw
					add $t2, $t2, $imm, 1					# Move to next pixel (t2++)
					beq $imm, $t2, $s2, end					# If reached the last pixel, go to end
					beq $imm, $zero, $zero, fill_steep_lines # Repeat for the entire row

shallow_slope:  
					mul $a2, $a0, $imm, 2					# Calculate 2 * dy (was $s2 and $s0)
					sub $a2, $a2, $a1, 0					# 2 * dy - dx (D) (was $s2 and $s1)
draw_shallow_lines:
					
					ble $imm, $a2, $zero, move_to_next_line	# If D <= 0, go to move_to_next_line
					add $t2, $a3, $zero, 0					# Set starting point 
fill_shallow_lines:		
					beq $imm, $t2, $v0, more_shallow_lines	# If reached end of row, go to more_shallow_lines
					out $t2, $zero, $imm, 20				# Set pixel address
					out $t1, $zero, $imm, 21				# Set pixel color to 255
					out $t0, $zero, $imm, 22				# Trigger the pixel draw
					add $t2, $t2, $imm, 1					# Move to next pixel (t2++)
					beq $imm, $t2, $s2, end					# If reached the last pixel, go to end
					beq $imm, $zero, $zero, fill_shallow_lines # Repeat for the entire row
more_shallow_lines:
					mul $t2, $a1, $imm, 2					# Add 2 * dx to D (was $s1)
					sub $a2, $a2, $t2, 0					# Update D (was $s2)
					add $a3, $a3, $imm, 256					# Move $a3 to the next row
					add $v0, $v0, $imm, 256					# Move down to next row (v0 = v0 + 256)
move_to_next_line:					
					mul $t2, $a0, $imm, 2					# Add 2 * dy to D (was $s0)
					add $a2, $a2, $t2, 0					# Update D (was $s2)
					add $v0, $v0, $imm, 1					# Move right (v0++)
					beq $imm, $zero, $zero, draw_shallow_lines # Repeat until last line

end:
					in $t0, $zero, $imm, 22					# Read monitorcmd (reset state)
					halt $zero, $zero, $zero, 0				# Stop execution
					.word 256 100
					.word 257 25700
					.word 258 25800