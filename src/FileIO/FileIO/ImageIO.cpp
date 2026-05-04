#include "ImageIO.hpp"

#include <fstream>

ImageIO::ImageIO() {}

ImageIO::ImageIO(const fs::path &imagePath) { ReadFileBMP(imagePath); }

ImageIO::~ImageIO() {}

bool ImageIO::ReadFileBMP(const fs::path &imagePath) {
  if (!ReadBMP(imagePath)) {
    cout << "Error: Failed to read file " << imagePath << "\n";
    return false;
  }

  if (!ValidateInputBMP(imagePath)) {
    cout << "Error: File " << imagePath << " not valid\n";
    return false;
  }

  if (!ParseBMP(imagePath)) {
    cout << "Error: Incorrect file format " << imagePath << "\n";
    return false;
  }

  return true;
}

bool ImageIO::WriteFileBMP(const fs::path &imagePath) {
  if (imagePath.extension().empty() ||
      (imagePath.extension() != ".bmp" && imagePath.extension() != ".BMP")) {
    cout << "Error: File name contains an invalid or empty extension\n";
    return false;
  }

  // if (!ValidateOutputBMP(imagePath))
  //   return false;

  if (!WriteBMP(imagePath))
    return false;

  std::cout << "Path to the resulting image: " << imagePath << "\n";

  return true;
}

void ImageIO::CreateBMP(uint32_t width, uint32_t height,
                        const vector<uint8_t> &pixels) {
  structBmp.headerBmp.bfType = 0x4D42;
  structBmp.headerBmp.bfReserved1 = 0;
  structBmp.headerBmp.bfReserved2 = 0;

  structBmp.palette.resize(256 * 4);
  for (int i = 0; i < 256; ++i) {
    structBmp.palette[i * 4 + 0] = i;
    structBmp.palette[i * 4 + 1] = i;
    structBmp.palette[i * 4 + 2] = i;
    structBmp.palette[i * 4 + 3] = 0;
  }

  structBmp.infoBmp.biWidth = width;
  structBmp.infoBmp.biHeight = height;
  structBmp.infoBmp.biBitCount = 8;
  structBmp.infoBmp.biSize = 40;
  structBmp.infoBmp.biPlanes = 1;
  structBmp.infoBmp.biCompression = 0;
  structBmp.infoBmp.biSizeImage = pixels.size();

  structBmp.pixels = pixels;

  structBmp.headerBmp.bfOffBits =
      sizeof(FileHeaderBMP) + sizeof(InfoBMP) + structBmp.palette.size();

  structBmp.headerBmp.bfSize = structBmp.headerBmp.bfOffBits + pixels.size();
}

bool ImageIO::ReadBMP(const fs::path &imagePath) {
  if (!fs::exists(imagePath)) {
    cout << "Error: File " << imagePath << " does not exist\n";
    return false;
  }

  ifstream file(imagePath, ios_base::binary);
  if (!file.is_open()) {
    cout << "Error: Cannot open file " << imagePath << "\n";
    return false;
  }

  file.seekg(0, ios_base::end);
  size_t sizeFile = static_cast<size_t>(file.tellg());
  file.seekg(0, ios::beg);

  if (sizeFile <= 0) {
    cout << "Error: File " << imagePath.filename() << " is empty\n";
    return false;
  }

  binaryImage.resize(sizeFile);

  if (!file.read(reinterpret_cast<char *>(binaryImage.data()), sizeFile)) {
    cout << "Error: Failed to read file " << imagePath.filename() << "\n";
    return false;
  }

  file.close();

  return true;
}

bool ImageIO::ValidateInputBMP(const fs::path &imagePath) {
  if (binaryImage.size() < BMP_HEADER_SIZE) {
    cout << "Error: File " << imagePath.filename()
         << " too is too small in size\n";
    return false;
  }

  if (binaryImage[0] != 'B' || binaryImage[1] != 'M') {
    cout << "Error: File " << imagePath.filename() << " is not in BMP format\n";
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

bool ImageIO::ParseBMP(const fs::path &imagePath) {
  if (binaryImage.size() < BMP_HEADER_SIZE) {
    cout << "Error: File " << imagePath.filename()
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
    cout << "Error: File " << imagePath.filename() << " is corrupted\n";
    return false;
  }

  structBmp.palette.assign(binaryImage.begin() + startPalette,
                           binaryImage.begin() + startPixels);
  structBmp.pixels.assign(binaryImage.begin() + startPixels, binaryImage.end());

  return true;
}

// bool ImageIO::ValidateOutputBMP(const fs::path &outputImagePath) {
//   if (structBmp.headerBmp.bfType != 0x4D42) {
//     cout << "Error: Invalid BMP header when attempting to write to file "
//          << outputImagePath << "\n";
//     return false;
//   }

//   if (structBmp.infoBmp.biSize != 40) {
//     cout << "Error: Invalid BMP info when attempting to write to file "
//          << outputImagePath << "\n";
//     return false;
//   }

//   if (structBmp.infoBmp.biBitCount <= 8 && structBmp.palette.empty()) {
//     cout << "Error: Invalid palette when attempting to write to file "
//          << outputImagePath << "\n";
//     return false;
//   }

//   if (structBmp.headerBmp.bfOffBits < sizeof(structBmp.headerBmp) +
//                                           structBmp.infoBmp.biSize +
//                                           structBmp.palette.size()) {
//     cout << "Error: Invalid size of BMP header, BMP information or palette "
//             "when writing to file  "
//          << outputImagePath << "\n";
//     return false;
//   }

//   return true;
// }

bool ImageIO::WriteBMP(const fs::path &outputImagePath) {

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

  if (!file.write(reinterpret_cast<char *>(structBmp.pixels.data()),
                  structBmp.pixels.size() * sizeof(uint8_t))) {
    cout << "Error: Failed to write pixel to file " << outputImagePath << "\n";
    return false;
  }

  file.close();

  return true;
}