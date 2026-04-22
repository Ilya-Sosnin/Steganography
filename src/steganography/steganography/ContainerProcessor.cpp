#include "ContainerProcessor.hpp"

#include <algorithm>
#include <bitset>
#include <filesystem>
#include <fstream>

ContainerProcessor::ContainerProcessor() {}

ContainerProcessor::~ContainerProcessor() {}

void ContainerProcessor::ExtBitPlane(int bitNumber, const fs::path &pathImage) {
  vector<uint8_t> pixels = imgIO.Read(pathImage);
  if (pixels.empty())
    return;

  vector<uint8_t> bitPlane = ExtractBitPlane(bitNumber, pixels);
  if (bitPlane.empty())
    return;

  fs::path outImageName = "ext_plane_bit_num_" + to_string(bitNumber) + ".bmp";
  if (!imgIO.Write(outImageName, bitPlane))
    return;
}

void ContainerProcessor::EmbeddingData(int bitNumber, const fs::path &pathImage,
                                       const fs::path &pathMessage) {
  vector<uint8_t> pixels = imgIO.Read(pathImage);
  if (pixels.empty())
    return;

  vector<uint8_t> message = txtIO.Read(pathMessage);
  if (message.empty())
    return;

  auto [sizeWriteMess, embedMessPixels] =
      EmbedTextIntoBitPlane(bitNumber, pixels, message);

  if (sizeWriteMess == 0 || embedMessPixels.empty()) {
    cout << "Embedding failed\n";
    return;
  }

  cout << "The image " << sizeWriteMess * 8 << " bits out of "
       << message.size() * 8 << "\n";

  fs::path outImageName = "embed_bit_num_" + to_string(bitNumber) + ".bmp";
  if (!imgIO.Write(outImageName, embedMessPixels))
    return;
}

void ContainerProcessor::ExtMessage(int bitNumber, const fs::path &pathImage) {
  vector<uint8_t> pixels = imgIO.Read(pathImage);
  if (pixels.empty())
    return;

  vector<uint8_t> message = ExtractMessage(bitNumber, pixels);
  if (message.empty())
    return;

  fs::path resultsDir = fs::path(PROJECT_ROOT) / "results";
  fs::create_directories(resultsDir);

  fs::path outTextFile = resultsDir / ("extract_message_bit_" +
                                       std::to_string(bitNumber) + ".txt");

  if (!txtIO.Write(outTextFile, message))
    return;
}

vector<uint8_t>
ContainerProcessor::ExtractBitPlane(int bitNumber,
                                    vector<uint8_t> imageBinary) {
  vector<uint8_t> imagePlane;
  imagePlane.resize(WIDTH * HEIGHT);

  for (size_t i = 0; i < imageBinary.size(); ++i) {
    int bit = (imageBinary[i] >> (bitNumber - 1)) & 1;
    imagePlane[i] = bit ? 255 : 0;
  }
  return imagePlane;
}

pair<size_t, vector<uint8_t>>
ContainerProcessor::EmbedTextIntoBitPlane(int bitNumber,
                                          vector<uint8_t> imageBinary,
                                          vector<uint8_t> privateMessage) {
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

vector<uint8_t>
ContainerProcessor::ExtractMessage(int bitNumber,
                                   vector<uint8_t> embedImageBinary) {
  uint8_t ch = 0;
  size_t count = 0;
  vector<uint8_t> extractedMessage;

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
