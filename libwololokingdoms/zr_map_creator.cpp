#include "zr_map_creator.h"
#include "string_helpers.h"
#include <miniz.h>

size_t ZRMapCreator::zipWriteCallback(void* context, mz_uint64 offset, const void* buffer, size_t size) {
    std::cout<<"ZRMapCreator::zipWriteCallback("<<offset<<","<<size<<")"<<std::endl;
    const char* char_buffer = reinterpret_cast<const char*>(buffer);
    ZRMapCreator* map_creator = reinterpret_cast<ZRMapCreator*>(context);

    map_creator->output.write(char_buffer, size);
    return size;
}

ZRMapCreator::ZRMapCreator(std::ostream& output): output(output) {
    mz_zip_zero_struct(&mz_handle);
    mz_handle.m_pWrite = ZRMapCreator::zipWriteCallback;
    mz_handle.m_pIO_opaque = this;
    mz_zip_writer_init(&mz_handle, 0);
    std::cout<<"ZRMapCreator::ZRMapCreator()"<<std::endl;
}

ZRMapCreator::~ZRMapCreator () {
    mz_zip_writer_end(&mz_handle);
    std::cout<<"ZRMapCreator::~ZRMapCreator()"<<std::endl;
}

void ZRMapCreator::addFile(const std::string& name, std::istream& content) {
    auto str = concat_stream(content);
    std::cout<<"ZRMapCreator::addFile(" <<name<<", "<<str.length()<<")"<<std::endl;
    mz_zip_writer_add_mem(
        &mz_handle,
        name.c_str(),
        str.c_str(),
        str.length(),
        MZ_NO_COMPRESSION);
}

void ZRMapCreator::end() {
    std::cout<<"ZRMapCreator::end()"<<std::endl;
    mz_zip_writer_finalize_archive(&mz_handle);
}
