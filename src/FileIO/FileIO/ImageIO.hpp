#ifndef IMAGEIO_HPP
#define IMAGEIO_HPP

#include <iostream>
#include <filesystem>
#include <vector>

using namespace std;
namespace fs = filesystem;

#pragma pack(push, 1)
struct FileHeaderBMP {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;

  void PrintHeaderBMP() const {
    cout << "\nbfType: " << bfType << "\n";
    cout << "bfSize: " << bfSize << "\n";
    cout << "bfReserved1: " << bfReserved1 << "\n";
    cout << "bfReserved2: " << bfReserved2 << "\n";
    cout << "bfOffBits: " << bfOffBits << "\n\n";
  }
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

  void PrintInfoBMP() const {
    cout << "\nbiSize: " << biSize << "\n";
    cout << "biWidth: " << biWidth << "\n";
    cout << "biHeight: " << biHeight << "\n";
    cout << "biPlanes: " << biPlanes << "\n";
    cout << "biBitCount: " << biBitCount << "\n";
    cout << "biCompression: " << biCompression << "\n";
    cout << "biSizeImage: " << biSizeImage << "\n";
    cout << "biXPelsPerMeter " << biXPelsPerMeter << "\n";
    cout << "biYPelsPerMeter: " << biYPelsPerMeter << "\n";
    cout << "biClrUsed: " << biClrUsed << "\n";
    cout << "biClrImportant: " << biClrImportant << "\n\n";
  }
};

struct StructBMP {
  FileHeaderBMP headerBmp;
  InfoBMP infoBmp;
  vector<uint8_t> palette;
  vector<uint8_t> pixels;
};
#pragma pack(pop)

class ImageIO {
public:
  ImageIO();
  ImageIO(const fs::path &imagePath);
  ~ImageIO();

  bool ReadFileBMP(const fs::path &imagePath);
  bool WriteFileBMP(const fs::path &imagePath);

  void CreateBMP(uint32_t width, uint32_t height,
                 const vector<uint8_t> &pixels);

  vector<uint8_t> &GetBinaryImage() { return binaryImage; };
  StructBMP &GetStructImage() { return structBmp; };

private:
  static constexpr size_t BMP_HEADER_SIZE = 54;

  vector<uint8_t> binaryImage;
  StructBMP structBmp;

  bool ReadBMP(const fs::path &imagePath);
  bool ValidateInputBMP(const fs::path &imagePath);
  bool ParseBMP(const fs::path &imagePath);
  // bool ValidateOutputBMP(const fs::path &imagePath);
  bool WriteBMP(const fs::path &imagePath);
};

#endif