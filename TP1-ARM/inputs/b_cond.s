.text

// Test BEQ (Branch if Equal)
cmp X1, X1       // Compare X1 with itself (equal)
beq label_eq     // Should branch to label_eq
adds X2, X0, 10  // Should not execute if branch works

label_eq:
cmp X1, X2       // Compare X1 with X2 (not equal)
bne label_ne     // Should branch to label_ne
adds X3, X0, 20  // Should not execute if branch works

label_ne:
cmp X3, X2       // Compare X3 with X2 (greater than)
bgt label_gt     // Should branch to label_gt
adds X4, X0, 30  // Should not execute if branch works

label_gt:
cmp X2, X3       // Compare X2 with X3 (less than)
blt label_lt     // Should branch to label_lt
adds X5, X0, 40  // Should not execute if branch works

label_lt:
cmp X3, X3       // Compare X3 with itself (greater or equal)
bge label_ge     // Should branch to label_ge
adds X6, X0, 50  // Should not execute if branch works

label_ge:
cmp X2, X3       // Compare X2 with X3 (less or equal)
ble label_le     // Should branch to label_le
adds X7, X0, 60  // Should not execute if branch works

label_le:
HLT 0            // End of program
