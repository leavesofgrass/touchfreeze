#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0400
#define STRICT

#include <atlbase.h>
extern CComModule _Module;
#include <atlwin.h>
#include <shellapi.h>