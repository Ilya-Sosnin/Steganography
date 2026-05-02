#include "ImageIO.hpp"

#include <fstream>
#include <iostream>

ImageIO::ImageIO() {}

ImageIO::~ImageIO() {}

vector<uint8_t> ImageIO::Read(const fs::path &imagePath) {

  inputImagePath = imagePath;
  originalImageName = inputImagePath.filename().stem().string();

  if (!ReadBMP())
    return {};

  if (!ValidateInputBMP())
    return {};

  if (!ParseBMP())
    return {};

  return structBmp.pixels;
}

bool ImageIO::Write(const fs::path &fileName, vector<uint8_t> &v) {
  if (fileName.extension().empty() ||
      (fileName.extension() != ".bmp" && fileName.extension() != ".BMP")) {
    cout << "Error: File name contains an invalid or empty extension\n";
    return false;
  }

  MakeOutputDir();

  fs::path outputImageName =
      originalImageName.string() + "_" + fileName.string();
  fs::path outputImagePath = outputImageDir / outputImageName;

  if (!ValidateOutputBMP(outputImagePath, v))
    return false;

  if (!WriteBMP(outputImagePath, v))
    return false;

  std::cout << "Path to the resulting image: " << outputImagePath << "\n";

  return true;
}

bool ImageIO::ReadBMP() {
  if (!fs::exists(inputImagePath)) {
    cout << "Error: File " << inputImagePath << " does not exist\n";
    return false;
  }

  ifstream file(inputImagePath, ios_base::binary);
  if (!file.is_open()) {
    cout << "Error: Cannot open file " << inputImagePath << "\n";
    return false;
  }

  file.seekg(0, ios_base::end);
  size_t sizeFile = static_cast<size_t>(file.tellg());
  file.seekg(0, ios::beg);

  if (sizeFile <= 0) {
    cout << "Error: File " << inputImagePath.filename() << " is empty\n";
    return false;
  }

  binaryImage.resize(sizeFile);

  if (!file.read(reinterpret_cast<char *>(binaryImage.data()), sizeFile)) {
    cout << "Error: Failed to read file " << inputImagePath.filename() << "\n";
    return false;
  }

  file.close();

  return true;
}

int ImageIO::GetWidthImage() { return structBmp.infoBmp.biWidth; }

int ImageIO::GetHeightImage() { return structBmp.infoBmp.biHeight; }

size_t ImageIO::GetHeaderSize() {
  return sizeof(structBmp.headerBmp) + sizeof(structBmp.infoBmp) +
         structBmp.palette.size();
}

size_t ImageIO::GetPixelsSize() { return structBmp.pixels.size(); }

bool ImageIO::ValidateInputBMP() {
  if (binaryImage.size() < BMP_HEADER_SIZE) {
    cout << "Error: File " << inputImagePath.filename()
         << " too is too small in size\n";
    return false;
  }

  if (binaryImage[0] != 'B' || binaryImage[1] != 'M') {
    cout << "Error: File " << inputImagePath.filename()
         << " is not in BMP format\n";
    return false;
  }
  uint32_t width, height;
  uint16_t bitCount;

  memcpy(&width, &binaryImage[18], sizeof(uint32_t));
  memcpy(&height, &binaryImage[22], sizeof(uint32_t));
  memcpy(&bitCount, &binaryImage[28], sizeof(uint16_t));

  // if (width != 512 || height != 512) {
  //   cout << "Error: File " << inputImagePath.filename()
  //        << " has an incorrect resolution - " << width << "x" << height
  //        << ". Expected 512x512\n";
  //   return false;
  // }

  // if (bitCount != 8) {
  //   cout << "Error: File " << inputImagePath.filename()
  //        << " has an incorrect number of bits per pixel - " << bitCount
  //        << ". Expected 8\n";
  //   return false;
  // }

  return true;
}

bool ImageIO::ParseBMP() {
  if (binaryImage.size() < BMP_HEADER_SIZE) {
    cout << "Error: File " << inputImagePath.filename()
         << " too is too small in size\n";
    return false;
  }

  memcpy(&structBmp.headerBmp, &binaryImage[0], sizeof(structBmp.headerBmp));
  memcpy(&structBmp.infoBmp, &binaryImage[14], sizeof(structBmp.infoBmp));

  int numColors = structBmp.infoBmp.biClrUsed;

  if (numColors == 0)
    numColors = (1 << structBmp.infoBmp.biBitCount);

  uint32_t startPixels = structBmp.headerBmp.bfOffBits;
  size_t startPalette = sizeof(structBmp.headerBmp) + structBmp.infoBmp.biSize;

  if (startPalette > binaryImage.size() || startPixels > binaryImage.size() ||
      startPalette > startPixels) {
    cout << "Error: File " << inputImagePath.filename() << " is corrupted\n";
    return false;
  }

  structBmp.palette.assign(binaryImage.begin() + startPalette,
                           binaryImage.begin() + startPixels);
  structBmp.pixels.assign(binaryImage.begin() + startPixels, binaryImage.end());

  return true;
}

void ImageIO::MakeOutputDir() {

  fs::path numSet = inputImagePath.parent_path().stem();

  outputImageDir = fs::path(PROJECT_ROOT) / "results" / numSet;

  fs::create_directories(outputImageDir);
}

bool ImageIO::ValidateOutputBMP(const fs::path &outputImagePath,
                                vector<uint8_t> &v) {
  if (v.empty()) {
    cout << "Error: Set of pixels to write to file " << inputImagePath
         << " is empty\n";
    return false;
  }

  if (structBmp.headerBmp.bfType != 0x4D42) {
    cout << "Error: Invalid BMP header when attempting to write to file "
         << outputImagePath << "\n";
    return false;
  }

  if (structBmp.infoBmp.biSize != 40) {
    cout << "Error: Invalid BMP info when attempting to write to file "
         << outputImagePath << "\n";
    return false;
  }

  if (structBmp.infoBmp.biBitCount <= 8 && structBmp.palette.empty()) {
    cout << "Error: Invalid palette when attempting to write to file "
         << outputImagePath << "\n";
    return false;
  }

  if (structBmp.headerBmp.bfOffBits < sizeof(structBmp.headerBmp) +
                                          structBmp.infoBmp.biSize +
                                          structBmp.palette.size()) {
    cout << "Error: Invalid size of BMP header, BMP information or palette "
            "when writing to file  "
         << outputImagePath << "\n";
    return false;
  }

  return true;
}

bool ImageIO::WriteBMP(const fs::path &outputImagePath, vector<uint8_t> &v) {

  ofstream file(outputImagePath, ios::binary);
  if (!file) {
    cout << "Error: Cannot open file " << outputImagePath << "\n";
    return false;
  }

  if (!file.write(reinterpret_cast<char *>(&structBmp.headerBmp),
                  sizeof(structBmp.headerBmp))) {
    cout << "Error: Failed to write header BMP to file " << outputImagePath
         << "\n";
    return false;
  }

  if (!file.write(reinterpret_cast<char *>(&structBmp.infoBmp),
                  sizeof(structBmp.infoBmp))) {
    cout << "Error: Failed to write info BMP to file " << outputImagePath
         << "\n";
    return false;
  }

  if (!file.write(reinterpret_cast<char *>(structBmp.palette.data()),
                  structBmp.palette.size() * sizeof(uint8_t))) {
    cout << "Error: Failed to write palette to file " << outputImagePath
         << "\n";
    return false;
  }

  if (!file.write(reinterpret_cast<char *>(v.data()),
                  v.size() * sizeof(uint8_t))) {
    cout << "Error: Failed to write pixel to file " << outputImagePath << "\n";
    return false;
  }

  file.close();

  return true;
}