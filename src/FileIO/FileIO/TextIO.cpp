#include "TextIO.hpp"

#include <fstream>
#include <iostream>
#include <vector>

TextIO::TextIO() {}

TextIO::~TextIO() {}

vector<uint8_t> TextIO::ReadBinary(const fs::path &inputTextFile) {
  if (!fs::exists(inputTextFile)) {
    cout << "Error: File " << inputTextFile << " does not exist\n";
    return {};
  }

  uintmax_t size = fs::file_size(inputTextFile);
  if (size < MIN_TEXT_SIZE) {
    cout << "Error: The message must be at least 30 bytes. Current size: "
         << size << " bytes\n";
    return {};
  }

  vector<uint8_t> privateMessage;

  ifstream file(inputTextFile, ios::binary);
  if (!file) {
    cout << "Error: Cannot open file " << inputTextFile << "\n";
    return {};
  }

  privateMessage.resize(size);

  if (!file.read(reinterpret_cast<char *>(privateMessage.data()), size)) {
    cout << "Error: Failed to read file " << inputTextFile << "\n";
    return {};
  }

  file.close();

  return privateMessage;
}

bool TextIO::WriteBinary(const fs::path &outTextFile, vector<uint8_t> &v) {
  ofstream file(outTextFile, ios::binary);
  if (!file) {
    cout << "Error: Cannot open file " << outTextFile << "\n";
    return false;
  }

  if (v.empty()) {
    cout << "Error: Extracted message is empty\n";
    return false;
  }

  if (!file.write(reinterpret_cast<char *>(v.data()),
                  v.size() * sizeof(uint8_t))) {
    cout << "Error: Failed to write pixel to file " << outTextFile << "\n";
    return false;
  }

  file.close();

  std::cout << "Path to the resulting text file: " << outTextFile << "\n";

  return true;
}