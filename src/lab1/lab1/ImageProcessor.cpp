#include "ImageProcessor.hpp"

#include <fstream>
#include <bitset>

ImageProcessor::ImageProcessor() {}
ImageProcessor::~ImageProcessor() {}

void ImageProcessor::ReadBMP(const std::string &imagePath)
{
    std::ifstream file;
    file.open(imagePath, std::ios_base::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file " << imagePath << std::endl;
        return;
    }

    file.read(reinterpret_cast<char *>(&headerBMP), sizeof(headerBMP));
    file.read(reinterpret_cast<char *>(&infoBMP), sizeof(infoBMP));

    uint32_t numColors = infoBMP.biClrUsed ? infoBMP.biClrUsed : (1 << infoBMP.biBitCount);

    palette.resize(numColors);
    file.read(reinterpret_cast<char *>(palette.data()), numColors * sizeof(uint32_t));

    width = infoBMP.biWidth;
    height = infoBMP.biHeight;

    imageBinary.resize(width * height);
    file.read(reinterpret_cast<char*> (imageBinary.data()), imageBinary.size());

    file.close();
}

void ImageProcessor::ExtractBitPlane()
{
}