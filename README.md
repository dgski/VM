# Super Simple Virtual Machine And Assembler

## VM Specifications
- 8 Bit Memory Space and Register Size
- 3 General Purpose Registers: R0, R1, R2
- One PC (Program Control) Register
- 16 Address Jump/Read Limitation

## Instruction Set
- **READ x, y**: Load value at memory offset y (from currency Program Control) into r0.
- **LOAD x, y**: Load value at memory address stored in register y into register x (Indirect)
- **STORE x, y**: Write value in register y into memory address stored in register x (Indirect)
- **COPY x, y**: Copy value in register y to register x
- **ADD x, y**: Add value in register y to value in register x and store result in register x
- **JG x**: Add offset of x to PC if previous instruction result result was greater than 0
- **SYS x**: Executes system call with id x. By convention R0 is used as an argument.
    - **HALT**: Stops the program
    - **PUTC**: Output character value stored at r0 to stdout.
    - **GETC**: Read one character from stdin and store in r0.

## Assembler Features

- Single file only.
- Labels to reference memory locations.
- In-line data prefixed with '.'.
- Ability to reference other data locations by creating a named datum with value of address of other datum.