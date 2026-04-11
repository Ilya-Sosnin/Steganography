#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

#pragma pack(push, 1)
struct BMPFileHeader {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
};

struct BMPInfo {
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

class ImageProcessor {
public:
  ImageProcessor(int bitNumber, fs::path originalImageFile,
                 fs::path messageFile, fs::path resultDir);
  ~ImageProcessor();

  void ProcessorImage();

private:
  int bitNumber{0};
  int width{512};
  int height{512};

  size_t minSizeMessage{30720};

  std::string originalImageName;
  fs::path originalImageDir;

  fs::path originalImageFile;
  fs::path messageFile;
  fs::path resultDir;

  std::vector<uint32_t> palette;

  BMPFileHeader headerBMP;
  BMPInfo infoBMP;

  std::vector<uint8_t> ReadBMP(const fs::path &imageFile);

  fs::path WriteBMP(const std::string imageName, std::vector<uint8_t> &v);

  std::vector<uint8_t> ReadTXT();

  fs::path WriteTxt(const std::string txtName, std::vector<uint8_t> &v);

  std::vector<uint8_t> ExtractBitPlane(std::vector<uint8_t> imageBinary);

  std::pair<size_t, std::vector<uint8_t>>
  EmbedTextIntoBitPlane(std::vector<uint8_t> binaryImage,
                        std::vector<uint8_t> privateMessage);

  std::vector<uint8_t> ExtractMessage(std::vector<uint8_t> embedImageBinary);
};

#endif