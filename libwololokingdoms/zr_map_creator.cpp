#include "zr_map_creator.h"

void ZRMapCreator::writeLocalFileHeader(std::string name, uint32_t size) {
    uint32_t signature = 0x04034b50;
    uint16_t short_dummy = 0;
    char* short_ptr = reinterpret_cast<char*>(&short_dummy);
    output.write(reinterpret_cast<char*>(&signature), sizeof(signature));
    output.write(short_ptr, sizeof(uint16_t)); // version
    output.write(short_ptr, sizeof(uint16_t)); // flag
    output.write(short_ptr, sizeof(uint16_t)); // mtime
    output.write(short_ptr, sizeof(uint16_t)); // compression
    output.write(short_ptr, sizeof(uint16_t)); // mdate
    output.write(reinterpret_cast<char*>(&size), sizeof(size)); // compressed size
    output.write(reinterpret_cast<char*>(&size), sizeof(size)); // uncompressed size
    uint16_t nameLength = name.length();
    output.write(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    output.write(short_ptr, sizeof(uint16_t)); // extra length
    output.write(name.c_str(), size);
}

void ZRMapCreator::addFile(std::string name, const char* content, uint32_t size) {
    writeLocalFileHeader(name, size);
    output.write(content, size);
}
