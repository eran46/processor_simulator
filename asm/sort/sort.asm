.word 256 45
.word 257 -12
.word 258 23
.word 259 0
.word 260 67
.word 261 -34
.word 262 88
.word 263 13
.word 264 -7
.word 265 56
.word 266 19
.word 267 5
.word 268 29
.word 269 1
.word 270 -8
.word 271 99
			add $t0, $zero, $zero, 0			# i = 0
			add $t1, $zero, $imm, 15			# for 15 iterations (size-1)
loop_start:			
			bge $imm, $t0, $t1, loop_end		# if i >= 15, jump to loop_end 
			add $s0, $zero, $imm, 0x100			# j = 256 (start address)
			sub $t2, $imm, $t0, 0x10f			# calculate end address for 15 - i iterations
inner_loop:
			bge $imm, $s0, $t2, next_iter		# if j >= 0x10f - i, jump to next_iter
			lw $a0, $s0, $zero, 0				# $a0 = a[j]
			lw $a1, $s0, $imm, 1				# $a1 = a[j+1]
			ble $imm, $a0, $a1, skip_swap		# if a[j] <= a[j+1], jump to skip_swap
			sw $a0, $s0, $imm, 1				# a[j+1] = $a0
			sw $a1, $s0, $zero, 0				# a[j] = $a1
skip_swap:
			add $s0, $s0, $imm, 1				# j++
			beq $imm, $zero, $zero, inner_loop	# jump to inner_loop
next_iter:
			add $t0, $t0, $imm, 1				# i++
			beq $imm, $zero, $zero, loop_start	# jump to loop_start
loop_end:
			halt $zero, $zero, $zero, 0			# halt
