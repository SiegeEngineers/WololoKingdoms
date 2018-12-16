#include "zr_map_creator.h"

struct LocalFileHeader {
  uint32_t signature = 0x04034b50;
  uint16_t version = 0;
  uint16_t flag = 0;
  uint16_t mtime = 0;
  uint16_t compression = 0;
  uint16_t mdate = 0;
  uint32_t compressed_size = 0;
  uint32_t uncompressed_size = 0;
  uint16_t name_length = 0;
  uint16_t extra_length = 0;
};

void ZRMapCreator::writeLocalFileHeader(std::string name, uint32_t size) {
    LocalFileHeader header;
    header.compressed_size = size;
    header.uncompressed_size = size;
    header.name_length = name.length();

    output.write(reinterpret_cast<char*>(&header), sizeof(header));
    output.write(name.c_str(), size);
    // no extra to write
}

void ZRMapCreator::addFile(std::string name, const char* content, uint32_t size) {
    writeLocalFileHeader(name, size);
    output.write(content, size);
}
