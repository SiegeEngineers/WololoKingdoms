#pragma once
#include <string>
#include <iostream>

class ZRMapCreator {
    std::ostream& output;
    void writeLocalFileHeader(std::string name, uint32_t size);
public:
    ZRMapCreator(std::ostream& output): output(output) { }
    void addFile(std::string name, const char* content, uint32_t size);
};
