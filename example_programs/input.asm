READ .ONE
JG START
.ONE 1
.ZERO 0
.PROMPT 62
START
    LOOP
        READ .PROMPT
        SYS PUTC
        SYS GETC
        COPY R1 R0
        .TERMINATION -121
        .TERMINATION_ADDRESS .TERMINATION
        READ .TERMINATION_ADDRESS
        LOAD R0 R0
        ADD R0 R1
        JG LOOP
        COPY R1 R0
        .INC 1
        READ .INC
        ADD R0 R1
        JG END
        READ .INC
        JG LOOP
END
    .D 68
    READ .D
    SYS PUTC
    .O 79
    READ .O
    SYS PUTC
    .N 78
    READ .N
    SYS PUTC
    .E 69
    READ .E
    SYS PUTC
    .NEWLINE 10
    READ .NEWLINE
    SYS PUTC
SYS HALT