#pragma once
#include <miniz.h>
#include <string>
#include <vector>
#include <tuple>
#include <iostream>

class ZRMapCreator {
    mz_zip_archive mz_handle;
    std::ostream& output;
    static size_t zipWriteCallback(void* context, mz_uint64 offset, const void* buffer, size_t size);

public:
    ZRMapCreator(std::ostream& output);
    ~ZRMapCreator();
    void addFile(const std::string& name, std::istream& content);
    void end();
};
