#pragma once

#ifdef __APPLE__
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 600
#endif
#include "png.hpp"