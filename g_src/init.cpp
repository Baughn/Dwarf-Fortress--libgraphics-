#include "platform.h"
#include <string.h>
#include <math.h>
#include <iosfwd>
#include <iostream>
#include <ios>
#include <streambuf>
#include <istream>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <zlib.h>

#include "svector.h"
using std::string;

#include "endian.h"

#include "files.h"

#include "textlines.h"

#include "enabler.h"

#include "basics.h"

#include "init.h"

extern enablerst enabler;


init_displayst::init_displayst()
{
	flag.set_size_on_flag_num(INIT_DISPLAY_FLAGNUM);
	windowed=INIT_DISPLAY_WINDOW_PROMPT;

	dwarf_frame_rate.QuadPart=enabler.qpfr.QuadPart/100;

	partial_print_count=0;
}