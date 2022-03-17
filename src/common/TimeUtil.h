#pragma once
#include "Arduino.h"
#include <time.h>

#ifndef ONE_HOUR
#define ONE_HOUR 3600
#endif

constexpr uint16_t QUATER_HOUR_SECONDS = (uint16_t)ONE_HOUR / (uint16_t)4u;
constexpr uint16_t QUATER_HOUR_MINUTES = (uint16_t)60 / (uint16_t)4u;
constexpr uint32_t MICROS_IN_SECOND = 1000000ul;

struct tmz: public tm
{
    int8_t quaterZone = 0;
};

struct timez_t
{
    time_t utcStamp = 0;
    int8_t quaterZone = 0;
};

extern int32_t ZoneInSeconds(int8_t quaterZone);
extern int16_t ZoneInMinutes(int8_t quaterZone);

extern time_t ExtractTime(char *timeString, tmz * stamp);
extern void PrintDateTime(Print *print, tm *stamp);