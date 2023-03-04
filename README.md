# Super Simple Virtual Machine

## Specifications
- 8 Bit Memory Space and Register Size
- 3 General Purpose Registers: R0, R1, R2
- One PC (Program Control) Register

## Instruction Set
- **READ x, y**: Load value at memory offset y (from currency Program Control) into r0.
- **LOAD x, y**: Load value at memory address stored in register y into register x
- **STORE x, y**: Write value in register y into memory address stored in register x
- **COPY x, y**: Copy value in register y to register x
- **ADD x, y**: Add value in register y to value in register x and store result in register x
- **JG x**: Add offset of x to PC if previous instruction result result was greater than 0
- **SYS x**: Executes system call with id x. By convention R0 is used as an argument.
    - **HALT**: Stops the program
    - **PUTC**: Output character value stored at r0 to stdout.

## TODO
- [x] Learning:
    - [x] Clarify how instructions work
    - [] How to implement negatives
- [] Write assembler
    - [] Parser
    - [] Bytecode Emitter
- [x] Write VM:
    - [x] Main Loop
    - [x] Registers and Main Memory
    - [x] Traps
    - [x] Instructions
    - [] Improve Ergonomics Ideas:
        - Alter READ instruction to always write into r0:
            - Gain 2 more bits for 5 total: 32 addresses away
        - Expand number of instruction bits to 4: 16 memory addresses
