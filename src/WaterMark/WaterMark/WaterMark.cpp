#include "WaterMark.hpp"

#include <numeric>
#include <random>
#include <utility>

WaterMark::WaterMark() {}
WaterMark::~WaterMark() {}

void WaterMark::EmbedWaterMarkLSB(const fs::path &pathImage,
                                  const fs::path &pathDw) {
  ImageIO originalImg(pathImage);
  StructBMP &image = originalImg.GetStructImage();

  ImageIO dwImg(pathDw);
  StructBMP &watermark = dwImg.GetStructImage();

  size_t bitsDw = watermark.pixels.size() * 8 + 32 + 32 + 32;
  size_t capContainer = image.pixels.size();

  if (bitsDw < capContainer / 2) {
    cout << "Error: Small size digietal watermark\n";
    return;
  }

  if (bitsDw > capContainer) {
    cout << "Error: Digital watermark size is very large\n";
    return;
  }

  int seed = GenerateSeed();
  TextIO txtIO;
  fs::path pathSeed = resultDir / fs::path("seed.txt");
  txtIO.Write(pathSeed, to_string(seed));

  vector<int> key = GenerateKey(seed, image.pixels.size());

  EmbedDW(key, image, watermark);

  string originalName = pathImage.filename().stem().string();
  fs::path pathEmbedDw =
      resultDir / fs::path(originalName + "_embed_dw_lsb.bmp");

  originalImg.WriteFileBMP(pathEmbedDw);
}

void WaterMark::ExtractedMarkLSB(const fs::path &pathSeed,
                                 const fs::path &pathImage) {
  ImageIO originalImage;
  originalImage.ReadFileBMP(pathImage);
  StructBMP image = originalImage.GetStructImage();
  int seed = ReadSeed(pathSeed);

  vector<int> key = GenerateKey(seed, image.pixels.size());

  StructDW watermark = ExtractDW(key, image);

  ImageIO DwImage;
  DwImage.CreateBMP(watermark.width, watermark.height, watermark.pixels);

  string originalName = pathImage.filename().stem().string();
  fs::path pathExtDw = resultDir / fs::path(originalName + "_ext_dw.bmp");

  DwImage.WriteFileBMP(pathExtDw);
}

void WaterMark::EmbedWaterMarkLocalVariance(const fs::path &pathImage,
                                            const fs::path &pathDw) {

  ImageIO originalImg(pathImage);
  StructBMP &image = originalImg.GetStructImage();

  ImageIO dwImg(pathDw);
  StructBMP &watermark = dwImg.GetStructImage();

  vector<pair<int, double>> variancePixels = CalculaingPixelVariance(image);
  vector<int> orderPixels = SortingVariance(variancePixels);

  size_t bitsDw = watermark.pixels.size() * 8 + 32 + 32 + 32;
  size_t capContainer = orderPixels.size();

  if (bitsDw < capContainer / 2) {
    cout << "Error: Small size digital watermark\n";
    return;
  }

  if (bitsDw > capContainer) {
    cout << "Error: Digital watermark size is very large\n";
    return;
  }

  EmbedDW(orderPixels, image, watermark);

  string originalName = pathImage.filename().stem().string();
  fs::path pathEmbedDw =
      resultDir / fs::path(originalName + "_embed_dw_variance.bmp");

  originalImg.WriteFileBMP(pathEmbedDw);
}

void WaterMark::ExtractWaterMarkLocalVariance(const fs::path &pathImage) {
  ImageIO originalImage;
  originalImage.ReadFileBMP(pathImage);
  StructBMP image = originalImage.GetStructImage();

  vector<pair<int, double>> variancePixels = CalculaingPixelVariance(image);
  vector<int> orderPixels = SortingVariance(variancePixels);

  StructDW watermark = ExtractDW(orderPixels, image);

  ImageIO DwImage;
  DwImage.CreateBMP(watermark.width, watermark.height, watermark.pixels);

  string originalName = pathImage.filename().stem().string();
  fs::path pathExtDw= resultDir / fs::path(originalName + "_ext_dw.bmp");

  DwImage.WriteFileBMP(pathExtDw);
}

int WaterMark::GenerateSeed() {
  random_device rd;
  return rd();
}

int WaterMark::ReadSeed(const fs::path &pathSeed) {
  TextIO txt;
  vector<string> v = txt.Read(pathSeed);

  if (v.empty() || v.size() != 1) {
    cout << "Error: Bad file with seed\n";
    return 0;
  }

  int seed = stoi(v[0]);

  return seed;
}

vector<int> WaterMark::GenerateKey(int seed, size_t size) {
  vector<int> embedPixels(size);
  iota(embedPixels.begin(), embedPixels.end(), 0);

  mt19937 gen(seed);
  shuffle(embedPixels.begin(), embedPixels.end(), gen);

  return embedPixels;
}

void WaterMark::EmbedDW(vector<int> &order, StructBMP &image, StructBMP &watermark) {
  uint32_t widthDw = watermark.infoBmp.biWidth;
  uint32_t heightDw = watermark.infoBmp.biHeight;
  uint32_t sizeDw = watermark.pixels.size();
  size_t index = 0;

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitWidth = (widthDw >> bit) & 1;
    image.pixels[pixel] &= ~(1 << bitPixel);
    image.pixels[pixel] |= (bitWidth << bitPixel);
    index++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitHeight = (heightDw >> bit) & 1;
    image.pixels[pixel] &= ~(1 << bitPixel);
    image.pixels[pixel] |= (bitHeight << bitPixel);
    index++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitDwSize = (sizeDw >> bit) & 1;
    image.pixels[pixel] &= ~(1 << bitPixel);
    image.pixels[pixel] |= (bitDwSize << bitPixel);
    index++;
  }

  for (size_t i = 0; i < watermark.pixels.size(); ++i) {
    uint8_t byteDw = watermark.pixels[i];
    for (int bit = 0; bit < 8; ++bit) {
      int pixel = order[index];
      uint8_t bitDw = (byteDw >> bit) & 1;
      image.pixels[pixel] &= ~(1 << bitPixel);
      image.pixels[pixel] |= (bitDw << bitPixel);
      index++;
    }
  }
}

StructDW WaterMark::ExtractDW(vector<int> &order, StructBMP &image) {
  uint32_t widthDw = 0;
  uint32_t heightDw = 0;
  uint32_t sizeDw = 0;
  size_t index = 0;
  vector<uint8_t> pixelsDw;

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitWidth = (image.pixels[pixel] >> bitPixel) & 1;
    widthDw |= (bitWidth << bit);
    index++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitHeight = (image.pixels[pixel] >> bitPixel) & 1;
    heightDw |= (bitHeight << bit);
    index++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitDw = (image.pixels[pixel] >> bitPixel) & 1;
    sizeDw |= (bitDw << bit);
    index++;
  }

  for (size_t i = 0; i < sizeDw; ++i) {
    uint8_t byteDw = 0;
    for (int bit = 0; bit < 8; ++bit) {
      int pixel = order[index];
      uint8_t bitDw = (image.pixels[pixel] >> bitPixel) & 1;
      byteDw |= (bitDw << bit);
      index++;
    }
    pixelsDw.push_back(byteDw);
  }

  StructDW dw;
  dw.width = widthDw;
  dw.height = heightDw;
  dw.pixels = pixelsDw;

  return dw;
}

vector<pair<int, double>> WaterMark::CalculaingPixelVariance(StructBMP &image) {
  vector<pair<int, double>> variancePixels;
  int width = image.infoBmp.biWidth;
  int height = image.infoBmp.biHeight;

  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      double sum = 0;
      double sumSqr = 0;
      int count = 0;

      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          int py = y + dy;
          int px = x + dx;

          int pixelIndex = py * width + px;
          uint8_t p = image.pixels[pixelIndex];

          sum += p;
          sumSqr += p * p;
          count++;
        }
      }
      double avg1 = sum / count;
      double avg2 = sumSqr / count;
      double variance = avg2 - avg1 * avg1;
      int indexPixel = y * width + x;

      variancePixels.push_back({indexPixel, variance});
    }
  }
  return variancePixels;
}

vector<int>
WaterMark::SortingVariance(vector<pair<int, double>> &variancePixels) {
  sort(variancePixels.begin(), variancePixels.end(),
       [](const auto &a, const auto &b) { return a.second > b.second; });

  vector<int> pixels;
  pixels.reserve(variancePixels.size());

  for (const auto &v : variancePixels)
    pixels.push_back(v.first);

  return pixels;
}
