#include <iostream>

#include "lab1/ImageProcessor.hpp"

int main()
{
    fs::path resultDir = fs::path(PROJECT_ROOT) / "result";

    if (!fs::exists(resultDir))
        fs::create_directory(resultDir);

    std::unique_ptr<ImageProcessor> imgProc;
    imgProc = std::make_unique<ImageProcessor>();
    fs::path pathDatasets = fs::path(PROJECT_ROOT) / "datasets/1/301.bmp";

    imgProc->ReadBMP(pathDatasets);

    for(size_t i = 1; i <= 8; ++i)
        imgProc->ExtractBitPlane(i);

    imgProc.reset();
}