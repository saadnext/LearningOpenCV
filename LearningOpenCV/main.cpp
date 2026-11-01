/*
===============================================================================
    ASCII Art Generator - C++ + OpenCV
    -----------------------------------
    Author  : Saad Rabia
    Version : 1.0
    License : MIT
    Date    : 2025

    Description:
    ------------
    This program converts any image into a colored ASCII art representation 
    directly displayed in the terminal. It uses OpenCV to process image pixels 
    and ANSI escape codes to colorize characters according to the original 
    pixel RGB values.

    The image is passed on the command line, so there is no need to edit and
    rebuild the source to convert a different picture.

    Requirements:
    -------------
    - OpenCV 4.x or later
    - Terminal supporting 24-bit ANSI colors
    - C++17 compiler

    Example usage:
    --------------
    > g++ -std=c++17 main.cpp -o ascii -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
    > ./ascii path/to/image.jpg
===============================================================================
*/

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX  // keep windows.h from breaking std::min/std::max
#endif
#include <windows.h>
#endif

using namespace cv;

namespace {

// ASCII ramp sorted from brightest (left) to darkest (right).
// More characters = higher detail in brightness representation.
constexpr char kAsciiRamp[] =
    " .`'^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$@█▓▒░";

// Number of entries in the ramp (the trailing '\0' is not a character).
constexpr std::size_t kRampSize = std::size(kAsciiRamp) - 1;

// Desired output width (number of characters per line).
constexpr int kConsoleWidth = 220;

// Character aspect ratio (fine-tune this if the output looks stretched).
constexpr double kCharAspect = 1.0;

std::string programName(const char* argv0) {
    const std::string path = argv0 ? argv0 : "LearningOpenCV";
    const std::size_t slash = path.find_last_of("\\/");
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

void printUsage(std::ostream& out, const char* argv0) {
    const std::string name = programName(argv0);

    out << "ASCII Art Generator - C++ + OpenCV\n"
        << "\n"
        << "Usage:\n"
        << "  " << name << " <image>\n"
        << "\n"
        << "Arguments:\n"
        << "  <image>       Path to the input image. Quote it if the path\n"
        << "                contains spaces.\n"
        << "\n"
        << "Options:\n"
        << "  -h, --help    Show this help and exit.\n"
        << "\n"
        << "Examples:\n"
        << "  " << name << " Resources/ronaldoAndMessi.jpg\n"
        << "  " << name << " \"C:\\Users\\me\\Pictures\\my photo.png\"\n"
        << "\n"
        << "Exit codes:\n"
        << "  0  success\n"
        << "  1  the image could not be loaded\n"
        << "  2  invalid command line (missing image or unknown option)\n";
}

// Reads the file as bytes and decodes it in memory. cv::imread takes a narrow
// path on Windows, so this also works for paths with non-ASCII characters.
bool loadImage(const std::string& path, Mat& image) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }

    const std::vector<unsigned char> bytes{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
    if (bytes.empty()) {
        return false;
    }

    image = imdecode(bytes, IMREAD_COLOR);  // always BGR, 3 channels
    return !image.empty();
}

// Converts the image to ANSI-colored ASCII art. The result is returned as one
// string so it can later be written to a file instead of the terminal.
std::string toAsciiArt(const Mat& img) {
    const int imageWidth = img.cols;
    const int imageHeight = img.rows;

    // Automatically calculate height based on image aspect ratio.
    const int consoleHeight = static_cast<int>(
        (imageHeight / static_cast<double>(imageWidth)) * kConsoleWidth / kCharAspect
    );

    // Compute scaling factors between image pixels and console cells.
    const double scaleX = static_cast<double>(imageWidth) / kConsoleWidth;
    const double scaleY = static_cast<double>(imageHeight) / (consoleHeight * kCharAspect);

    // Each cell costs an ANSI color prefix plus one character; reserving keeps
    // large images from constantly reallocating.
    std::string art;
    art.reserve(static_cast<std::size_t>(kConsoleWidth) * (consoleHeight + 1) * 16);

    for (int y = 0; y < consoleHeight; ++y) {
        for (int x = 0; x < kConsoleWidth; ++x) {
            // Map console coordinates to image coordinates (with safety bounds).
            const int srcX = std::min(static_cast<int>(x * scaleX), imageWidth - 1);
            const int srcY = std::min(static_cast<int>(y * scaleY), imageHeight - 1);

            // Access pixel color (BGR order in OpenCV).
            const Vec3b pixel = img.at<Vec3b>(srcY, srcX);
            const int r = pixel[2];
            const int g = pixel[1];
            const int b = pixel[0];

            // Compute perceived brightness (using Rec.709 luminance formula).
            const float brightness = 0.2126f * r + 0.7152f * g + 0.0722f * b;

            // Map brightness (0-255) to ASCII ramp index.
            const int index = static_cast<int>(brightness * (kRampSize - 1) / 255.0f);

            // Emit the character with the pixel's original RGB color.
            art += "\033[38;2;";
            art += std::to_string(r);
            art += ';';
            art += std::to_string(g);
            art += ';';
            art += std::to_string(b);
            art += 'm';
            art += kAsciiRamp[index];
        }

        art += '\n';
    }

    // Reset color formatting at the end.
    art += "\033[0m";
    return art;
}

}  // namespace

int main(int argc, char* argv[]) {
#if defined(_WIN32)
    // Make the block characters in the ramp render instead of mojibake.
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::string imagePath;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "-h" || arg == "--help" || arg == "/?") {
            printUsage(std::cout, argv[0]);
            return 0;
        }

        // Reserved for future options (--width, --chars, --save, ...).
        if (arg.size() > 1 && arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n\n";
            printUsage(std::cerr, argv[0]);
            return 2;
        }

        if (!imagePath.empty()) {
            std::cerr << "Too many arguments: " << arg << "\n\n";
            printUsage(std::cerr, argv[0]);
            return 2;
        }

        imagePath = arg;
    }

    if (imagePath.empty()) {
        printUsage(std::cerr, argv[0]);
        return 2;
    }

    Mat img;
    if (!loadImage(imagePath, img)) {
        std::cerr << "Error: could not open image: \"" << imagePath << "\"\n";
        return 1;
    }

    std::cout << toAsciiArt(img);
    std::cout.flush();
    return 0;
}
