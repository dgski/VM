#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

enum class ConditionFlag {
  Negative,
  Zero,
  Positive
};

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
  SUB,
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

struct SourceAndDestinations {
  uint8_t source;
  uint8_t destination;
};
SourceAndDestinations getRegisterOperands(uint8_t rawInstruction)
{
  const auto destination = getValue(rawInstruction, 1, 2);
  const auto source = getValue(rawInstruction, 3, 2);
  return { source, destination };
}

void updateFlag(ConditionFlag& conditionalFlag, uint8_t registerValue)
{
  if (registerValue > 0) {
    conditionalFlag = ConditionFlag::Positive;
  } else if (registerValue < 0) {
    conditionalFlag = ConditionFlag::Negative;
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
  }
}

void handleNextInstruction(bool& running, uint8_t memory[], uint8_t registers[], ConditionFlag& conditionalFlag)
{
  const auto rawInstruction = getRawInstruction(memory, registers[Register::ProgramControl]++);
  const auto operation = getOperation(rawInstruction);
  switch (operation) {
  case READ: {
    const auto destination = getValue(rawInstruction, 3, 2);
    const auto offset = getValue(rawInstruction, 0, 3);
    registers[destination] = memory[registers[Register::ProgramControl] + offset];
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
    registers[destination] += registers[source];
    updateFlag(conditionalFlag, registers[destination]);
    break;
  }
  case SUB: {
    const auto [source, destination] = getRegisterOperands(rawInstruction);
    registers[destination] -= registers[source];
    updateFlag(conditionalFlag, registers[destination]);
    break;
  }
  case JG: {
    const auto programControlOffset = getValue(rawInstruction, 0, 5);
    if (conditionalFlag == ConditionFlag::Positive) {
      registers[Register::ProgramControl] += programControlOffset;
    }
    break;
  }
  case SYS: {
    handleSystemCall(running, registers[Register::R0], rawInstruction);
    break;
  }
  }
}

int main(int argc, const char** argv)
{
  if (argc != 2) {
    std::cout << "invalid usage" << std::endl;
    return EXIT_FAILURE;
  }

  uint8_t memory[1 << 8] = { 0 };
  uint8_t registers[1 << 3] = { 0 };
  ConditionFlag conditionalFlag = ConditionFlag::Zero;

  const char* pathToProgramFile = argv[1];
  loadProgramFileIntoMemory(memory, pathToProgramFile);

  bool running = true;
  while (running) {
    handleNextInstruction(running, memory, registers, conditionalFlag);
  }

  return EXIT_SUCCESS;
}