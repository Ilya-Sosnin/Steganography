#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

#pragma pack(push, 1)
struct BMPFileHeader
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BMPInfo
{
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

class ImageProcessor
{
public:
    ImageProcessor();
    ~ImageProcessor();

    void ReadBMP(const std::string &imagePath);
    void ExtractBitPlane();

private:
    int width{0};
    int height{0};
    
    std::vector<uint8_t> imageBinary;
    std::vector<uint8_t> palette;

    BMPFileHeader headerBMP;
    BMPInfo infoBMP;
};

#endif