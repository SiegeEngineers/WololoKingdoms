#ifndef FIX_H
#define FIX_H
#include "genie/dat/DatFile.h"

namespace wololo {

struct Fix {
	void (*patch)(genie::DatFile*);
	std::string name;
};

}



#endif // FIX_H
