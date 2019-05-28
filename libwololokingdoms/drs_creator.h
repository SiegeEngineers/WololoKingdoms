#pragma once
#include <fs.h>
#include <iostream>
#include <map>
#include <vector>

enum DRSTableType {
  Bina = 0x62696e61, // 'bina'
  Slp = 0x736c7020, // 'slp '
  Wav = 0x77617620 // 'wav '
};

class DRSCreatorTableEntry {
  uint32_t id_;
  uint32_t offset_ = 0;
  uint32_t size_ = 0;
  // Owned by this object
  std::istream* stream_ = nullptr;
  fs::path filename_;

public:
  DRSCreatorTableEntry (uint32_t id, std::istream* data);
  DRSCreatorTableEntry (uint32_t id, const fs::path& filename);
  DRSCreatorTableEntry(DRSCreatorTableEntry&& entry);
  DRSCreatorTableEntry(const DRSCreatorTableEntry&) = delete;
  DRSCreatorTableEntry& operator=(DRSCreatorTableEntry const&);
  ~DRSCreatorTableEntry ();

  void writeMeta (std::ostream& target);
  void writeContent (std::ostream& target);
};

class DRSCreatorTable {
  std::vector<DRSCreatorTableEntry> files_;
  uint32_t offset_ = 0;
  bool wrote_contents_ = false;
public:
  void addFile(uint32_t id, std::istream* data);
  void addFile(uint32_t id, const fs::path& filename);
  void setOffset (uint32_t offset);
  size_t getMetaSize ();
  void writeMeta(DRSTableType type, std::ostream& target);
  void writeFileMeta(std::ostream& target);
  void writeFileContents(std::ostream& target);
};

class DRSCreator {
  std::ostream& output_;
  std::map<DRSTableType, DRSCreatorTable> tables_;

public:
  DRSCreator(std::ostream& output);

  void addFile(DRSTableType table, uint32_t id, std::istream* data);
  void addFile(DRSTableType table, uint32_t id, const fs::path& filename);
  void commit();
};
