///Basic: always global
#ifndef __BASIC_H_
#define __BASIC_H_

#include <stdlib.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#endif

//#ifndef TIME_TEST
#define TIME_TEST
//#endif

#define MEMORY_TEST

extern void StartTimer();
extern double GetTimer();

extern void StartTimerAll();
extern double GetTimerAll();

#endif // __BASIC_H_

