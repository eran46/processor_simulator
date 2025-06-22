        add $v0, $zero, $zero, 0          # set $v0 to 0
        add $sp, $zero, $imm, 1500        # initialize $sp to 1500
.word 256 6
.word 257 3
        lw $t0, $zero, $imm, 0x100        # load n from address 0x100
        lw $t1, $zero, $imm, 0x101        # load k from address 0x101

        jal $ra, $imm, $zero, calc_binom  # compute $v0 = binom(n,k)
        sw $v0, $zero, $imm, 0x102        # store result at address 0x102
        halt $zero, $zero, $zero, 0       # stop execution

calc_binom:
        sub $sp, $sp, $imm, 3             # reserve space on stack for 3 items
        sw $t0, $sp, $imm, 0              # push argument n to stack
        sw $t1, $sp, $imm, 1              # push argument k to stack
        sw $ra, $sp, $imm, 2              # push return address to stack
        bne $imm, $t1, $zero, k_nonzero   # if k != 0, branch to k_nonzero
        add $v0, $v0, $imm, 1             # if k == 0, increment result
        beq $imm, $zero, $zero, return_result # branch to return_result

k_nonzero:
        bne $imm, $t1, $t0, k_not_equal_n # if k != n, branch to k_not_equal_n
        add $v0, $v0, $imm, 1             # if k == n, increment result
        beq $imm, $zero, $zero, return_result # branch to return_result

k_not_equal_n:
        sub $t0, $t0, $imm, 1             # decrement n by 1
        jal $ra, $imm, $zero, calc_binom  # recursive call binom(n-1, k)
        sub $t1, $t1, $imm, 1             # decrement k by 1
        jal $ra, $imm, $zero, calc_binom  # recursive call binom(n-1, k-1)

return_result:
        lw $ra, $sp, $imm, 2              # restore return address from stack
        lw $t1, $sp, $imm, 1              # restore k from stack
        lw $t0, $sp, $imm, 0              # restore n from stack
        add $sp, $sp, $imm, 3             # clean up the stack
        beq $ra, $zero, $zero, 0          # return from function
