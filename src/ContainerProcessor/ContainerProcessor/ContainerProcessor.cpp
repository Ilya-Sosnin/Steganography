#include "ContainerProcessor/ContainerProcessor.hpp"

#include <algorithm>
#include <bitset>
#include <filesystem>
#include <fstream>

ContainerProcessor::ContainerProcessor() {}

ContainerProcessor::~ContainerProcessor() {}

void ContainerProcessor::ExtBitPlane(int bitNumber, const fs::path &pathImage) {
  ImageIO imgIO(pathImage);
  StructBMP &image = imgIO.GetStructImage();

  vector<uint8_t> bitPlane = ExtractBitPlane(bitNumber, image);
  if (bitPlane.empty())
    return;

  image.pixels = bitPlane;

  string originalName = pathImage.filename().stem().string();
  fs::path outImageName =
      resultDir / fs::path(originalName + "_ext_plane_bit_num_" +
                           to_string(bitNumber) + ".bmp");
  imgIO.WriteFileBMP(outImageName);
}

void ContainerProcessor::EmbeddingData(int bitNumber,
                                       const fs::path &pathMessage,
                                       const fs::path &pathImage) {
  ImageIO imgIO(pathImage);
  StructBMP &image = imgIO.GetStructImage();

  TextIO txtIO;
  vector<uint8_t> message = txtIO.ReadBinary(pathMessage);
  if (message.empty())
    return;

  size_t sizeWriteMess = EmbedTextIntoBitPlane(bitNumber, message, image);

  if (sizeWriteMess == 0) {
    cout << "Embedding failed\n";
    return;
  }

  cout << sizeWriteMess << " out of " << message.size() * 8
       << " bits message embedded in image\n";

  string originalName = pathImage.filename().stem().string();
  fs::path outImageName =
      resultDir / fs::path(originalName + "_embed_bit_num_" +
                           to_string(bitNumber) + ".bmp");
  imgIO.WriteFileBMP(outImageName);
}

void ContainerProcessor::ExtMessage(int bitNumber, const fs::path &pathImage) {
  ImageIO imgIO(pathImage);
  StructBMP &image = imgIO.GetStructImage();

  vector<uint8_t> message = ExtractMessage(bitNumber, image);
  if (message.empty())
    return;

  TextIO txtIO;
  fs::path outTextFile = resultDir / fs::path("extract_message_bit_" +
                                              to_string(bitNumber) + ".txt");

  if (!txtIO.WriteBinary(outTextFile, message))
    return;
}

vector<uint8_t> ContainerProcessor::ExtractBitPlane(int bitNumber,
                                                    StructBMP &image) {
  vector<uint8_t> imagePlane;
  imagePlane.resize(WIDTH * HEIGHT);

  for (size_t i = 0; i < image.pixels.size(); ++i) {
    int bit = (image.pixels[i] >> (bitNumber - 1)) & 1;
    imagePlane[i] = bit ? 255 : 0;
  }
  return imagePlane;
}

size_t ContainerProcessor::EmbedTextIntoBitPlane(
    int bitNumber, vector<uint8_t> &privateMessage, StructBMP &image) {
  size_t byteImage = 0;
  size_t sizeWrittenMessage = 0;

  for (auto byteMessage : privateMessage) {
    if (byteImage >= image.pixels.size())
      break;

    for (int i = 7; i >= 0; --i) {
      int bitMessage = (byteMessage >> i) & 1;
      image.pixels[byteImage] &= ~(1 << (bitNumber - 1));
      image.pixels[byteImage] |= (bitMessage << (bitNumber - 1));
      byteImage++;
    }
    sizeWrittenMessage++;
  }
  return sizeWrittenMessage;
}

vector<uint8_t> ContainerProcessor::ExtractMessage(int bitNumber,
                                                   StructBMP &image) {
  uint8_t ch = 0;
  size_t count = 0;
  vector<uint8_t> extractedMessage;

  for (auto byte : image.pixels) {
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
