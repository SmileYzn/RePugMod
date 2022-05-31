#pragma once

enum class TimeUnitType
{
    TIMEUNIT_SECONDS = 0,
    TIMEUNIT_MINUTES = 1,
    TIMEUNIT_HOURS = 2,
    TIMEUNIT_DAYS = 3,
    TIMEUNIT_WEEKS = 4,
    TIMEUNIT_MONTHS = 5,
    TIMEUNIT_YEARS = 6
};

#define SECONDS_IN_MINUTE 60
#define SECONDS_IN_HOUR   3600
#define SECONDS_IN_DAY    86400
#define SECONDS_IN_WEEK   604800
#define SECONDS_IN_MONTH  2592000
#define SECONDS_IN_YEAR   31536000

class CTimeFormat
{
public:
    std::string GetTimeLength(long unitCnt, TimeUnitType Type);
};

extern CTimeFormat gTimeFormat;
