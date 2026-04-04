#include <iostream>

#include "lab1/ImageProcessor.hpp"

int main()
{
    std::unique_ptr<ImageProcessor> imgProc;
    imgProc = std::make_unique<ImageProcessor>();

    std::string path = "../datasets/2/1.bmp";
    imgProc->ReadBMP(path);
}
