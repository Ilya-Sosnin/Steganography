#include "CLI/CLI.hpp"

#include "steganography/ContainerProcessor.hpp"

int main(int argc, char *argv[]) {
  CLI::App app{};

  int bitNumber = 0;
  string mode;
  fs::path image;
  fs::path message;

  app.add_option("--mode", mode)->required();
  app.add_option("--bit", bitNumber)->required();
  app.add_option("--image", image)->required();
  auto msgOpt = app.add_option("--message", message);

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  if (bitNumber < 1 || bitNumber > 8) {
    std::cout << "--bit must be between 1 and 8\n";
    return -1;
  }

  fs::path imagePath = fs::path(PROJECT_ROOT) / image;
  if (!fs::exists(imagePath)) {
    std::cout << "--image does not exist\n";
    return -1;
  }

  if (mode == "embed") {
    if (!msgOpt->count()) {
      std::cout << "---message required for embed\n";
      return -1;
    }
  }

  ContainerProcessor containerProc;

  if (mode == "plane") {
    containerProc.ExtBitPlane(bitNumber, imagePath);
  } else if (mode == "embed") {
    if (!msgOpt->count()) {
      std::cout << "--message required for embed\n";
      return -1;
    }
    fs::path messagePath = fs::path(PROJECT_ROOT) / message;
    if (!fs::exists(messagePath)) {
      std::cout << "--message does not exist\n";
      return -1;
    }
    containerProc.EmbeddingData(bitNumber, imagePath, messagePath);
  } else if (mode == "extract") {
    containerProc.ExtMessage(bitNumber, imagePath);
  } else {
    cout << "Error: unknown mode\n";
    return -1;
  }

  return 0;
}