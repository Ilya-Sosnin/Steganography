#ifndef WATERMARK_H
#define WATERMARK_H

#include <vector>

using namespace std;

class WaterMark {
public:
  WaterMark();
  ~WaterMark();

private:
  vector<unsigned long> GenerateKey();
  vector<uint8_t> EmbedWaterMarkLSB(vector<uint8_t>& image, vector<uint8_t>& log);
};

#endif