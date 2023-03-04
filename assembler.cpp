#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, const char** argv)
{
  if (argc != 3) {
    std::cout << "invalid usage" << std::endl;
    return EXIT_FAILURE;
  }

  const char* inputAssemblyFilePath = argv[2];
  const char* bytecodeOutputFilePath = argv[3];

  std::string line;
  std::ifstream assemblyFileStream(inputAssemblyFilePath);
  while (std::getline(assemblyFileStream, line)) {
  }

  return EXIT_SUCCESS;
}