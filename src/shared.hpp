#pragma once

#include <stdexcept>

enum Register {
  R0,
  R1,
  R2,
  PC
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
  PUTC,
  GETC
};

bool isNegative(uint8_t value, int widthInBits)
{
  return uint8_t(1 << (widthInBits - 1)) & value;
}

uint8_t setBit(uint8_t value, int bit)
{
  return value | uint8_t(1 << bit);
}

int8_t getSignedValue(uint8_t instruction, int bit, int widthInBits)
{
  const uint8_t mask = ~(~uint8_t(0) << widthInBits);
  instruction >>= bit;
  int8_t value = instruction & mask;

  if (isNegative(value, widthInBits)) {
    value |= (~uint8_t(0) << widthInBits);
  }

  return value;
}

uint8_t getValue(uint8_t instruction, int bit, int widthInBits)
{
  const uint8_t mask = ~(~uint8_t(0) << widthInBits);
  instruction >>= bit;
  uint8_t value = instruction & mask;
  return value;
}

uint8_t setValue(uint8_t value, int bit, int widthInBits)
{
  const uint8_t mask = (~(~uint8_t(0) << widthInBits) << bit);
  value <<= bit;
  value &= mask;
  return value;
}

uint8_t setSignedValue(int8_t value, int bit, int widthInBits)
{
  return setValue(value, bit, widthInBits);
}

uint8_t setInstruction(uint8_t instruction)
{
  return setValue(instruction, 5, 3);
}

uint8_t setRegisters(uint8_t destination, uint8_t source)
{
  return setValue(destination, 3, 2) | setValue(source, 1, 2);
}

Register getRegister(std::string_view name)
{
  if (name == "R0") {
    return Register::R0;
  } else if (name == "R1") {
    return Register::R1;
  } else if (name == "R2") {
    return Register::R2;
  } else if (name == "PC") {
    return Register::PC;
  }
  
  throw std::runtime_error("invalid register name");
}