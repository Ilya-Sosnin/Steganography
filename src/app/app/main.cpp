#include <iostream>

#include "lab1/ImageProcessor.hpp"

int main()
{
    std::unique_ptr<ImageProcessor> imgProc;
    imgProc = std::make_unique<ImageProcessor>();

    int bitNum = 8;
    fs::path pathDatasets = fs::path(PROJECT_ROOT).concat("/datasets/2/1.bmp");
    fs::path pathMessage = fs::path(PROJECT_ROOT).concat("/message/private_message.txt");

    imgProc->ReadBMP(pathDatasets);

    // for(size_t i = 1; i <= 8; ++i)

    imgProc->ExtractBitPlane(bitNum);

    imgProc->ReadTXT(pathMessage);

    //for(size_t i = 1; i <= 8; ++i)
    imgProc->EmbedTextIntoBitPlane(bitNum);

    imgProc->ExtractMessage(bitNum);

    imgProc.reset();
}