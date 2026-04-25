#ifndef TEXTIO_HPP
#define TEXTIO_HPP

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
using namespace std;

class TextIO {
public:
  TextIO();
  ~TextIO();

  std::vector<uint8_t> ReadBinary(const fs::path &inputTextFile);
  bool WriteBinary(const fs::path &inputTextFile, std::vector<uint8_t> &v);

private:
  static constexpr size_t MIN_TEXT_SIZE = 30720;
};

#endif