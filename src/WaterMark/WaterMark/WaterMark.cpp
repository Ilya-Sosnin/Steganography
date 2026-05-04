#include "WaterMark.hpp"

#include <numeric>
#include <random>
#include <utility>

WaterMark::WaterMark() {}
WaterMark::~WaterMark() {}

void WaterMark::EmbedWaterMarkLSB(const fs::path &pathImage,
                                  const fs::path &pathLogo) {
  ImageIO originalImg(pathImage);
  StructBMP &image = originalImg.GetStructImage();

  ImageIO logoImg(pathLogo);
  StructBMP &logo = logoImg.GetStructImage();

  size_t bitsLogo = logo.pixels.size() * 8 + 32 + 32 + 32;
  size_t capContainer = image.pixels.size();

  if (bitsLogo < capContainer / 2) {
    cout << "Error: Small size logo \n";
    return;
  }

  if (bitsLogo > capContainer) {
    cout << "Error: Logo size is very large\n";
    return;
  }

  int seed = GenerateSeed();
  TextIO txtIO;
  fs::path pathSeed = resultDir / fs::path("seed.txt");
  txtIO.Write(pathSeed, to_string(seed));

  vector<int> key = GenerateKey(seed, image.pixels.size());

  EmbedDW(key, image, logo);

  string originalName = pathImage.filename().stem().string();
  fs::path pathEmbedLogo =
      resultDir / fs::path(originalName + "_embed_logo_lsb.bmp");

  originalImg.WriteFileBMP(pathEmbedLogo);
}

void WaterMark::ExtractedMarkLSB(const fs::path &pathSeed,
                                 const fs::path &pathImage) {
  ImageIO originalImage;
  originalImage.ReadFileBMP(pathImage);
  StructBMP image = originalImage.GetStructImage();
  int seed = ReadSeed(pathSeed);

  vector<int> key = GenerateKey(seed, image.pixels.size());

  StructDW dw = ExtractDW(key, image);

  ImageIO logoImage;
  logoImage.CreateBMP(dw.width, dw.height, dw.pixels);

  string originalName = pathImage.filename().stem().string();
  fs::path pathExtLogo = resultDir / fs::path(originalName + "_ext_logo.bmp");

  logoImage.WriteFileBMP(pathExtLogo);
}

void WaterMark::EmbedWaterMarkLocalVariance(const fs::path &pathImage,
                                             const fs::path &pathLogo) {

  ImageIO originalImg(pathImage);
  StructBMP &image = originalImg.GetStructImage();

  ImageIO logoImg(pathLogo);
  StructBMP &logo = logoImg.GetStructImage();

  vector<pair<int, double>> variancePixels = CalculaingPixelVariance(image);
  vector<int> orderPixels = SortingVariance(variancePixels);

  size_t bitsLogo = logo.pixels.size() * 8 + 32 + 32 + 32;
  size_t capContainer = orderPixels.size();

  if (bitsLogo < capContainer / 2) {
    cout << "Error: Small size logo \n";
    return;
  }

  if (bitsLogo > capContainer) {
    cout << "Error: Logo size is very large\n";
    return;
  }

  EmbedDW(orderPixels, image, logo);

  string originalName = pathImage.filename().stem().string();
  fs::path pathEmbedLogo =
      resultDir / fs::path(originalName + "_embed_logo_var.bmp");

  originalImg.WriteFileBMP(pathEmbedLogo);
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

void WaterMark::EmbedDW(vector<int> &order, StructBMP &image, StructBMP &logo) {
  uint32_t widthLogo = logo.infoBmp.biWidth;
  uint32_t heightLogo = logo.infoBmp.biHeight;
  uint32_t sizeLogo = logo.pixels.size();
  size_t index = 0;

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitWidth = (widthLogo >> bit) & 1;
    image.pixels[pixel] &= ~(1 << bitPixel);
    image.pixels[pixel] |= (bitWidth << bitPixel);
    index++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitHeight = (heightLogo >> bit) & 1;
    image.pixels[pixel] &= ~(1 << bitPixel);
    image.pixels[pixel] |= (bitHeight << bitPixel);
    index++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitlogoSize = (sizeLogo >> bit) & 1;
    image.pixels[pixel] &= ~(1 << bitPixel);
    image.pixels[pixel] |= (bitlogoSize << bitPixel);
    index++;
  }

  for (size_t i = 0; i < logo.pixels.size(); ++i) {
    uint8_t byteLogo = logo.pixels[i];
    for (int bit = 0; bit < 8; ++bit) {
      int pixel = order[index];
      uint8_t bitLogo = (byteLogo >> bit) & 1;
      image.pixels[pixel] &= ~(1 << bitPixel);
      image.pixels[pixel] |= (bitLogo << bitPixel);
      index++;
    }
  }
}

StructDW WaterMark::ExtractDW(vector<int> &order, StructBMP &image) {
  uint32_t witdhLogo = 0;
  uint32_t heightLogo = 0;
  uint32_t sizeLogo = 0;
  size_t index = 0;
  vector<uint8_t> pixelsLogo;

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitWidth = (image.pixels[pixel] >> bitPixel) & 1;
    witdhLogo |= (bitWidth << bit);
    index++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitHeight = (image.pixels[pixel] >> bitPixel) & 1;
    heightLogo |= (bitHeight << bit);
    index++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = order[index];
    uint8_t bitLogo = (image.pixels[pixel] >> bitPixel) & 1;
    sizeLogo |= (bitLogo << bit);
    index++;
  }

  for (size_t i = 0; i < sizeLogo; ++i) {
    uint8_t byteLogo = 0;
    for (int bit = 0; bit < 8; ++bit) {
      int pixel = order[index];
      uint8_t bitLogo = (image.pixels[pixel] >> bitPixel) & 1;
      byteLogo |= (bitLogo << bit);
      index++;
    }
    pixelsLogo.push_back(byteLogo);
  }

  StructDW dw;
  dw.width = witdhLogo;
  dw.height = heightLogo;
  dw.pixels = pixelsLogo;

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

