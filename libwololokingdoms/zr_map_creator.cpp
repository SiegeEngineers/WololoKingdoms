#include "zr_map_creator.h"
#include "string_helpers.h"
#include <miniz.h>

size_t ZRMapCreator::zipWriteCallback(void* context,
                                      [[maybe_unused]] mz_uint64 offset,
                                      const void* buffer, size_t size) {
  const char* char_buffer = reinterpret_cast<const char*>(buffer);
  ZRMapCreator* map_creator = reinterpret_cast<ZRMapCreator*>(context);

  map_creator->output.write(char_buffer, size);
  return size;
}

ZRMapCreator::ZRMapCreator(std::ostream& output) : output(output) {
  mz_zip_zero_struct(&mz_handle);
  mz_handle.m_pWrite = ZRMapCreator::zipWriteCallback;
  mz_handle.m_pIO_opaque = this;
  mz_zip_writer_init(&mz_handle, 0);
}

ZRMapCreator::~ZRMapCreator() { mz_zip_writer_end(&mz_handle); }

void ZRMapCreator::addFile(const std::string& name, std::istream& content) {
  auto str = concat_stream(content);
  auto size = str.length();
  mz_zip_writer_add_mem_ex(&mz_handle,
                           name.c_str(),      // name
                           str.c_str(), size, // content
                           NULL, 0,           // comment
                           // MZ_ZIP_FLAG_WOLOLOKINGDOMS is a custom flag to
                           // force output of uncompressed size header members
                           // where normally, miniz would omit them
                           MZ_NO_COMPRESSION | MZ_ZIP_FLAG_WOLOLOKINGDOMS,
                           str.length(), 0);
}

void ZRMapCreator::end() { mz_zip_writer_finalize_archive(&mz_handle); }
