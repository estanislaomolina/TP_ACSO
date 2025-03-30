.text
.text
// Initialize registers
mov X0, #10          // Initial value
mov X2, #20          // Initial value

// Set X1 to a fixed address where we want to jump
// This is a simplification - normally you would calculate this 
// based on your program's memory layout
mov X1, #0x1000      // Set X1 to address where target code is located

// Branch to address in X1
br X1

// These instructions should be skipped
mov X3, #30          // Should be skipped 
mov X4, #40          // Should be skipped

// We can either place target code at 0x1000 or just verify that the BR
// instruction changed the PC based on the value in X1
HLT 0