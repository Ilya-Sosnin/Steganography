#include "ImageProcessor.hpp"

#include <fstream>
#include <bitset>
#include <filesystem>

ImageProcessor::ImageProcessor() {}

ImageProcessor::~ImageProcessor() {}

void ImageProcessor::ReadBMP(const fs::path &imagePath)
{
    if (!fs::exists(imagePath))
    {
        std::cout << "Error: File " << imagePath << " does not exist" << std::endl;
        return;
    }

    std::ifstream file(imagePath, std::ios_base::binary);
    if (!file.is_open())
    {
        std::cout << "Error: Cannot open file " << imagePath << std::endl;
        return;
    }

    originalImageName = imagePath.stem().string();
    originalImageDir = imagePath.parent_path().filename();

    file.read(reinterpret_cast<char *>(&headerBMP), sizeof(headerBMP));
    file.read(reinterpret_cast<char *>(&infoBMP), sizeof(infoBMP));

    uint32_t numColors = infoBMP.biClrUsed ? infoBMP.biClrUsed : (1 << infoBMP.biBitCount);

    palette.resize(numColors);
    file.read(reinterpret_cast<char *>(palette.data()), numColors * sizeof(uint32_t));

    width = int(infoBMP.biWidth);
    height = int(infoBMP.biHeight);

    imageBinary.resize(width * height);
    file.read(reinterpret_cast<char *>(imageBinary.data()), imageBinary.size());

    file.close();
}

void ImageProcessor::WriteBMP(const std::string imageName, std::vector<uint8_t> &v)
{
    fs::path outDir = resultDir / originalImageDir;

    if (!fs::exists(outDir))
        fs::create_directories(outDir);

    fs::path outFile = outDir / imageName;

    std::ofstream file(outFile, std::ios_base::binary);
    if (!file.is_open())
    {
        std::cout << "Error: Cannot open file " << outFile << std::endl;
        return;
    }

    file.write(reinterpret_cast<char *>(&headerBMP), sizeof(headerBMP));
    file.write(reinterpret_cast<char *>(&infoBMP), sizeof(infoBMP));
    file.write(reinterpret_cast<char *>(palette.data()), palette.size() * sizeof(uint32_t));
    file.write(reinterpret_cast<char *>(v.data()), v.size());

    file.close();
}

void ImageProcessor::ReadTXT(const fs::path &txtPath)
{
    if (!fs::exists(txtPath))
    {
        std::cout << "Error: File " << txtPath << " does not exist" << std::endl;
        return;
    }

    uintmax_t size = fs::file_size(txtPath);
    if (size < minSizeMessage)
    {
        std::cout << "Error: The message must be at least 30 bytes. Current size: "
                  << size << " bytes" << std::endl;
        return;
    }

    std::ifstream file(txtPath, std::ios::binary);
    if (!file)
    {
        std::cout << "Error: Cannot open file " << txtPath << std::endl;
        return;
    }

    privateMessage.resize(size);

    file.read(reinterpret_cast<char *>(privateMessage.data()), size);
    file.close();
}

void ImageProcessor::ExtractBitPlane(int bitNum)
{
    if (bitNum < 1 || bitNum > 8)
    {
        std::cout << "Warning: Bit position must be between 1 and 8" << std::endl;
        return;
    }

    bitNum -= 1;
    imagePlane.resize(width * height);

    for (size_t i = 0; i < imageBinary.size(); ++i)
    {
        int bit = (imageBinary[i] >> (bitNum)) & 1;
        imagePlane[i] = bit ? 255 : 0;
    }

    std::string imageName = originalImageName + "_plane_bit_" +
                            std::to_string(bitNum) + ".bmp";
    WriteBMP(imageName, imagePlane);
}

void ImageProcessor::EmbedTextIntoBitPlane(int bitNum)
{
    if (bitNum < 1 || bitNum > 8)
    {
        std::cout << "Warning: Bit position must be between 1 and 8" << std::endl;
        return;
    }

    bitNum -= 1;
    size_t byteImage = 0;
    size_t sizeWrittenMessage = 0;
    imageEmbed = imageBinary;

    for (auto byteMessage : privateMessage)
    {
        if (byteImage >= imageEmbed.size())
            break;

        for (int i = 0; i < 8; ++i)
        {
            int bitMessage = (byteMessage >> i) & 1;
            imageEmbed[byteImage] &= ~(1 << bitNum);
            imageEmbed[byteImage] |= (bitMessage << bitNum);
            byteImage++;
        }
        sizeWrittenMessage++;
    }

    std::cout << sizeWrittenMessage * 8
              << " of " << privateMessage.size() * 8
              << " bits written" << std::endl;

    std::string imageName = originalImageName + "_embed_bit_" +
                            std::to_string(bitNum) + ".bmp";
    WriteBMP(imageName, imageEmbed);
}
