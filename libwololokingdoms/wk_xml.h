#pragma once
#include <ostream>

/**
 * Handles writing the mod XML file for a given patch level.
 */
void write_wk_xml(std::ostream& output, std::string baseModName, int patch_level);
