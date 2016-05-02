#ifndef FIX_H
#define FIX_H
#include "genie/dat/DatFile.h"
#include <map>

namespace wololo {

struct Fix {
	void (*patch)(genie::DatFile*, std::map<int, std::string>*);
	std::string name;
};

}



#endif // FIX_H
