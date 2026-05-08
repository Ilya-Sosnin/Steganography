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
  void EmbeddingData(int bitNumber, const fs::path &pathMessage,
                     const fs::path &pathImage);
  void ExtMessage(int bitNumber, const fs::path &pathImage);

private:
  static constexpr int WIDTH = 512;
  static constexpr int HEIGHT = 512;

  fs::path resultDir = PROJECT_ROOT / fs::path("results");

  vector<uint8_t> ExtractBitPlane(int bitNumber, StructBMP &image);
  size_t EmbedTextIntoBitPlane(int bitNumber, vector<uint8_t> &message,
                               StructBMP &image);
  vector<uint8_t> ExtractMessage(int bitNumber, StructBMP &image);
};

#endif