#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "shared.hpp"

struct SplitLine {
  std::string_view one;
  std::string_view two;
  std::string_view three;
};
SplitLine splitLine(std::string_view line, char delimeter)
{
  SplitLine result;

  const auto getNextToken = [](std::string_view& output, std::string_view& input) {
    if (input.empty()) {
      return;
    }

    if (const auto firstSpaceIndex = input.find(' '); firstSpaceIndex == std::string::npos) {
      output = input;
      input = std::string_view();
    } else {
      output = std::string_view(input.data(), firstSpaceIndex);
      input.remove_prefix(firstSpaceIndex + 1);
    }
  };

  getNextToken(result.one, line);
  getNextToken(result.two, line);
  getNextToken(result.three, line);
  return result;
}

bool isInstruction(std::string_view line)
{
  return (line == "READ") || (line == "LOAD") || (line == "STORE") || (line == "COPY") || (line == "ADD") || (line == "JG") || (line == "SYS");
}

bool isData(std::string_view line)
{
  return line.front() == '.';
}

bool isLabel(std::string_view line)
{
  return !isInstruction(line) && !isData(line);
}

struct LabelsAndData {
  std::unordered_map<std::string, int> labelToAddress;
  std::unordered_map<std::string, int> dataToAddress;
};
LabelsAndData getLabelsAndData(const char* inputAssemblyFilePath)
{
  LabelsAndData result;

  int currentMemoryAddress = 0;
  std::string line;
  std::ifstream assemblyFileStream(inputAssemblyFilePath);
  while (std::getline(assemblyFileStream, line)) {
    if (line.empty()) {
      continue;
    }

    if (auto [one, two, three] = splitLine(line, ' '); isInstruction(one)) {
      ++currentMemoryAddress;
      continue;
    } else if (isData(one)) {
      result.dataToAddress[std::string(one)] = currentMemoryAddress++;
    } else /*isLabel*/ {
      result.labelToAddress[std::string(one)] = ++currentMemoryAddress;
    }
  }

  return result;
}

uint8_t createInstruction(int currentMemoryAddress, std::string_view instruction, std::string_view operandOne, std::string_view operandTwo, const LabelsAndData& labelsAndData)
{
  std::cout << "instruction=" << instruction << " operandOne=" << operandOne << " operandTwo=" << operandTwo << std::endl;

  if (instruction == "READ") {
    const auto operandValue = isData(operandOne) ? (currentMemoryAddress - labelsAndData.dataToAddress.at(std::string(operandOne))) : static_cast<uint8_t>(std::stoi(std::string(operandOne)));
    return setInstruction(Instruction::READ) | setValue(operandValue, 0, 5);
  } else if (instruction == "LOAD") {
    return setInstruction(Instruction::LOAD) | setRegisters(getRegister(operandOne), getRegister(operandTwo));
  } else if (instruction == "STORE") {
    return setInstruction(Instruction::STORE) | setRegisters(getRegister(operandOne), getRegister(operandTwo));
  } else if (instruction == "COPY") {
    return setInstruction(Instruction::COPY) | setRegisters(getRegister(operandOne), getRegister(operandTwo));
  } else if (instruction == "ADD") {
    return setInstruction(Instruction::ADD) | setRegisters(getRegister(operandOne), getRegister(operandTwo));
  } else if (instruction == "JG") {
    const auto operandValue = isLabel(operandOne) ? (currentMemoryAddress - labelsAndData.labelToAddress.at(std::string(operandOne))) : static_cast<uint8_t>(std::stoi(std::string(operandOne)));
    return setInstruction(Instruction::JG) | setValue(operandValue, 0, 5);
  } else if (instruction == "SYS") {
    const auto syscall = [&]() {
      if (operandOne == "PUTC") {
        return SystemCalls::PUTC;
      } else {
        return SystemCalls::HALT;
      }
    }();

    return setInstruction(Instruction::SYS) | setValue(syscall, 0, 5);
  }

  return 0;
}

std::vector<uint8_t> generateMachineCode(
  const char* inputAssemblyFilePath, const LabelsAndData& labelsAndData)
{
  std::vector<uint8_t> result;

  int currentMemoryAddress = 0;
  std::string line;
  std::ifstream assemblyFileStream(inputAssemblyFilePath);
  while (std::getline(assemblyFileStream, line)) {
    if (line.empty()) {
      continue;
    }

    if (auto [one, two, three] = splitLine(line, ' '); isInstruction(one)) {
      result.push_back(createInstruction(currentMemoryAddress++, one, two, three, labelsAndData));
    } else if (isData(one)) {
      const auto datum = isData(two) ? labelsAndData.dataToAddress.at(std::string(two)) : static_cast<uint8_t>(std::stoi(std::string(two)));
      result.push_back(datum);
      ++currentMemoryAddress;
      continue;
    } else /*isLabel*/ {
      continue;
    }
  }

  return result;
}

int main(int argc, const char** argv)
{
  if (argc != 3) {
    std::cout << "invalid usage" << std::endl;
    return EXIT_FAILURE;
  }

  const char* inputAssemblyFilePath = argv[1];
  const char* bytecodeOutputFilePath = argv[2];

  const auto labelsAndData = getLabelsAndData(inputAssemblyFilePath);

  for (auto [label, value] : labelsAndData.labelToAddress) {
    std::cout << label << ":" << value << std::endl;
  }

  for (auto [data, value] : labelsAndData.dataToAddress) {
    std::cout << data << ":" << value << std::endl;
  }

  const auto machineCode = generateMachineCode(inputAssemblyFilePath, labelsAndData);

  std::ofstream outputFile(bytecodeOutputFilePath, std::ios::out | std::ios::binary);
  for (auto byte : machineCode) {
    outputFile.put(byte);
  }

  return EXIT_SUCCESS;
}