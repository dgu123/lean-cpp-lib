// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define LEAN_FAST_STL

#include <lean/depconfig.h>
#include <lean/lean.h>

#include <lean/time/highres_timer.h>

#include <iostream>
#include <random>

#ifdef LEAN_DEBUG_BUILD
	#define DEBUG_DENOMINATOR 100
#else
	#define DEBUG_DENOMINATOR 1
#endif

inline void print_results(const char *test, const char *name1, double time1, const char *name2, double time2)
{
	std::cout << test << ":" << std::endl
		<< " -> " << name1 << ": " << time1 << "    (" << time2 / time1 << ")" << std::endl
		<< " -> " << name2 << ": " << time2 << "    (" << time1 / time2 << ")" << std::endl
		<< std::endl;
}
