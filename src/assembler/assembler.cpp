#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "../shared.hpp"

std::vector<std::string> getLinesFromFile(const char* filePath)
{
  std::vector<std::string> result;
  std::string line;
  std::ifstream assemblyFileStream(filePath);
  while (std::getline(assemblyFileStream, line)) {
    if (line.empty()) {
      continue;
    }
    result.push_back(line);
  }

  return result;
}

bool isWhitespace(char c)
{
  return (c == ' ') || (c == '\t');
}

struct SplitLine {
  std::string_view one;
  std::string_view two;
  std::string_view three;
};
SplitLine splitLine(std::string_view line, char delimeter)
{
  SplitLine result;

  const auto getNextToken = [](std::string_view& output, std::string_view& input) {
    while (!input.empty() && isWhitespace(input.front())) {
      input.remove_prefix(1);
    }

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
  return
    (line == "READ") ||
    (line == "LOAD") ||
    (line == "STORE") ||
    (line == "COPY") ||
    (line == "ADD") ||
    (line == "JG") ||
    (line == "SYS");
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
LabelsAndData getLabelsAndData(const std::vector<std::string>& lines)
{
  LabelsAndData result;

  int currentMemoryAddress = 0;
  for (const auto& line : lines) {
    if (auto [one, two, three] = splitLine(line, ' '); isInstruction(one)) {
      ++currentMemoryAddress;
    } else if (isData(one)) {
      result.dataToAddress[std::string(one)] = currentMemoryAddress++;
    } else /*isLabel*/ {
      result.labelToAddress[std::string(one)] = currentMemoryAddress;
    }
  }

  return result;
}

uint8_t createInstruction(
  int currentMemoryAddress,
  std::string_view instruction,
  std::string_view operandOne,
  std::string_view operandTwo,
  const LabelsAndData& labelsAndData)
{
  if (instruction == "READ") {
    const int8_t operandValue = isData(operandOne) ?
      (labelsAndData.dataToAddress.at(std::string(operandOne)) - currentMemoryAddress) :
      static_cast<int8_t>(std::stoi(std::string(operandOne)));
    return setInstruction(Instruction::READ) | setSignedValue(operandValue, 0, 5);
  } else if (instruction == "LOAD") {
    return setInstruction(Instruction::LOAD) | setRegisters(getRegister(operandOne), getRegister(operandTwo));
  } else if (instruction == "STORE") {
    return setInstruction(Instruction::STORE) | setRegisters(getRegister(operandOne), getRegister(operandTwo));
  } else if (instruction == "COPY") {
    return setInstruction(Instruction::COPY) | setRegisters(getRegister(operandOne), getRegister(operandTwo));
  } else if (instruction == "ADD") {
    return setInstruction(Instruction::ADD) | setRegisters(getRegister(operandOne), getRegister(operandTwo));
  } else if (instruction == "JG") {
    const int8_t operandValue = isLabel(operandOne) ?
      (labelsAndData.labelToAddress.at(std::string(operandOne)) - currentMemoryAddress) :
      static_cast<int8_t>(std::stoi(std::string(operandOne)));
    const auto jumpInstruction = setInstruction(Instruction::JG) | setSignedValue(operandValue, 0, 5);
    return jumpInstruction;
  } else if (instruction == "SYS") {
    const auto syscall = [operandOne]() {
      if (operandOne == "PUTC") {
        return SystemCalls::PUTC;
      } else if (operandOne == "GETC") {
        return SystemCalls::GETC;
      } else {
        return SystemCalls::HALT;
      }
    }();
    return setInstruction(Instruction::SYS) | setValue(syscall, 0, 5);
  }

  return 0;
}

std::vector<uint8_t> generateMachineCode(const std::vector<std::string>& lines, const LabelsAndData& labelsAndData)
{
  std::vector<uint8_t> result;

  int currentMemoryAddress = 0;
  for (const auto& line : lines) {
    if (auto [one, two, three] = splitLine(line, ' '); isInstruction(one)) {
      const auto machineInstruction = createInstruction(currentMemoryAddress++, one, two, three, labelsAndData);
      result.push_back(machineInstruction);
    } else if (isData(one)) {
      const auto datum = isData(two) ?
        labelsAndData.dataToAddress.at(std::string(two)) :
        static_cast<int8_t>(std::stoi(std::string(two)));
      result.push_back(datum);
      ++currentMemoryAddress;
    } else /*isLabel*/ {
      continue;
    }
  }

  return result;
}

void writeToBinaryFile(const char* machineCodeOutputFilePath, const std::vector<uint8_t>& machineCode)
{
  std::ofstream outputFile(machineCodeOutputFilePath, std::ios::out | std::ios::binary);
  for (auto byte : machineCode) {
    outputFile.put(byte);
  }
}

int main(int argc, const char** argv)
{
  if (argc != 3) {
    std::cout << "usage: ./assembler {inputAssemblyFilePath} {machineCodeOutputFilePath}" << std::endl;
    return EXIT_FAILURE;
  }

  const char* inputAssemblyFilePath = argv[1];
  const char* machineCodeOutputFilePath = argv[2];

  const auto lines = getLinesFromFile(inputAssemblyFilePath);
  const auto labelsAndData = getLabelsAndData(lines);
  const auto machineCode = generateMachineCode(lines, labelsAndData);
  writeToBinaryFile(machineCodeOutputFilePath, machineCode);

  return EXIT_SUCCESS;
}