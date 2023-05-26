#pragma once

#define WIN32_LEAN_AND_MEAN

#include <cassert>
#include <Shlobj.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <inttypes.h>

#include "external/loguru/loguru.hpp"
#include "external/inipp/inipp/inipp.h"
#include "external/length-disassembler/headerOnly/ldisasm.h"