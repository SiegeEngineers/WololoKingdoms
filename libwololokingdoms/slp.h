/// Header only library for working with SLP files.
///
/// Currently supports only read/writing SLP files and
/// duplicating SLP frames.
///
/// The OpenAge project has documentation about the SLP file format:
/// https://github.com/SFTtech/openage/blob/master/doc/media/slp-files.md

#pragma once

#include <iostream>
#include <vector>

/// Reads 4 bytes in little-endian order from `in`, constructs
/// a big-ending 32-bit int from them, and returns the int.
/// @param in input stream from which the slp is read.
int32_t read_int(std::istream& in) {
  int32_t integer = 0;
  for (int i = 0; i != 4; ++i) { integer |= ((int32_t)in.get()) << 8 * i; }
  return integer;
}

/// Reads 4 bytes in little-endian order from `in`, constructs
/// a big-ending 32-bit unsigned int from them, and returns the int.
/// @param in input stream from which the slp is read.
uint32_t read_uint(std::istream& in) {
  uint32_t integer = 0;
  for (int i = 0; i != 4; ++i) { integer |= ((uint32_t)in.get()) << 8 * i; }
  return integer;
}

/// Writes the 32-bit big-endian integer as 4 little-endian ordered bytes to
/// `out`.
/// @param integer the integer to write.
/// @param out output stream to which the slp is copied.
void write_int(int32_t integer, std::ostream& out) {
  int32_t byte3 = integer >> 24;
  integer ^= byte3 << 24;
  int32_t byte2 = integer >> 16;
  integer ^= byte2 << 16;
  int32_t byte1 = integer >> 8;
  integer ^= byte1 << 8;
  int32_t byte0 = integer;
  out << (char)byte0;
  out << (char)byte1;
  out << (char)byte2;
  out << (char)byte3;
}

/// Writes the 32-bit big-endian unsigned integer as 4 little-endian ordered
/// bytes to `out`.
/// @param integer the integer to write.
/// @param out output stream to which the slp is copied.
void write_uint(uint32_t integer, std::ostream& out) {
  uint32_t byte3 = integer >> 24;
  integer ^= byte3 << 24;
  uint32_t byte2 = integer >> 16;
  integer ^= byte2 << 16;
  uint32_t byte1 = integer >> 8;
  integer ^= byte1 << 8;
  uint32_t byte0 = integer;
  out << (char)byte0;
  out << (char)byte1;
  out << (char)byte2;
  out << (char)byte3;
}

/// Represents header information for an SLP file.
struct slp_header {
  /// The SLP version description. "2.0N" for classic Aoe2 files.
  char version[4];
  /// The number of frames in the SLP.
  int32_t num_frames;
  /// A comment, usually "ArtDesk 1.00 SLP Writer" for Aoe2 SLPs.
  char comment[24];
};

/// Reads an slp header from `in` and returns a struct representing it.
/// @param in input stream from which the slp is read.
slp_header read_header(std::istream& in) {
  slp_header header;
  for (int i = 0; i != 4; ++i) { header.version[i] = in.get(); }
  header.num_frames = read_int(in);
  for (int i = 0; i != 24; ++i) { header.comment[i] = in.get(); }
  return header;
}

/// Writes an slp header to an output stream.
/// @param header the header to write.
/// @param out output stream to which the slp is copied.
void write_header(slp_header* header, std::ostream& out) {
  for (int i = 0; i != 4; ++i) { out << header->version[i]; }
  write_int(header->num_frames, out);
  for (int i = 0; i != 24; ++i) { out << header->comment[i]; }
}

/// The number of Bytes in an slp frame info struct.
const int FRAME_INFO_NUM_BYTES = 32;

/// Represents information for a single slp frame.
struct slp_frame_info {
  /// Points to the position of the first byte of this frame's command table.
  uint32_t cmd_table_offset;
  /// Points to the position of the first byte of this frame's outline table.
  uint32_t outline_table_offset;
  /// Unused.
  uint32_t palette_offset;
  /// An originally unused varible repurposed as a Frame Palette ID.
  /// Used only for HD and DE, not for the original Aoe2.
  uint32_t properties;
  // width and height are i32 in the OpenAge docs, u32 in Tevious's docs.
  /// The width of the SLP's image, in pixels.
  int32_t width;
  /// The height of the SLP's image, in pixels.
  int32_t height;
  /// The frame's x anchor point, representing the horizontal center of the
  /// frame.
  int32_t hotspot_x;
  /// The frame's y anchor point, representing the vertical center of the frame.
  int32_t hotspot_y;
};

/// Reads slp frame information from in.
/// Requires that the header information of in has already been read.
/// @param in input stream from which the slp is read.
/// @returns a vector containing slp frame information, where the `i`th element
/// is the information for the `i`th frame.
std::vector<slp_frame_info> read_frame_info(int32_t num_frames,
                                            std::istream& in) {
  std::vector<slp_frame_info> frames;
  for (int i = 0; i != num_frames; ++i) {
    slp_frame_info frame;
    frame.cmd_table_offset = read_uint(in);
    frame.outline_table_offset = read_uint(in);
    frame.palette_offset = read_uint(in);
    frame.properties = read_uint(in);
    frame.width = read_int(in);
    frame.height = read_int(in);
    frame.hotspot_x = read_int(in);
    frame.hotspot_y = read_int(in);
    frames.push_back(frame);
  }
  return frames;
}

/// Write slp frame information to out.
/// @param frame_info a vector containing slp frame information to be written,
///     where the `i`th element is the information for the `i`th frame.
/// @param out the output stream to which the slp is written.
void write_frame_info(std::vector<slp_frame_info>& frame_info,
                      std::ostream& out) {
  for (slp_frame_info frame : frame_info) {
    write_uint(frame.cmd_table_offset, out);
    write_uint(frame.outline_table_offset, out);
    write_uint(frame.palette_offset, out);
    write_uint(frame.properties, out);
    write_int(frame.width, out);
    write_int(frame.height, out);
    write_int(frame.hotspot_x, out);
    write_int(frame.hotspot_y, out);
  }
}

/// Represents the data portion of an SLP frame, consisting of an
/// outline table, command table, and drawing commands that follow
/// the command table.
///
/// This struct is a basic implementation that serves only the
/// duplication of frames. The outline table and drawing commands
/// are not structured beyond their raw bytes, as modifying them is
/// unnecessary for frame duplication. A future implementation may
/// provide structure to these elements, so the `char` type of their
/// fields should not be relied upon.
struct frame_data {
  /// The outline table of an slp.
  /// The number of elements is equal to 4 times the height of the slp,
  /// as each element consists of 4 bytes.
  std::vector<char> outline_table;
  /// The command table of an slp.
  /// The number of elements is equal to the height of the slp,
  /// as each element consists of 4 bytes that are read as an unsigned int.
  std::vector<uint32_t> command_table;
  /// The drawing command bytes that succeed the command table.
  std::vector<char> remaining_bytes;
};

/// Reads `num_frames` worth of frame data from the input stream `in`.
/// Uses `frame_infos` for the outline table offset, command table offset, and
/// height of each frame. Returns a vector containing the read data.
/// @param num_frames the number of frames to read.
/// @param frame_infos the frame information for each frame, size must equal
/// `num_frames`.
/// @param in the input stream from which the slp is read.
std::vector<frame_data>
read_frame_data(int32_t num_frames, std::vector<slp_frame_info>& frame_infos,
                std::istream& in) {
  std::vector<frame_data> frames(num_frames);
  // Reads all but the final frame.
  for (int i = 0; i != num_frames - 1; ++i) {
    auto fi = &frame_infos[i];
    auto num_bytes = 4 * fi->height;
    for (int _ = 0; _ != num_bytes; ++_) { frames[i].outline_table.push_back(in.get()); }
    for (int _ = 0; _ != fi->height; ++_) { frames[i].command_table.push_back(read_uint(in)); }
    auto finext = &frame_infos[i + 1];
    // The number of bytes for this frame after its command table.
    auto num_data_bytes = finext->outline_table_offset - (fi->cmd_table_offset + num_bytes);
    for (int _ = 0; _ != num_data_bytes; ++_) { frames[i].remaining_bytes.push_back(in.get()); }
  }
  // Reads the final frame.
  {
    auto i = num_frames - 1;
    auto fi = &frame_infos[i];
    auto num_bytes = 4 * fi->height;
    for (int _ = 0; _ != num_bytes; ++_) { frames[i].outline_table.push_back(in.get()); }
    for (int _ = 0; _ != fi->height; ++_) { frames[i].command_table.push_back(read_uint(in)); }
    // Reads until the end of the stream.
    while (in.good()) {
      char byte = in.get();
      // Avoids appending the EoF byte.
      if (in.good()) { frames[i].remaining_bytes.push_back(byte); }
    }
  }
  return frames;
}

/// Writes an slp file containing `frames` to an output stream.
/// @param frames the frames of the slp file.
/// @out the output stream to which the slp is written.
void write_frame_data(std::vector<frame_data>& frames, std::ostream& out) {
  for (auto& frame : frames) {
    for (char byte : frame.outline_table) { out << byte; }
    for (uint32_t integer : frame.command_table) { write_uint(integer, out); }
    for (char byte : frame.remaining_bytes) { out << byte; }
  }
  // Writes the eof byte.
  char byte = -1;
  out << byte;
}

/// Represents an slp file.
struct slp {
  /// Header information for the slp file.
  slp_header header;
  /// Information describing each frame of the slp file, size must equal
  /// the number of frames specified by the header.
  std::vector<slp_frame_info> frame_info;
  /// The outline table, command table, and drawing commands of each
  /// frame of the slp file, size must equal the number of frames
  /// specified by the header.
  std::vector<::frame_data> frame_data;
};

/// Reads an slp file from `in` and returns a struct representing it.
/// @param in the input stream from which the slp is read.
slp read_slp(std::istream& in) {
  slp file;
  file.header = read_header(in);
  file.frame_info = read_frame_info(file.header.num_frames, in);
  file.frame_data = read_frame_data(file.header.num_frames, file.frame_info, in);
  return file;
}

/// Writes an slp file to the output stream out.
/// @param slp_file the slp to write.
/// @param out output stream to which the slp is written.
void write_slp(slp& slp_file, std::ostream& out) {
  write_header(&slp_file.header, out);
  write_frame_info(slp_file.frame_info, out);
  write_frame_data(slp_file.frame_data, out);
}

/// Duplicates frame i of slp_file, appending the frame to the end of the slp.
/// Mutates slp_file to contain the duplicated frame.
/// @param slp_file the slp to modify.
/// @param i the index of the frame to copy, must be contained within the slp.
void duplicate_frame(slp& slp_file, int i) {
  // Creates space for the new frame information.
  for (auto& info : slp_file.frame_info) {
    info.outline_table_offset += FRAME_INFO_NUM_BYTES;
    info.cmd_table_offset += FRAME_INFO_NUM_BYTES;
  }

  // Adds the new frame information.
  auto frame_info = slp_file.frame_info[i];
  auto final_frame_info = slp_file.frame_info[slp_file.header.num_frames - 1];
  auto final_frame = slp_file.frame_data[slp_file.header.num_frames - 1];

  // Adjusts the command table offsets to account for the new frame information.
  for (auto& frame_data : slp_file.frame_data) {
    for (int i = 0; i != frame_data.command_table.size(); ++i) {
      frame_data.command_table[i] += FRAME_INFO_NUM_BYTES;
    }
  }

  frame_info.outline_table_offset = final_frame_info.cmd_table_offset +
                                    4 * final_frame_info.height +
                                    final_frame.remaining_bytes.size();

  auto cmd_offset_duplicate = frame_info.outline_table_offset + 4 * frame_info.height;
  auto cmd_offset_difference = cmd_offset_duplicate - frame_info.cmd_table_offset;
  frame_info.cmd_table_offset = cmd_offset_duplicate;
  slp_file.frame_info.push_back(frame_info);

  // Adds the frame data.
  auto frame_data = slp_file.frame_data[i];
  for (int j = 0; j != frame_data.command_table.size(); ++j) {
    frame_data.command_table[j] += cmd_offset_difference;
  }
  slp_file.frame_data.push_back(frame_data);

  // Adds the additional frame to the header.
  ++slp_file.header.num_frames;
}
