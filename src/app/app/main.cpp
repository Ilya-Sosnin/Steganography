#include <iostream>

#include "lab1/ImageProcessor.hpp"

int main()
{
    std::unique_ptr<ImageProcessor> imgProc;
    imgProc = std::make_unique<ImageProcessor>();
    fs::path pathDatasets = fs::path(PROJECT_ROOT) / "datasets/2/1.bmp";

    imgProc->ReadBMP(pathDatasets);

    for(size_t i = 1; i <= 8; ++i)
        imgProc->ExtractBitPlane(i);

    imgProc.reset();
}