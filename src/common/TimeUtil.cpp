#include "TimeUtil.h"

int32_t ZoneInSeconds(int8_t quaterZone) {
    return (int32_t)quaterZone * (int32_t)QUATER_HOUR_SECONDS;
}

int16_t ZoneInMinutes(int8_t quaterZone) {
    return (int16_t)quaterZone * (int16_t)QUATER_HOUR_MINUTES;
}

// Scan time from format: %y/%m/%d,%H:%M:%S+%TZ
time_t ExtractTime(char *timeString, tmz * stamp) {
    if (sscanf(timeString, "%d/%d/%d,%d:%d:%d%d", &stamp->tm_year, &stamp->tm_mon, &stamp->tm_mday, &stamp->tm_hour, &stamp->tm_min, &stamp->tm_sec, &stamp->quaterZone) > 0) {
        if (stamp->tm_year < 100) stamp->tm_year += 100;
        return mktime(stamp);
    }

    /*
    if (strptime(timeString, "%y/%m/%d,%H:%M:%S", &stamp) != NULL) {
        // adjust for timezone offset which is +/- in 15 minute increments

        *target = mktime(&stamp);

        if (timeZone != NULL) {
            char *zPointer = strchr(timeString, '+');
            if (zPointer == NULL) {
                zPointer = strchr(timeString, '-');
            }
            if (zPointer != NULL) {
                *timeZone = atoi(zPointer) * (15 * 60);
            }
        }
        return true;
    }
    */
    return 0;
}