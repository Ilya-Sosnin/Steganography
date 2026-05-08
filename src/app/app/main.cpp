#include "CLI/CLI.hpp"

#include "ContainerProcessor/ContainerProcessor.hpp"
#include "WaterMark/WaterMark.hpp"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  CLI::App app{};

  string mode;
  fs::path image;
  fs::path dw;
  fs::path seed;

  app.add_option("--mode", mode, "Mode: embed1, embed2, ext1, ext2")->required();
  app.add_option("--image", image, "Path to image");
  app.add_option("--dw", dw, "Path to watermark (logo)");
  app.add_option("--seed", seed, "Path to seed file");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  if (mode != "embed1" && mode != "embed2" &&
      mode != "ext1" && mode != "ext2") {
    cout << "Error: unknown mode\n";
    cout << "Available modes: embed1, embed2, ext1, ext2\n";
    return -1;
  }

  if (mode == "embed1" || mode == "embed2") {
    if (image.empty() || dw.empty()) {
      cout << "Error: --image and --dw required for embedding\n";
      return -1;
    }
  }

  if (mode == "ext1") {
    if (image.empty() || seed.empty()) {
      cout << "Error: --image and --seed required for LSB extraction\n";
      return -1;
    }
  }

  if (mode == "ext2") {
    if (image.empty()) {
      cout << "Error: --image required for extraction\n";
      return -1;
    }
  }

  if (!image.empty() && !fs::exists(image)) {
    cout << "Error: Image file does not exist\n";
    return -1;
  }

  if ((mode == "embed1" || mode == "embed2") && !fs::exists(dw)) {
    cout << "Error: Watermark file does not exist\n";
    return -1;
  }

  if (mode == "ext1" && !fs::exists(seed)) {
    cout << "Error: Seed file does not exist\n";
    return -1;
  }

  WaterMark mark;

  if (mode == "embed1") {
    mark.EmbedWaterMarkLSB(image, dw);
  } else if (mode == "embed2") {
    mark.EmbedWaterMarkLocalVariance(image, dw);
  } else if (mode == "ext1") {
    mark.ExtractedMarkLSB(seed, image);
  } else if (mode == "ext2") {
    mark.ExtractWaterMarkLocalVariance(image);
  }

  return 0;
}

// int main(int argc, char *argv[]) {
//   CLI::App app{};

//   int bitNumber = 0;
//   string mode;
//   fs::path image;
//   fs::path message;

//   app.add_option("--mode", mode)->required();
//   app.add_option("--bit", bitNumber)->required();
//   app.add_option("--image", image)->required();
//   auto msgOpt = app.add_option("--message", message);

//   try {
//     app.parse(argc, argv);
//   } catch (const CLI::ParseError &e) {
//     return app.exit(e);
//   }

//   if (bitNumber < 1 || bitNumber > 8) {
//     std::cout << "--bit must be between 1 and 8\n";
//     return -1;
//   }

//   if (!fs::exists(image)) {
//     std::cout << "--image does not exist\n";
//     return -1;
//   }

//   if (mode == "embed") {
//     if (!msgOpt->count()) {
//       std::cout << "---message required for embed\n";
//       return -1;
//     }
//   }

//   ContainerProcessor containerProc;

//   if (mode == "plane") {
//     containerProc.ExtBitPlane(bitNumber, image);
//   } else if (mode == "embed") {
//     if (!msgOpt->count()) {
//       std::cout << "--message required for embed\n";
//       return -1;
//     }
    
//     if (!fs::exists(message)) {
//       std::cout << "--message does not exist\n";
//       return -1;
//     }
//     containerProc.EmbeddingData(bitNumber, message, image);
//   } else if (mode == "extract") {
//     containerProc.ExtMessage(bitNumber, image);
//   } else {
//     cout << "Error: unknown mode\n";
//     return -1;
//   }

//   return 0;
// }