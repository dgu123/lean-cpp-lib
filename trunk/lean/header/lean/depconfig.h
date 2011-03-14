/*****************************************************/
/* lean Dependency Config       (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_DEPENDENCY_CONFIG
#define LEAN_DEPENDENCY_CONFIG

// Get rid of macros
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Disable warnings
#define _CRT_SECURE_NO_WARNINGS // Using _standard_ library
#define _SCL_SECURE_NO_WARNINGS

// Optimization
#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0

#endif