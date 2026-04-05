#include "ImageProcessor.hpp"

#include <fstream>
#include <bitset>
#include <filesystem>

ImageProcessor::ImageProcessor() {}
ImageProcessor::~ImageProcessor() {}

void ImageProcessor::ReadBMP(const fs::path &imagePath)
{
    std::ifstream file;
    file.open(imagePath, std::ios_base::binary);
    if (!file.is_open())
    {
        std::cout << "Error: Cannot open file " << imagePath << std::endl;
        return;
    }

    originalImageName = imagePath.stem().string();

    file.read(reinterpret_cast<char *>(&headerBMP), sizeof(headerBMP));
    file.read(reinterpret_cast<char *>(&infoBMP), sizeof(infoBMP));

    uint32_t numColors = infoBMP.biClrUsed ? infoBMP.biClrUsed : (1 << infoBMP.biBitCount);

    palette.resize(numColors);
    file.read(reinterpret_cast<char *>(palette.data()), numColors * sizeof(uint32_t));

    width = infoBMP.biWidth;
    height = infoBMP.biHeight;

    imageBinary.resize(width * height);
    file.read(reinterpret_cast<char *>(imageBinary.data()), imageBinary.size());

    file.close();
}

void ImageProcessor::WriteBMP(const std::string imageName, std::vector<uint8_t> &v)
{
    std::ofstream file;
    file.open(imageName, std::ios_base::binary);
    if (!file.is_open())
    {
        std::cout << "Error: Cannot open file " << imageName << std::endl;
        return;
    }

    file.write(reinterpret_cast<char *>(&headerBMP), sizeof(headerBMP));
    file.write(reinterpret_cast<char *>(&infoBMP), sizeof(infoBMP));
    file.write(reinterpret_cast<char *>(palette.data()), palette.size() * sizeof(uint32_t));
    file.write(reinterpret_cast<char *>(v.data()), v.size());
    file.close();
}

void ImageProcessor::ExtractBitPlane(int bitNum)
{
    if (bitNum < 1 || bitNum > 8)
    {
        std::cout << "Warning: Bit position must be between 1 and 8" << std::endl;
        return;
    }

    imagePlane.resize(width * height);
    for (size_t i = 0; i < imageBinary.size(); ++i)
    {
        int bit = (imageBinary[i] >> (bitNum - 1)) & 1;
        imagePlane[i] = bit ? 255 : 0;
    }

    std::string imageName = originalImageName + "_plane_bit_num" +
                            std::to_string(bitNum) + ".bmp";
    WriteBMP(imageName, imagePlane);
}