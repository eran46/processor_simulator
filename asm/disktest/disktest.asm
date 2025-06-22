add $a0,$zero,$imm,2048
add $s0,$zero,$zero,0		#
out $s0,$zero,$imm,15		# disksector = 0
add $s1,$zero,$a0,0		#
out $s1,$zero,$imm,16		# diskbuffer = 2048
add $s2,$zero,$imm,8		# stop read condition
add $t0,$zero,$imm, 1		#
out $t0,$zero,$imm,1		# irq1enable = 1		#
out $t0,$zero,$imm,14		# diskcmd = 1 (read)

add $s0,$s0,$imm,1		# $s0++ (disksector counter)
add $s1,$s1,$imm,128		# $s1+= 128 (diskbuffer)

wait_loop1:
in $t2,$zero,$imm,17		# $t2 = diskstatus
bne $imm,$t2,$zero,wait_loop1	# if diskstatus != 0 keep waiting

disk_read_loop:
bge $imm,$s0,$s2,calculate_sum	# while($s0 < $s2)
out $s0,$zero,$imm,15		# disksector = $s0
out $s1,$zero,$imm,16		# diskbuffer = $s1
add $t0,$zero,$imm, 1		# 
out $t0,$zero,$imm,14		# diskcmd = 1
add $s0,$s0,$imm,1		# $s0++ (disksector counter)
add $s1,$s1,$imm,128		# $s1+= 128 (diskbuffer)
beq $imm,$zero,$zero,wait_loop1

calculate_sum:
add $a1,$zero,$imm,2048		#
add $a0,$zero,$zero,0 		# writing memory offset
add $t0,$zero,$zero,0		# sector offset
add $t1,$zero,$zero,0		# counter

lw $s1,$a1,$t0,0		# s1 = mem[a1=2048 + t0=0] 45-46
add $a2, $zero, $s1		# $a2 is the sum
add $t0,$t0,$imm,128		# t0+=128
add $t1,$t1,$imm,1		# t1++

for1:
for2:
lw $s1,$a1,$t0,0		# 53
add $a2,$a2,$s1,0		
add $t0,$t0,$imm,128
add $t1,$t1,$imm,1
blt $imm,$t1,$s2,for2

add $t0,$zero,$zero,0 		# reset t0
add $t1,$zero,$zero,0		# reset t1
sw $a2,$imm,$a0,3072		# store sum
add $a2,$zero,$zero,0		# reset sum
add $a0,$a0,$imm,1		# increment writing memory offset
add $s1,$zero,$imm,128		# for comparison
add $a1,$a1,$imm,1		# a1++
blt $imm,$a0,$s1,for1

disk_write:
add $s0,$zero,$imm,8		#
out $s0,$zero,$imm,15		# disksector = 8
add $s1,$zero,$imm,3072		#
out $s1,$zero,$imm,16		# diskbuffer = 3072
add $t0,$zero,$imm,2
out $t0,$zero,$imm,14		# diskcmd = 2 (write)

wait_loop2:
in $t2,$zero,$imm,17		# $t2 = diskstatus
bne $imm,$t2,$zero,wait_loop2

halt $zero,$zero,$zero,0

irq1:
add $a3,$zero,$zero,0		# (finalize DMA in the background)
out $a3,$zero,$imm,4		# irq1status = 0
reti $zero,$zero,$zero,0 




