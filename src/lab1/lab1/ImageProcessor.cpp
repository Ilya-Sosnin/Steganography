#include "ImageProcessor.hpp"

#include <algorithm>
#include <bitset>
#include <filesystem>
#include <fstream>

ImageProcessor::ImageProcessor(int bitNumber, fs::path originalImageFile,
                               fs::path messageFile, fs::path resultDir)
    : bitNumber(bitNumber), originalImageFile(originalImageFile),
      messageFile(messageFile), resultDir(resultDir) {}

ImageProcessor::~ImageProcessor() {}

void ImageProcessor::ProcessorImage() {
  // auto originalImageBinary = ReadBMP(originalImageFile);

  // std::cout << "From " << originalImageFile << " read "
  //           << originalImageBinary.size() * 8 << " bits\n";

  // auto extractedBitPlane = ExtractBitPlane(originalImageBinary);

  // auto bitPlaneFile = WriteBMP(originalImageName + "_plane_bit_" +
  //                                  std::to_string(bitNumber) + ".bmp",
  //                              extractedBitPlane);

  // std::cout << "Bit plane written to " << bitPlaneFile.filename() << "\n";

  // auto privateMessage = ReadTXT();

  // std::cout << "From " << messageFile.filename() << " read "
  //           << privateMessage.size() * 8 << " bits\n";

  // auto [writtenBits, embededTextBitPlane] =
  //     EmbedTextIntoBitPlane(originalImageBinary, privateMessage);

  // auto embedFile = WriteBMP(originalImageName + "_embed_bit_" +
  //                               std::to_string(bitNumber) + ".bmp",
  //                           embededTextBitPlane);

  // std::cout << "Embedded image written to " << embedFile.filename() << "\n";

  // auto extractedMessage = ExtractMessage(embededTextBitPlane);

  // std::cout << "Extracted message size " << extractedMessage.size() * 8
  //           << " bits\n";

  // auto outTxtFile = WriteTxt(originalImageName + "_extract_message_bit_" +
  //                                std::to_string(bitNumber) + ".txt",
  //                            extractedMessage);

  // std::cout << "Extract message written to " << outTxtFile.filename() <<
  // "\n";
}

// std::vector<uint8_t> ImageProcessor::ReadTXT() {
//   if (!fs::exists(messageFile)) {
//     std::cout << "Error: File " << messageFile << " does not exist\n";
//     return {};
//   }

//   uintmax_t size = fs::file_size(messageFile);
//   if (size < minSizeMessage) {
//     std::cout << "Error: The message must be at least 30 bytes. Current size:
//     "
//               << size << " bytes\n";
//     return {};
//   }

//   std::vector<uint8_t> privateMessage;

//   std::ifstream file(messageFile, std::ios::binary);
//   if (!file) {
//     std::cout << "Error: Cannot open file " << messageFile << "\n";
//     return {};
//   }

//   privateMessage.resize(size);

//   file.read(reinterpret_cast<char *>(privateMessage.data()), size);
//   file.close();

//   return privateMessage;
// }

// fs::path ImageProcessor::WriteTxt(const std::string txtName,
//                                   std::vector<uint8_t> &v) {
//   fs::path outDir = resultDir / originalImageDir;

//   if (!fs::exists(outDir))
//     fs::create_directories(outDir);

//   fs::path outFile = outDir / txtName;
//   std::ofstream file(outFile, std::ios::binary);
//   if (!file) {
//     std::cout << "Error: Cannot open file " << outFile << "\n";
//     return {};
//   }

//   if (v.size() <= 0) {
//     std::cout << "Error: Size of extracted message  " << v.size() << "\n";
//     return {};
//   }

//   file.write(reinterpret_cast<char *>(v.data()), v.size() * sizeof(uint8_t));
//   file.close();

//   return outFile;
// }

std::vector<uint8_t>
ImageProcessor::ExtractBitPlane(std::vector<uint8_t> imageBinary) {
  std::vector<uint8_t> imagePlane;
  imagePlane.resize(width * height);

  for (size_t i = 0; i < imageBinary.size(); ++i) {
    int bit = (imageBinary[i] >> (bitNumber - 1)) & 1;
    imagePlane[i] = bit ? 255 : 0;
  }
  return imagePlane;
}

std::pair<size_t, std::vector<uint8_t>>
ImageProcessor::EmbedTextIntoBitPlane(std::vector<uint8_t> imageBinary,
                                      std::vector<uint8_t> privateMessage) {
  size_t byteImage = 0;
  size_t sizeWrittenMessage = 0;

  for (auto byteMessage : privateMessage) {
    if (byteImage >= imageBinary.size())
      break;

    for (int i = 7; i >= 0; --i) {
      int bitMessage = (byteMessage >> i) & 1;
      imageBinary[byteImage] &= ~(1 << (bitNumber - 1));
      imageBinary[byteImage] |= (bitMessage << (bitNumber - 1));
      byteImage++;
    }
    sizeWrittenMessage++;
  }
  return {sizeWrittenMessage, imageBinary};
}

std::vector<uint8_t>

ImageProcessor::ExtractMessage(std::vector<uint8_t> embedImageBinary) {
  uint8_t ch = 0;
  size_t count = 0;
  std::vector<uint8_t> extractedMessage;

  for (auto byte : embedImageBinary) {
    int bit = (byte >> (bitNumber - 1)) & 1;
    ch = (ch << 1) | bit;
    count++;

    if (count == 8) {
      extractedMessage.push_back(ch);
      ch = 0;
      count = 0;
    }
  }
  return extractedMessage;
}

ImageIO::ImageIO() {}

ImageIO::~ImageIO() {}

vector<uint8_t> ImageIO::loadImage(const fs::path &imagePath) {

  inputImagePath = imagePath;

  if (!ReadBMP())
    return {};

  if (!ValidateInputBMP())
    return {};

  if (!ParseBMP())
    return {};

  return pixels;
}

bool ImageIO::saveImage(const fs::path &fileName, vector<uint8_t> &v) {
  if (fileName.extension().empty() ||
      (fileName.extension() != ".bmp" && fileName.extension() != ".BMP")) {
    cout << "Error: File name contains an invalid or empty extension\n";
    return false;
  }

  MakeOutputDir();

  fs::path outputImagePath = outputImageDir / fileName;

  if (!ValidateOutputBMP(outputImagePath, v))
    return false;

  if (!WriteBMP(outputImagePath, v))
    return false;

  return true;
}

bool ImageIO::ReadBMP() {
  if (!fs::exists(inputImagePath)) {
    std::cout << "Error: File " << inputImagePath << " does not exist\n";
    return false;
  }

  std::ifstream file(inputImagePath, ios_base::binary);
  if (!file.is_open()) {
    std::cout << "Error: Cannot open file " << inputImagePath << "\n";
    return false;
  }

  file.seekg(0, ios_base::end);
  size_t sizeFile = static_cast<size_t>(file.tellg());
  file.seekg(0, std::ios::beg);

  if (sizeFile <= 0) {
    std::cout << "Error: File " << inputImagePath.filename() << " is empty\n";
    return false;
  }

  binaryImage.resize(sizeFile);

  if (!file.read(reinterpret_cast<char *>(binaryImage.data()), sizeFile)) {
    std::cout << "Error: Failed to read file " << inputImagePath.filename()
              << "\n";
    return false;
  }

  file.close();

  return true;
}

bool ImageIO::ValidateInputBMP() {
  if (binaryImage.size() < BMP_HEADER_SIZE) {
    std::cout << "Error: File " << inputImagePath.filename()
              << " too is too small in size\n";
    return false;
  }

  if (binaryImage[0] != 'B' || binaryImage[1] != 'M') {
    std::cout << "Error: File " << inputImagePath.filename()
              << " is not in BMP format\n";
    return false;
  }
  uint32_t width, height;
  uint16_t bitCount;

  memcpy(&width, &binaryImage[18], sizeof(uint32_t));
  memcpy(&height, &binaryImage[22], sizeof(uint32_t));
  memcpy(&bitCount, &binaryImage[28], sizeof(uint16_t));

  if (width != 512 || height != 512) {
    std::cout << "Error: File " << inputImagePath.filename()
              << " has an incorrect resolution - " << width << "x" << height
              << ". Expected 512x512\n";
    return false;
  }

  if (bitCount != 8) {
    std::cout << "Error: File " << inputImagePath.filename()
              << " has an incorrect number of bits per pixel - " << bitCount
              << ". Expected 8\n";
    return false;
  }

  return true;
}

bool ImageIO::ParseBMP() {
  if (binaryImage.size() < BMP_HEADER_SIZE) {
    std::cout << "Error: File " << inputImagePath.filename()
              << " too is too small in size\n";
    return false;
  }

  memcpy(&headerBMP, &binaryImage[0], sizeof(headerBMP));
  memcpy(&infoBMP, &binaryImage[14], sizeof(infoBMP));

  int numColors = infoBMP.biClrUsed;

  if (numColors == 0)
    numColors = (1 << infoBMP.biBitCount);

  uint32_t startPixels = headerBMP.bfOffBits;
  size_t startPalette = sizeof(headerBMP) + infoBMP.biSize;

  if (startPalette > binaryImage.size() || startPixels > binaryImage.size() ||
      startPalette > startPixels) {
    std::cout << "Error: File " << inputImagePath.filename()
              << " is corrupted\n";
    return false;
  }

  palette.assign(binaryImage.begin() + startPalette,
                 binaryImage.begin() + startPixels);
  pixels.assign(binaryImage.begin() + startPixels, binaryImage.end());

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

  if (headerBMP.bfType != 0x4D42) {
    cout << "Error: Invalid BMP header when attempting to write to file "
         << outputImagePath << "\n";
    return false;
  }

  if (infoBMP.biSize != 40) {
    cout << "Error: Invalid BMP info when attempting to write to file "
         << outputImagePath << "\n";
    return false;
  }

  if (infoBMP.biBitCount <= 8 && palette.empty()) {
    cout << "Error: Invalid palette when attempting to write to file "
         << outputImagePath << "\n";
    return false;
  }

  if (headerBMP.bfOffBits <
      sizeof(headerBMP) + infoBMP.biSize + palette.size()) {
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

  if (!file.write(reinterpret_cast<char *>(&headerBMP), sizeof(headerBMP))) {
    cout << "Error: Failed to write header BMP to file " << outputImagePath
         << "\n";
    return false;
  }

  if (!file.write(reinterpret_cast<char *>(&infoBMP), sizeof(infoBMP))) {
    cout << "Error: Failed to write info BMP to file " << outputImagePath
         << "\n";
    return false;
  }

  if (!file.write(reinterpret_cast<char *>(palette.data()),
                  palette.size() * sizeof(uint8_t))) {
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
