#include "WaterMark.hpp"

#include <numeric>
#include <random>

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

  EmbedLSB(seed, image, logo);

  string originalName = pathImage.filename().stem().string();
  fs::path pathEmbedLogo =
      resultDir / fs::path(originalName + "_embed_logo.bmp");

  originalImg.WriteFileBMP(pathEmbedLogo);
}

void WaterMark::ExtractedMarkLSB(const fs::path &pathSeed,
                                 const fs::path &pathImage) {
  ImageIO originalImage;
  originalImage.ReadFileBMP(pathImage);
  StructBMP image = originalImage.GetStructImage();
  int seed = ReadSeed(pathSeed);
  StructDW dw = ExtractLSB(seed, image);

  ImageIO logoImage;
  logoImage.CreateBMP(dw.width, dw.height, dw.pixels);

  string originalName = pathImage.filename().stem().string();
  fs::path pathExtLogo = resultDir / fs::path(originalName + "_ext_logo.bmp");

  logoImage.WriteFileBMP(pathExtLogo);
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

void WaterMark::EmbedLSB(int seed, StructBMP &image, StructBMP &logo) {
  vector<int> key = GenerateKey(seed, image.pixels.size());

  uint32_t widthLogo = logo.infoBmp.biWidth;
  uint32_t heightLogo = logo.infoBmp.biHeight;
  uint32_t sizeLogo = logo.pixels.size();
  size_t indexKey = 0;

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = key[indexKey];
    uint8_t bitWidth = (widthLogo >> bit) & 1;
    image.pixels[pixel] &= ~(1 << bitPixel);
    image.pixels[pixel] |= (bitWidth << bitPixel);
    indexKey++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = key[indexKey];
    uint8_t bitHeight = (heightLogo >> bit) & 1;
    image.pixels[pixel] &= ~(1 << bitPixel);
    image.pixels[pixel] |= (bitHeight<< bitPixel);
    indexKey++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = key[indexKey];
    uint8_t bitlogoSize = (sizeLogo >> bit) & 1;
    image.pixels[pixel] &= ~(1 << bitPixel);
    image.pixels[pixel] |= (bitlogoSize << bitPixel);
    indexKey++;
  }

  for (size_t i = 0; i < logo.pixels.size(); ++i) {
    uint8_t byteLogo = logo.pixels[i];
    for (int bit = 0; bit < 8; ++bit) {
      int pixel = key[indexKey];
      uint8_t bitLogo = (byteLogo >> bit) & 1;
      image.pixels[pixel] &= ~(1 << bitPixel);
      image.pixels[pixel] |= (bitLogo << bitPixel);
      indexKey++;
    }
  }
}

StructDW WaterMark::ExtractLSB(int seed, StructBMP &image) {
  vector<int> key = GenerateKey(seed, image.pixels.size());

  uint32_t witdhLogo = 0;
  uint32_t heightLogo = 0;
  uint32_t sizeLogo = 0;
  size_t indexKey = 0;
  vector<uint8_t> pixelsLogo;

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = key[indexKey];
    uint8_t bitWidth = (image.pixels[pixel] >> bitPixel) & 1;
    witdhLogo |= (bitWidth << bit);
    indexKey++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = key[indexKey];
    uint8_t bitHeight = (image.pixels[pixel] >> bitPixel) & 1;
    heightLogo |= (bitHeight << bit);
    indexKey++;
  }

  for (int bit = 0; bit < 32; ++bit) {
    int pixel = key[indexKey];
    uint8_t bitLogo = (image.pixels[pixel] >> bitPixel) & 1;
    sizeLogo |= (bitLogo << bit);
    indexKey++;
  }

  for (size_t i = 0; i < sizeLogo; ++i) {
    uint8_t byteLogo = 0;
    for (int bit = 0; bit < 8; ++bit) {
      int pixel = key[indexKey];
      uint8_t bitLogo = (image.pixels[pixel] >> bitPixel) & 1;
      byteLogo |= (bitLogo << bit);
      indexKey++;
    }
    pixelsLogo.push_back(byteLogo);
  }

  StructDW dw;
  dw.width = witdhLogo;
  dw.height = heightLogo;
  dw.pixels = pixelsLogo;

  return dw;
}
