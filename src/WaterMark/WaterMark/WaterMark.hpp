#ifndef WATERMARK_H
#define WATERMARK_H

#include "FileIO/ImageIO.hpp"
#include "FileIO/TextIO.hpp"

#include <iostream>
#include <vector>

class WaterMark {
public:
  WaterMark();
  ~WaterMark();

  void EmbedWaterMarkLSB(const fs::path &pathImage, const fs::path &pathLogo);

private:
  int bitNum{1};
  int GenerateSeed();
  vector<int> GenerateKey(int seed, size_t size);
  void EmbedLSB(int seed, vector<uint8_t> &image, vector<uint8_t> &logo);
  vector <uint8_t> ExtractLSB(int seed);
};

#endif