#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <iostream>

class ZRMapCreator {
    std::ostream& output;
    std::vector<std::tuple<std::string, uint32_t, uint32_t>> files;
    void writeLocalFileHeader(std::string name, uint32_t size);
    void writeCentralDirectoryHeader(std::string name, uint32_t size, uint32_t offset);
    void writeEndOfDirectory(uint32_t offset, uint32_t size);
public:
    ZRMapCreator(std::ostream& output): output(output) { }
    void addFile(std::string name, const char* content, uint32_t size);
    void end();
};
