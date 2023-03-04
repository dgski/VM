#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

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
      one.remove_prefix(1);
      result.dataToAddress[std::string(one)] = currentMemoryAddress++;
    } else /*isLabel*/ {
      result.labelToAddress[std::string(one)] = ++currentMemoryAddress;
    }
  }

  return result;
}

uint8_t createInstruction(std::string_view one, std::string_view two, std::string_view three)
{
  return 0;
}

std::vector<uint8_t> generateMachineCode(const char* inputAssemblyFilePath, const LabelsAndData& labelsAndData)
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
      result.push_back(createInstruction(one, two, three));
    } else if (isData(one)) {
      result.push_back(static_cast<uint8_t>(std::stoi(std::string(two))));
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
  const auto machineCode = generateMachineCode(inputAssemblyFilePath, labelsAndData);

  std::ofstream outputFile(bytecodeOutputFilePath);
  for (auto byte : machineCode) {
    outputFile.put(byte);
  }

  return EXIT_SUCCESS;
}