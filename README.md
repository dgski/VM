# Super Simple Virtual Machine

## Specifications
- 8 Bit Memory Space and Register Size
- 3 General Purpose Registers: R0, R1, R2
- One PC (Program Control) Register

## Instruction Set
- **LOAD x, y**: Load value at memory address stored in register y into register x
- **STORE x, y**: Write value in register y into memory address stored in register x
- **COPY x, y**: Copy value in register y to register x
- **ADD x, y**: Add value in register y to value in register x and store result in register x
- **SUB x, y**: Subtract value in register y from value in register x and store result in register
- **JG x**: Add offset of x to PC if previous instruction result result was greater than 0
- **SYS x**: Executes system call with id x. By convention R0 is used as an argument.
    - **PUTC**: Output character value stored at r0 to stdout.

## TODO
- [] Learning:
    - [x] Clarify how instructions work
- [] Write assembler
    - [] Parser
    - [] Bytecode Emitter
- [] Write VM:
    - [x] Main Loop
    - [x] Registers and Main Memory
    - [] Traps
    - [x] Instructions
