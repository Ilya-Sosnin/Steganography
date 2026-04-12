#include <charconv>
#include <string_view>

#include "lab1/ImageProcessor.hpp"

int main(int argc, char *argv[]) {
  // if (argc != 4) {
  //   std::cout << "Usage: "
  //             << "./app.exe -<flag options> <path to image> <path to message>\n";

  //   std::cout << "Files must be located relative to the program root!\n";

  //   std::cout << "Example: "
  //             << "./app.exe 1 /datasets/2/1.bmp /message/private_message.txt\n";
  //   return 0;
  // }

  // std::string bitNum = argv[1];

  // int bitNumber;
  // auto [ptr, ec] =
  //     std::from_chars(bitNum.data(), bitNum.data() + bitNum.size(), bitNumber);

  // if (ec != std::errc() || ptr != bitNum.data() + bitNum.size()) {
  //   std::cout << "<bit_position> must be a number\n";
  //   return 0;
  // }

  // if (bitNumber < 1 || bitNumber > 8) {
  //   std::cout << "<bit_position> must be between 1 and 8\n";
  //   return 0;
  // }

  // fs::path imageFile = fs::path(PROJECT_ROOT).concat(argv[2]);
  // if (!fs::exists(imageFile)) {
  //   std::cout << "<path to image> = " << imageFile << " does not exist\n";
  //   return 0;
  // }

  // fs::path messageFile = fs::path(PROJECT_ROOT).concat(argv[3]);
  // if (!fs::exists(messageFile)) {
  //   std::cout << "<path to message> = " << messageFile << " does not exist\n";
  //   return 0;
  // }

  // fs::path resultDir = fs::path(PROJECT_ROOT).concat("/result");
  // if (!fs::exists(resultDir))
  //   fs::create_directory(resultDir);

  // std::unique_ptr<ImageProcessor> imgProc;
  // imgProc = std::make_unique<ImageProcessor>(bitNumber, imageFile, messageFile,
  //                                            resultDir);
  // imgProc->ProcessorImage();
  // imgProc.reset();
}