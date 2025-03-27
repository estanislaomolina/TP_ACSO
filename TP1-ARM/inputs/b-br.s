.text
mov X5, [foo]
br X5
adds X2, X0, 10

bar:
HLT 0         

foo:
cmp X11, X11 
beq bar
adds X3, X0, 10 
HLT 0 