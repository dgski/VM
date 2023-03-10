READ .MEMORY_START_ADDRESS
JG MAIN

.MEMORY_START_ADDRESS .MEMORY
.ONE 1
.MINUS -1
.ZERO 0
.CHAR 65

MAIN
    JG MAIN_BODY
    .MAIN_SKIP 1
    .MAIN_FUNC_CALL_OFFSET 5
    MAIN_BODY
        COPY R2 R0
        COPY R1 PC
        READ .MAIN_FUNC_CALL_OFFSET
        ADD R1 R0
        STORE R2 R1
        JG FUNC
        READ .MAIN_SKIP
        SYS HALT

FUNC
    READ .FUNC_ONE
    JG FUNC_BODY
    .FUNC_ONE 1
    .FUNC_MINUS -1
    .FUNC_FUNC2_CALL_OFFSET 5
    FUNC_BODY
        READ .FUNC_ONE
        ADD R2 R0
        COPY R1 PC
        READ .FUNC_FUNC2_CALL_OFFSET
        ADD R1 R0
        STORE R2 R1
        READ .FUNC_ONE
        JG FUNC2
        READ .FUNC_MINUS
        ADD R2 R0
        LOAD PC R2

FUNC2
    READ .FUNC2_SKIP
    JG FUNC2_BODY
    .FUNC2_SKIP 1
    .TWO 50
    FUNC2_BODY
        READ .TWO
        SYS PUTC
        LOAD PC R2

.MEMORY 1