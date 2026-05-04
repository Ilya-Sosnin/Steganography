#ifndef TEXTIO_HPP
#define TEXTIO_HPP

#include <filesystem>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;

class TextIO {
public:
  TextIO();
  ~TextIO();

  vector<uint8_t> ReadBinary(const fs::path &textPath);
  vector<string> Read(const fs::path &textPath);
  bool WriteBinary(const fs::path &textPath, vector<uint8_t> &v);
  bool Write(const fs::path &textPath, const string &str);

  // private:
  //   static constexpr size_t MIN_TEXT_SIZE = 30720;
};

#endif