#include "TimeManager.h"

uint32_t TimeManager::syncMicroTS = 0;
time_t TimeManager::launchTimeStamp = 0;
time_t TimeManager::currentTimeStamp = 0;
int8_t TimeManager::quaterZone = 0;

void TimeManager::LateLoop()
{
 	uint32_t seconds = getSecondsSinceUpdate();
	if (seconds > 0) {
        currentTimeStamp += seconds;
        syncMicroTS += (seconds * MICROS_IN_SECOND);

        /*
        DebugHandler::outWrite("Time: ");
        time_t localTime = GetLocalTime();
        DebugHandler::outWrite(ctime(&localTime));
        DebugHandler::outWriteEnd();
        DebugHandler::outWrite("Online: ");
        DebugHandler::outWrite(GetOnlineSeconds());
        DebugHandler::outWriteEnd();
        */

	}
}

time_t TimeManager::GetTimeSeconds(tm *tmStruct)
{
#if defined(ESP32) || defined(ARDUINO_ARCH_SAMD)
	return mktime(tmStruct);
#else
	return mk_gmtime(tmStruct);
#endif
}

time_t TimeManager::GetLocalTime()
{
    return GetUtcTime() + (time_t)quaterZone;
}

time_t TimeManager::GetUtcTime()
{
    return currentTimeStamp + getSecondsSinceUpdate();
}

uint32_t TimeManager::GetOnlineSeconds()
{
    return GetUtcTime() - launchTimeStamp;
}

uint32_t TimeManager::getSecondsSinceUpdate()
{
    uint32_t microsTS = micros();
	return (microsTS - syncMicroTS) / MICROS_IN_SECOND;
}

void TimeManager::UpdateSystemTime(tmz *tmStruct)
{
    syncMicroTS = micros();
    uint32_t prevTS = currentTimeStamp;
    currentTimeStamp = GetTimeSeconds(tmStruct) - ZoneInSeconds(tmStruct->quaterZone);

    launchTimeStamp += difftime(currentTimeStamp, prevTS);
}
