	add $r2, $r2, $imm, 1		# PC=0
	add $r3, $r3, $imm, 1		# PC=1
	add $r4, $r4, $imm, 1		# PC=2
	blt $zero, $r2, $imm, 100	# PC=3
	add $r5, $r5, $imm, 1		# PC=4
	halt $zero, $zero, $zero, 0	# PC=5
	halt $zero, $zero, $zero, 0	# PC=6
	halt $zero, $zero, $zero, 0	# PC=7
	halt $zero, $zero, $zero, 0	# PC=8
	halt $zero, $zero, $zero, 0	# PC=9
