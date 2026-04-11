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
  auto originalImageBinary = ReadBMP(originalImageFile);

  std::cout << "From " << originalImageFile << " read "
            << originalImageBinary.size() * 8 << " bits\n";

  auto extractedBitPlane = ExtractBitPlane(originalImageBinary);

  auto bitPlaneFile = WriteBMP(originalImageName + "_plane_bit_" +
                                   std::to_string(bitNumber) + ".bmp",
                               extractedBitPlane);

  std::cout << "Bit plane written to " << bitPlaneFile.filename() << "\n";

  auto privateMessage = ReadTXT();

  std::cout << "From " << messageFile.filename() << " read "
            << privateMessage.size() * 8 << " bits\n";

  auto [writtenBits, embededTextBitPlane] =
      EmbedTextIntoBitPlane(originalImageBinary, privateMessage);

  auto embedFile = WriteBMP(originalImageName + "_embed_bit_" +
                                std::to_string(bitNumber) + ".bmp",
                            embededTextBitPlane);

  std::cout << "Embedded image written to " << embedFile.filename() << "\n";

  auto extractedMessage = ExtractMessage(embededTextBitPlane);

  std::cout << "Extracted message size " << extractedMessage.size() * 8
            << " bits\n";

  auto outTxtFile = WriteTxt(originalImageName + "_extract_message_bit_" +
                                 std::to_string(bitNumber) + ".txt",
                             extractedMessage);

  std::cout << "Extract message written to " << outTxtFile.filename() << "\n";
}

std::vector<uint8_t> ImageProcessor::ReadBMP(const fs::path &imageFile) {
  if (!fs::exists(imageFile)) {
    std::cout << "Error: File " << imageFile << " does not exist\n";
    return {};
  }

  std::ifstream file(imageFile, std::ios_base::binary);
  if (!file.is_open()) {
    std::cout << "Error: Cannot open file " << imageFile << "\n";
    return {};
  }

  std::vector<uint8_t> imageBinary;
  originalImageName = imageFile.stem().string();
  originalImageDir = imageFile.parent_path().filename();

  file.read(reinterpret_cast<char *>(&headerBMP), sizeof(headerBMP));
  file.read(reinterpret_cast<char *>(&infoBMP), sizeof(infoBMP));

  uint32_t numColors =
      infoBMP.biClrUsed ? infoBMP.biClrUsed : (1 << infoBMP.biBitCount);

  palette.resize(numColors);
  file.read(reinterpret_cast<char *>(palette.data()),
            numColors * sizeof(uint32_t));

  width = int(infoBMP.biWidth);
  height = int(infoBMP.biHeight);

  imageBinary.resize(width * height);
  file.read(reinterpret_cast<char *>(imageBinary.data()), imageBinary.size());

  file.close();

  return imageBinary;
}

fs::path ImageProcessor::WriteBMP(const std::string imageName,
                                  std::vector<uint8_t> &v) {
  fs::path outDir = resultDir / originalImageDir;

  if (!fs::exists(outDir))
    fs::create_directories(outDir);

  fs::path outFile = outDir / imageName;

  std::ofstream file(outFile, std::ios_base::binary);
  if (!file.is_open()) {
    std::cout << "Error: Cannot open file " << outFile << "\n";
    return {};
  }

  file.write(reinterpret_cast<char *>(&headerBMP), sizeof(headerBMP));
  file.write(reinterpret_cast<char *>(&infoBMP), sizeof(infoBMP));
  file.write(reinterpret_cast<char *>(palette.data()),
             palette.size() * sizeof(uint32_t));
  file.write(reinterpret_cast<char *>(v.data()), v.size());

  file.close();

  return outFile;
}

std::vector<uint8_t> ImageProcessor::ReadTXT() {
  if (!fs::exists(messageFile)) {
    std::cout << "Error: File " << messageFile << " does not exist\n";
    return {};
  }

  uintmax_t size = fs::file_size(messageFile);
  if (size < minSizeMessage) {
    std::cout << "Error: The message must be at least 30 bytes. Current size: "
              << size << " bytes\n";
    return {};
  }

  std::vector<uint8_t> privateMessage;

  std::ifstream file(messageFile, std::ios::binary);
  if (!file) {
    std::cout << "Error: Cannot open file " << messageFile << "\n";
    return {};
  }

  privateMessage.resize(size);

  file.read(reinterpret_cast<char *>(privateMessage.data()), size);
  file.close();

  return privateMessage;
}

fs::path ImageProcessor::WriteTxt(const std::string txtName,
                                  std::vector<uint8_t> &v) {
  fs::path outDir = resultDir / originalImageDir;

  if (!fs::exists(outDir))
    fs::create_directories(outDir);

  fs::path outFile = outDir / txtName;
  std::ofstream file(outFile, std::ios::binary);
  if (!file) {
    std::cout << "Error: Cannot open file " << outFile << "\n";
    return {};
  }

  if (v.size() <= 0) {
    std::cout << "Error: Size of extracted message  " << v.size() << "\n";
    return {};
  }

  file.write(reinterpret_cast<char *>(v.data()), v.size() * sizeof(uint8_t));
  file.close();

  return outFile;
}

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