#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

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

    void ReadBMP(const fs::path &imagePath);
    void WriteBMP(const std::string imageName, std::vector<uint8_t> &v);
    void ReadTXT(const fs::path &txtPath);
    void WriteTxt(const std::string txtName, std::vector<uint8_t> &v);
    void ExtractBitPlane(int bitNum);
    void EmbedTextIntoBitPlane(int bitNum);
    void ExtractMessage(int bitNum);

private:
    int width{512};
    int height{512};

    size_t minSizeMessage{30720};
    // size_t maxSizeMessage{32764};

    std::string originalImageName;
    fs::path originalImageDir;
    fs::path resultDir = fs::path(PROJECT_ROOT).concat("/result");
    // fs::path txtPath = fs::path(PROJECT_ROOT) / "message/";

    std::vector<uint32_t> palette;
    std::vector<uint8_t> imageBinary;
    std::vector<uint8_t> imagePlane;
    std::vector<uint8_t> imageEmbed;
    std::vector<uint8_t> privateMessage;
    std::vector<uint8_t> extractedMessage;

    BMPFileHeader headerBMP;
    BMPInfo infoBMP;
};

#endif