#include "drs_creator.h"
#include <cassert>
#include <cstring>
#include <exception>
#include <fstream>

DRSCreatorTableEntry::DRSCreatorTableEntry (uint32_t id, std::istream* data)
    : id_(id), stream_(data) {
}

DRSCreatorTableEntry::DRSCreatorTableEntry (uint32_t id, const fs::path& filename)
    : id_(id), filename_(filename) {
}

DRSCreatorTableEntry::DRSCreatorTableEntry (DRSCreatorTableEntry&& entry)
    :
    id_(entry.id_),
    stream_(entry.stream_),
    filename_(std::move(entry.filename_)) {
  entry.stream_ = nullptr;
}

DRSCreatorTableEntry::~DRSCreatorTableEntry () {
  // may be moved out.
  if (stream_) delete stream_;
}

void DRSCreatorTableEntry::writeMeta (std::ostream& target) {
  // must have been written to file
  assert(offset_ > 0);
  target.write(reinterpret_cast<char*>(&id_), 4);
  target.write(reinterpret_cast<char*>(&offset_), 4);
  target.write(reinterpret_cast<char*>(&size_), 4);
}

void DRSCreatorTableEntry::writeContent (std::ostream& target) {
  offset_ = target.tellp();
  if (stream_) {
    target << stream_->rdbuf();
  } else {
    std::ifstream stream (filename_, std::ios::binary);
    target << stream.rdbuf();
    stream.close();
  }
  size_ = static_cast<uint32_t>(target.tellp()) - offset_;
}

void DRSCreatorTable::addFile (uint32_t id, std::istream* data) {
  files_.push_back(DRSCreatorTableEntry(id, data));
}

void DRSCreatorTable::addFile (uint32_t id, const fs::path& filename) {
  files_.push_back(DRSCreatorTableEntry(id, filename));
}

void DRSCreatorTable::setOffset (uint32_t offset) {
  offset_ = offset;
}

size_t DRSCreatorTable::getMetaSize () {
  return files_.size() * (3 * sizeof(uint32_t));
}

void DRSCreatorTable::writeMeta (DRSTableType type, std::ostream& target) {
  assert(offset_ > 0);
  target.write(reinterpret_cast<char*>(&type), 4);
  target.write(reinterpret_cast<char*>(&offset_), 4);
  auto num_files = files_.size();
  target.write(reinterpret_cast<char*>(&num_files), 4);
}

void DRSCreatorTable::writeFileMeta (std::ostream& target) {
  assert(wrote_contents_);
  for (auto& file : files_) {
    file.writeMeta(target);
  }
}

void DRSCreatorTable::writeFileContents (std::ostream& target) {
  for (auto& file : files_) {
    file.writeContent(target);
    if (!target) {
      throw std::system_error(errno, std::system_category(), "failed to write file");
    }
  }
  wrote_contents_ = true;
}

DRSCreator::DRSCreator(std::ostream& output) : output_(output) {
}

void DRSCreator::addFile (DRSTableType table, uint32_t id, std::istream* data) {
  tables_[table].addFile(id, data);
}

void DRSCreator::addFile (DRSTableType table, uint32_t id, const fs::path& filename) {
  tables_[table].addFile(id, filename);
}

void DRSCreator::commit () {
  output_.write("Copyright (c) 1997 Ensemble Studios.\x1a\0\0", 40);
  output_.write("1.00", 4);
  output_.write("tribe\0\0\0\0\0\0", 12);
  auto num_tables = tables_.size();
  auto files_offset = static_cast<uint32_t>(output_.tellp()) + 8 + 12 * num_tables;
  for (auto& table : tables_) {
    table.second.setOffset(files_offset);
    files_offset += table.second.getMetaSize();
  }
  output_.write(reinterpret_cast<char*>(&num_tables), 4);
  output_.write(reinterpret_cast<char*>(&files_offset), 4);

  auto meta_offset = output_.tellp();
  auto meta_size = files_offset - meta_offset;
  auto dummy_buffer = new char[meta_size];
  // Easy to recognise if there is a bug
  memset(dummy_buffer, 'q', meta_size);
  output_.write(dummy_buffer, meta_size);
  delete[] dummy_buffer;

  for (auto& table : tables_) {
    table.second.writeFileContents(output_);
  }

  auto end_offset = output_.tellp();
  output_.seekp(meta_offset);

  for (auto& table : tables_) {
    table.second.writeMeta(table.first, output_);
  }
  for (auto& table : tables_) {
    table.second.writeFileMeta(output_);
  }

  output_.seekp(end_offset);
}
