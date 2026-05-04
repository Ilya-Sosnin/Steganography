#include "TextIO.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


TextIO::TextIO() {}

TextIO::~TextIO() {}

vector<uint8_t> TextIO::ReadBinary(const fs::path &textPath) {
  if (!fs::exists(textPath)) {
    cout << "Error: File " << textPath << " does not exist\n";
    return {};
  }

  uintmax_t size = fs::file_size(textPath);
  // if (size < MIN_TEXT_SIZE) {
  //   cout << "Error: The message must be at least 30 bytes. Current size: "
  //        << size << " bytes\n";
  //   return {};
  // }

  vector<uint8_t> text;

  ifstream file(textPath, ios::binary);
  if (!file) {
    cout << "Error: Cannot open file " << textPath << "\n";
    return {};
  }

  text.resize(size);

  if (!file.read(reinterpret_cast<char *>(text.data()), size)) {
    cout << "Error: Failed to read file " << textPath << "\n";
    return {};
  }

  file.close();

  return text;
}

vector<string> TextIO::Read(const fs::path &textPath) {
  vector<string> v;

  ifstream file(textPath);
  if (!file) {
    cout << "Error: Cannot open file " << textPath << "\n";
    return {};
  }

  if (file.is_open()) {
    string line;
    while (getline(file, line)) {
      v.push_back(line);
    }
    file.close();
  } else {
    cout << "Error:  \n";
  }

  return v;
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

bool TextIO::Write(const fs::path &outTextFile, const std::string &str) {
  ofstream file(outTextFile, ios::binary | ios::beg);
  if (!file) {
    cout << "Error: Cannot open file " << outTextFile << "\n";
    return false;
  }

  file << str;

  file.close();

  std::cout << "Path to the resulting text file: " << outTextFile << "\n";

  return true;
}