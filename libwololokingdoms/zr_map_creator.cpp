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

struct CentralDirectoryHeader {
  uint32_t signature = 0x02014b50;
  uint16_t version = 0;
  uint16_t min_version = 0;
  uint16_t flags = 0;
  uint16_t compression = 0;
  uint16_t mtime = 0;
  uint16_t mdate = 0;
  uint32_t crc32 = 0;
  uint32_t compressed_size = 0;
  uint32_t uncompressed_size = 0;
  uint16_t name_length = 0;
  uint16_t extra_length = 0;
  uint16_t comment_length = 0;
  uint16_t disk = 0;
  uint16_t internal_attributes = 0;
  uint32_t external_attributes = 0;
  uint32_t offset = 0;
};

struct EndOfDirectory {
  uint32_t signature = 0x06054b50;
  uint16_t disk = 0;
  uint16_t cd_disk = 0;
  uint16_t disk_records_count = 0;
  uint16_t records_count = 0;
  uint32_t cd_size = 0;
  uint32_t cd_offset = 0;
  uint16_t comment_length = 0;
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

void ZRMapCreator::writeCentralDirectoryHeader(std::string name, uint32_t size, uint32_t offset) {
    CentralDirectoryHeader header;
    header.compressed_size = size;
    header.uncompressed_size = size;
    header.name_length = name.length();
    header.offset = offset;

    output.write(reinterpret_cast<char*>(&header), sizeof(header));
    output.write(name.c_str(), size);
    // no extra to write
}

void ZRMapCreator::writeEndOfDirectory(uint32_t cd_offset, uint32_t cd_size) {
    EndOfDirectory header;
    header.disk_records_count = files.size();
    header.records_count = files.size();
    header.cd_size = cd_size;
    header.cd_offset = cd_offset;

    output.write(reinterpret_cast<char*>(&header), sizeof(header));
}

void ZRMapCreator::addFile(std::string name, const char* content, uint32_t size) {
    uint32_t offset = output.tellp();
    files.push_back(std::tuple(name, size, offset));
    writeLocalFileHeader(name, size);
    output.write(content, size);
}

void ZRMapCreator::end() {
    auto cd_offset = output.tellp();
    for (auto& it : files) {
        writeCentralDirectoryHeader(std::get<0>(it), std::get<1>(it), std::get<2>(it));
    }
    auto cd_size = output.tellp() - cd_offset;
    writeEndOfDirectory(cd_offset, cd_size);
}
