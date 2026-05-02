#include "WaterMark.hpp"

#include <numeric>
#include <random>

WaterMark::WaterMark() {}
WaterMark::~WaterMark() {}

void WaterMark::EmbedWaterMarkLSB(const fs::path &pathImage,
                                  const fs::path &pathLogo) {
  ImageIO originalImg;
  ImageIO logoImg;

  vector<uint8_t> logo = logoImg.Read(pathLogo);
  if (logo.empty())
    return;

  vector<uint8_t> image = originalImg.Read(pathImage);
  if (image.empty())
    return;

  if (logo.size() * 8 < image.size() / 2) {
    cout << "Error: Small size logo\n";
    return;
  }

  if(logo.size() * 8 >= image.size()) {
    cout << "Error: Logo size is very large\n"; 
    return;
  }

  int seed = GenerateSeed();
  TextIO txtIO;
  fs::path pathSeed = PROJECT_ROOT / fs::path("results/seed.txt");
  txtIO.Write(pathSeed, to_string(seed));
  
  EmbedLSB(seed, image, logo);
  fs::path pathEmbedLogo = fs::path("embed_logo.bmp");
  originalImg.Write(pathEmbedLogo, image);
}

int WaterMark::GenerateSeed() {
  random_device rd;
  return rd();
}

vector<int> WaterMark::GenerateKey(int seed, size_t size) {
  vector<int> embedPixels(size);
  iota(embedPixels.begin(), embedPixels.end(), 0);

  mt19937 gen(seed);
  shuffle(embedPixels.begin(), embedPixels.end(), gen);

  return embedPixels;
}

void WaterMark::EmbedLSB(int seed, vector<uint8_t> &image,
                                    vector<uint8_t> &logo) {
  vector<int> key = GenerateKey(seed, image.size());

  size_t indexKey = 0;
  for(size_t i = 0; i < logo.size(); ++i){
    uint8_t byteLogo = logo[i];
    for(int bit = 0; bit < 8; ++bit) {
      int pixel = key[indexKey];
      uint8_t bitLogo = (byteLogo >> bit) & 1;
      image[pixel] &= ~(1 << bitNum);
      image[pixel] |= (bitLogo << bitNum);
      indexKey++;
    }
  }
}
