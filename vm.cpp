#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "shared.hpp"

constexpr auto TOTAL_MEMORY_ADDRESSES = (1 << 8);
constexpr auto TOTAL_REGISTER_COUNT = (1 << 2);

enum class ConditionFlag {
  Negative,
  Zero,
  Positive
};

struct SourceAndDestinations {
  uint8_t source;
  uint8_t destination;
};
SourceAndDestinations getRegisterOperands(uint8_t rawInstruction)
{
  const auto source = getValue(rawInstruction, 1, 2);
  const auto destination = getValue(rawInstruction, 3, 2);
  return { source, destination };
}

void updateFlag(ConditionFlag& conditionalFlag, uint8_t registerValue)
{
  const uint8_t negativeBitMask = 0b10000000;
  if (registerValue & negativeBitMask) {
    conditionalFlag = ConditionFlag::Negative;
  } else if (registerValue != 0) {
    conditionalFlag = ConditionFlag::Positive;
  } else {
    conditionalFlag = ConditionFlag::Zero;
  }
}

void loadProgramFileIntoMemory(uint8_t memory[], const char* pathToProgramFile)
{
  int i = 0;
  std::ifstream file(pathToProgramFile);
  while (!file.eof()) {
    file.get(reinterpret_cast<char&>(memory[i++]));
  }
}

uint8_t getRawInstruction(uint8_t memory[], uint8_t address)
{
  return memory[address];
}

Instruction getOperation(uint8_t rawInstruction)
{
  return Instruction(getValue(rawInstruction, 5, 3));
}

SystemCalls getSystemCall(uint8_t rawInstruction)
{
  return SystemCalls(getValue(rawInstruction, 0, 5));
}

void handleSystemCall(bool& running, uint8_t& r0, uint8_t rawInstruction)
{
  const auto systemCall = getSystemCall(rawInstruction);
  switch (systemCall) {
  case HALT: {
    running = false;
    break;
  }
  case PUTC: {
    std::cout << r0;
    break;
  }
  case GETC: {
    std::cin >> r0;
    break;
  }
  }
}

void dumpState(uint8_t memory[], uint8_t registers[], ConditionFlag& conditionalFlag)
{
  std::cout
    << "["
    << " R0=" << int16_t(registers[Register::R0])
    << " R1=" << int16_t(registers[Register::R1])
    << " R2=" << int16_t(registers[Register::R2])
    << " PC=" << int16_t(registers[Register::ProgramControl])
    << " ]"
    << std::endl;
}

void handleNextInstruction(
  bool& running, uint8_t memory[], uint8_t registers[], ConditionFlag& conditionalFlag)
{
  const auto rawInstruction = getRawInstruction(memory, registers[Register::ProgramControl]++);
  const auto operation = getOperation(rawInstruction);

  switch (operation) {
  case READ: {
    const int8_t offset = getSignedValue(rawInstruction, 0, 5) - 1;
    registers[Register::R0] = memory[registers[Register::ProgramControl] + offset];
    updateFlag(conditionalFlag, registers[Register::R0]);
    break;
  }
  case LOAD: {
    const auto [source, destination] = getRegisterOperands(rawInstruction);
    const auto sourceMemoryAddress = registers[source];
    registers[destination] = memory[sourceMemoryAddress];
    updateFlag(conditionalFlag, registers[destination]);
    break;
  }
  case STORE: {
    const auto [source, destination] = getRegisterOperands(rawInstruction);
    const auto destinationMemoryAddress = registers[destination];
    memory[destinationMemoryAddress] = registers[source];
    break;
  }
  case COPY: {
    const auto [source, destination] = getRegisterOperands(rawInstruction);
    registers[destination] = registers[source];
    updateFlag(conditionalFlag, registers[destination]);
    break;
  }
  case ADD: {
    const auto [source, destination] = getRegisterOperands(rawInstruction);
    registers[destination] += int8_t(registers[source]);
    updateFlag(conditionalFlag, registers[destination]);
    break;
  }
  case JG: {
    const int8_t programControlOffset = getSignedValue(rawInstruction, 0, 5) - 1;
    if (conditionalFlag == ConditionFlag::Positive) {
      registers[Register::ProgramControl] += programControlOffset;
    }
    break;
  }
  case SYS: {
    handleSystemCall(running, registers[Register::R0], rawInstruction);
    updateFlag(conditionalFlag, registers[Register::R0]);
    break;
  }
  }
}

int main(int argc, const char** argv)
{
  if (argc != 2) {
    std::cout << "usage: ./vm {pathToProgramFile}" << std::endl;
    return EXIT_FAILURE;
  }

  uint8_t memory[TOTAL_MEMORY_ADDRESSES] = { 0 };
  uint8_t registers[TOTAL_REGISTER_COUNT] = { 0 };
  ConditionFlag conditionalFlag = ConditionFlag::Zero;

  const char* pathToProgramFile = argv[1];
  loadProgramFileIntoMemory(memory, pathToProgramFile);

  bool running = true;
  while (running) {
    handleNextInstruction(running, memory, registers, conditionalFlag);
  }

  return EXIT_SUCCESS;
}