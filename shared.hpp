#pragma once

enum Register {
  R0,
  R1,
  R2,
  ProgramControl
};

enum Instruction {
  READ,
  LOAD,
  STORE,
  COPY,
  ADD,
  JG,
  SYS
};

enum SystemCalls {
  HALT,
  PUTC
};

uint8_t getValue(uint8_t instruction, int bit, int widthInBits)
{
  const uint8_t mask = ~(~uint8_t(0) << widthInBits);
  instruction >>= bit;
  return instruction & mask;
}

uint8_t setValue(uint8_t value, int bit, int widthInBits)
{
  return value << bit; // TODO: safety?
}

uint8_t setInstruction(uint8_t instruction)
{
  return setValue(instruction, 5, 3);
}

uint8_t setRegisters(uint8_t destination, uint8_t source)
{
  return setValue(destination, 3, 2) | setValue(source, 0, 2);
}

Register getRegister(std::string_view name)
{
  if (name == "R0") {
    return Register::R0;
  } else if (name == "R1") {
    return Register::R1;
  } else if (name == "R2") {
    return Register::R2;
  } else {
    return Register::ProgramControl;
  }
}