#include "ppmLoader.h"
#include <util/skeeper/Skepper.h>

void ppmLoader::upload(image& img, const char* filename)
{
    std::ofstream stream(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!stream.is_open()) {
        throw std::exception("file was not open");
    }
    stream << img.magic << (char)0xA << img.width << ' ' << img.height << (char)0xA << img.depth << (char)0xA;
    const int dataSize = img.width * img.height * 3;
    stream.write(img.data, dataSize);
    stream.close();
}

void ppmLoader::download(image& img, const char* filename)
{
    std::ifstream stream(filename, std::ios::in | std::ios::binary);
    if (!stream.is_open()) {
        throw std::exception("file was not open");
    }
    Skepper::skipComment(stream);
    stream >> img.magic;
    Skepper::skipComment(stream);
    stream >> img.width >> img.height >> img.depth;
    Skepper::skipEOL(stream);
    const int dataSize = img.width * img.height * 3;
    img.data = new char[dataSize];
    stream.read(img.data, dataSize);
    stream.close();
}

