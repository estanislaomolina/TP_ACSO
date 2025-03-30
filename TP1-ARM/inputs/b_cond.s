.text

// Test BEQ (Branch if Equal)
adds X3, X1, 5
cmp X1, X1       // Compare X1 with itself (equal)
beq label_eq     // Should branch to label_eq
adds X2, X0, 10  // Should not execute if branch works

label_eq:
cmp X1, X3
beq label_ne1 // Should continue
adds X30, X0, 20

label_ne1:
cmp X1, X1       // Compare X1 with X2 (not equal)
bne label_ne2     // Should continue
adds X30, X0, 20  

label_ne2:
cmp X1, X3
bne label_gt1
adds X30, X0, 20 // Should not execute

label_gt1:
cmp X3, X2       // Compare X3 with X2 (greater than)
bgt label_gt2     // Should branch to label_gt2
adds X30, X0, 20  // Should not execute if branch works

label_gt2:
cmp X2, X3       // Compare X2 with X3 (greater than)
bgt label_lt1    // Should not branch to label_lt1
adds X30, X0, 20  // Should not execute if branch works

label_lt1:
cmp X1, X3
blt label_lt2
adds X30, X0, 20 // Should not execute

label_lt2:
cmp X3, X1
blt label_ge1
adds X30, X0, 20 // Should execute

label_ge1:
cmp X3, X1  // Compare X3 with X1 (greater or equal)
bge label_bge2  // Should branch if X3 >= X1
adds X30, X0, 20 // Should not execute if branch works

label_bge2:
cmp X1, X3  // Compare X1 with X3 (greater or equal)
bge label_ble1  // Should not branch if X1 < X3
adds X30, X0, 20 // Should not execute if branch works

label_ble1:
cmp X1, X3  // Compare X1 with X3 (less or equal)
ble label_ble2  // Should branch if X1 <= X3
adds X30, X0, 20 // Should not execute if branch works

label_ble2:
cmp X3, X1  // Compare X3 with X1 (less or equal)
ble end_test    // Should not branch if X3 > X1
adds X30, X0, 20 // Should execute

end_test:
HLT 0
