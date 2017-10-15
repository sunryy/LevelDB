#pragma once
#include <string.h>
//Include the appropriate platform specific file below. If you are
//porting to a new platform, see "port_example.h" for documentation
//of what the new port_<platform>.h file must provide.
#if defined(LEVELDB_PLATFORM_POSIX)
#include "port/port_posix.h"
#elif defined(LEVELDB_PLATFORM_CHROMIUM)
#include "port/port_chromium.h"
#elif defined(LEVELDB_PLATFORM_ANDROID)
#include "port/port_android.h"
#elif defined(LEVELDB_PLATFORM_WINDOWS)
#include "port/port_win.h"
#endif