#ifndef WATERMARK_H
#define WATERMARK_H

#include "FileIO/ImageIO.hpp"
#include "FileIO/TextIO.hpp"

#include <iostream>
#include <vector>

struct StructDW {
  uint32_t width;
  uint32_t height;
  vector<uint8_t> pixels;
};

class WaterMark {
public:
  WaterMark();
  ~WaterMark();

  void EmbedWaterMarkLSB(const fs::path &pathImage, const fs::path &pathDw);
  void ExtractedMarkLSB(const fs::path &pathSeed, const fs::path &pathImage);

  void EmbedWaterMarkLocalVariance(const fs::path &pathImage,
                                   const fs::path &pathDw);
  void ExtractWaterMarkLocalVariance(const fs::path &pathImage);

private:
  int bitPixel{0};
  fs::path resultDir = PROJECT_ROOT / fs::path("results");

  int GenerateSeed();
  vector<int> GenerateKey(int seed, size_t size);
  int ReadSeed(const fs::path &pathSeed);

  void EmbedDW(vector<int> &order, StructBMP &image, StructBMP &logo);
  StructDW ExtractDW(vector<int> &order, StructBMP &image);

  vector<pair<int, double>> CalculaingPixelVariance(StructBMP &image);
  vector<int> SortingVariance(vector<pair<int, double>> &variancePixels);
};

#endif