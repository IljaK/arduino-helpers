#pragma once
#include <Arduino.h>
#include "TimeUtil.h"
#include "Util.h"

class TimeManager {
private:
    static uint32_t syncMicroTS;
    static time_t launchTimeStamp;
    static time_t currentTimeStamp;
    static int8_t quaterZone;

    static uint32_t getSecondsSinceUpdate();

public:
    static void LateLoop();
    static time_t GetLocalTime();
    static time_t GetUtcTime();
    static uint32_t GetOnlineSeconds();
    static time_t GetTimeSeconds(tm *tmStruct);

    static void UpdateSystemTime(tmz *tmStruct);
};