#pragma once
#include <stdint.h>

// https://github.com/SFTtech/openage/blob/master/doc/media/drs-files.md

struct DrsHeader {
	char copyright[40];
	char version[4];
	char ftype[12];
	int32_t table_count;
	int32_t file_offset;
};

struct DrsTableInfo {
	uint8_t file_type;
	char file_extension[3];
	int32_t file_info_offset;
	int32_t num_files;
};

struct DrsFileInfo {
	int32_t file_id;
	int32_t file_data_offset;
	int32_t file_size;
};
