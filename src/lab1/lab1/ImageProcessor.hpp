#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

#pragma pack(push, 1)
struct FileHeaderBMP {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
};

struct InfoBMP {
  uint32_t biSize;
  uint32_t biWidth;
  uint32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  uint32_t biXPelsPerMeter;
  uint32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
};
#pragma pack(pop)

class ImageIO {
public:
  ImageIO();
  ~ImageIO();

  vector<uint8_t> Read(const fs::path &imageFile);
  bool Write(const fs::path &imageName, vector<uint8_t> &v);

private:
  static constexpr size_t BMP_HEADER_SIZE = 54;

  fs::path originalImageName;
  fs::path inputImagePath;
  fs::path outputImageDir;

  vector<uint8_t> binaryImage;
  vector<uint8_t> palette;
  vector<uint8_t> pixels;

  FileHeaderBMP headerBMP;
  InfoBMP infoBMP;

  bool ReadBMP();
  bool ValidateInputBMP();
  bool ParseBMP();

  void MakeOutputDir();
  bool ValidateOutputBMP(const fs::path &outputImagePath, vector<uint8_t> &v);
  bool WriteBMP(const fs::path &outputImagePath, vector<uint8_t> &v);
};

class TextIO {
public:
  TextIO();
  ~TextIO();

  std::vector<uint8_t> Read(const fs::path &inputTextFile);
  bool Write(const fs::path &inputTextFile, std::vector<uint8_t> &v);

private:
  static constexpr size_t MIN_TEXT_SIZE = 30720;
};

class ImageProcessor {
public:
  ImageProcessor();
  ~ImageProcessor();

  void ExtBitPlane(int bitNumber, const fs::path &pathImage);
  void EmbeddingData(int bitNumber, const fs::path &pathImage,
                     const fs::path &pathMessage);

  void ExtMessage(int bitNumber, const fs::path &pathImage);

private:
  static constexpr int WIDTH = 512;
  static constexpr int HEIGHT = 512;

  ImageIO imgIO;
  TextIO txtIO;

  vector<uint8_t> ExtractBitPlane(int bitNumber, vector<uint8_t> imageBinary);

  pair<size_t, vector<uint8_t>>
  EmbedTextIntoBitPlane(int bitNumber, vector<uint8_t> binaryImage,
                        vector<uint8_t> privateMessage);

  vector<uint8_t> ExtractMessage(int bitNumber,
                                 vector<uint8_t> embedImageBinary);
};

#endif