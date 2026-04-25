#ifndef CONTAINERPROCESSOR_HPP
#define CONTAINERPROCESSOR_HPP

#include "FileIO/ImageIO.hpp"
#include "FileIO/TextIO.hpp"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class ContainerProcessor {
public:
  ContainerProcessor();
  ~ContainerProcessor();

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