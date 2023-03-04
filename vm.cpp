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
  LOAD,
  STORE,
  COPY,
  ADD,
  SUB,
  JG
};

int8_t getValue(int8_t instruction, int bit, int widthInBits)
{
  const uint8_t mask = ~(~uint8_t(0) << widthInBits);
  instruction >>= bit;
  return instruction & mask;
}

struct SourceAndDestinations {
  int8_t source;
  int8_t destination;
};
SourceAndDestinations getRegisterOperands(int8_t rawInstruction)
{
  const auto source = getValue(rawInstruction, 1, 2);
  const auto destination = getValue(rawInstruction, 3, 2);
  return { source, destination };
}

void updateFlag(ConditionFlag& conditionalFlag, int8_t registerValue)
{
  if (registerValue > 0) {
    conditionalFlag = ConditionFlag::Positive;
  } else if (registerValue < 0) {
    conditionalFlag = ConditionFlag::Negative;
  } else {
    conditionalFlag = ConditionFlag::Zero;
  }
}

void loadProgramFileIntoMemory(int8_t memory[], const char* pathToProgramFile)
{
  int i = 0;
  std::ifstream file(pathToProgramFile);
  while (!file.eof()) {
    file.get(reinterpret_cast<char&>(memory[i++]));
  }
}

int8_t getRawInstruction(int8_t memory[], int8_t address)
{
  return memory[address];
}

Instruction getOperation(int8_t rawInstruction)
{
  return Instruction(getValue(rawInstruction, 5, 3));
}

int main(int argc, const char** argv)
{
  if (argc != 2) {
    std::cout << "invalid usage" << std::endl;
    return EXIT_FAILURE;
  }

  int8_t memory[1 << 8] = { 0 };
  int8_t registers[1 << 3] = { 0 };
  ConditionFlag conditionalFlag = ConditionFlag::Zero;

  const char* pathToProgramFile = argv[1];
  loadProgramFileIntoMemory(memory, pathToProgramFile);

  while (registers[Register::ProgramControl] < sizeof(memory)) {
    const auto rawInstruction = getRawInstruction(memory, registers[Register::ProgramControl]++);
    const auto operation = getOperation(rawInstruction);
    switch (operation) {
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
    }
  }

  return EXIT_SUCCESS;
}