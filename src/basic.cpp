#include "basic.h"

#ifdef _WIN32
static double PCFreq = 0.0;
static __int64 timerStart = 0;
#else
static struct timeval timerStart;
#endif

#ifdef _WIN32
static double PCFreqAll = 0.0;
static __int64 timerStartAll = 0;
#else
static struct timeval timerStartAll;
#endif

void StartTimer()
{
#ifdef _WIN32
    LARGE_INTEGER li;

    if (!QueryPerformanceFrequency(&li))
    {
        printf("QueryPerformanceFrequency failed!\n");
    }

    PCFreq = (double)li.QuadPart/1000.0;
    QueryPerformanceCounter(&li);
    timerStart = li.QuadPart;
#else
    gettimeofday(&timerStart, NULL);
#endif
}

// time elapsed in second or ms
static double Unit = 1000.0;

double GetTimer()
{
#ifdef _WIN32
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double)(li.QuadPart-timerStart)/PCFreq/Unit;
#else
    struct timeval timerStop, timerElapsed;
    gettimeofday(&timerStop, NULL);
    timersub(&timerStop, &timerStart, &timerElapsed);
    return (double)(timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0) / Unit;
#endif
}

void StartTimerAll()
{
#ifdef _WIN32
    LARGE_INTEGER li;

    if (!QueryPerformanceFrequency(&li))
    {
        printf("QueryPerformanceFrequency failed!\n");
    }

    PCFreqAll = (double)li.QuadPart/1000.0;
    QueryPerformanceCounter(&li);
    timerStartAll = li.QuadPart;
#else
    gettimeofday(&timerStartAll, NULL);
#endif
}

// time elapsed in ms
double GetTimerAll()
{
#ifdef _WIN32
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double)(li.QuadPart-timerStartAll)/PCFreqAll/Unit;
#else
    struct timeval timerStop, timerElapsed;
    gettimeofday(&timerStop, NULL);
    timersub(&timerStop, &timerStartAll, &timerElapsed);
    return (double)(timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0 ) / Unit;
#endif
}

