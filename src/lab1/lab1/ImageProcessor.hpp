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

  // std::vector<uint32_t> palette;

  // BMPFileHeader headerBMP;
  // BMPInfo infoBMP;

  std::vector<uint8_t> ExtractBitPlane(std::vector<uint8_t> imageBinary);

  std::pair<size_t, std::vector<uint8_t>>
  EmbedTextIntoBitPlane(std::vector<uint8_t> binaryImage,
                        std::vector<uint8_t> privateMessage);

  std::vector<uint8_t> ExtractMessage(std::vector<uint8_t> embedImageBinary);
};

class ImageIO {
public:
  ImageIO();
  ~ImageIO();

  vector<uint8_t> loadImage(const fs::path &imageFile);
  bool saveImage(const fs::path& imageName, vector<uint8_t> &v);

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
  bool ValidateOutputBMP(const fs::path& outputImagePath, vector<uint8_t> &v);
  bool WriteBMP(const fs::path& outputImagePath, vector<uint8_t> &v);
};

#endif