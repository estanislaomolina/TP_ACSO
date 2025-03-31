movz x0, #10

loop_start:
    subs x0, x0, #1
    cbz x0, loop_end
    b loop_start

movz x1, #4

loop_end:
HLT 0
