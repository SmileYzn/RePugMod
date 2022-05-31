#include "precompiled.h"

CTimeFormat gTimeFormat;

std::string CTimeFormat::GetTimeLength(long unitCnt, TimeUnitType Type)
{
    static std::string Output;

    if (unitCnt > 0)
    {
        long yearCnt = 0, monthCnt = 0, weekCnt = 0, dayCnt = 0, hourCnt = 0, minuteCnt = 0, secondCnt = 0;

        switch (Type)
        {
            case TimeUnitType::TIMEUNIT_SECONDS:
            {
                secondCnt = unitCnt;
                break;
            }
            case TimeUnitType::TIMEUNIT_MINUTES:
            {
                secondCnt = unitCnt * SECONDS_IN_MINUTE;
                break;
            }
            case TimeUnitType::TIMEUNIT_HOURS:
            {
                secondCnt = unitCnt * SECONDS_IN_HOUR;
                break;
            }
            case TimeUnitType::TIMEUNIT_DAYS:
            {
                secondCnt = unitCnt * SECONDS_IN_DAY;
                break;
            }
            case TimeUnitType::TIMEUNIT_WEEKS:
            {
                secondCnt = unitCnt * SECONDS_IN_WEEK;
                break;
            }
            case TimeUnitType::TIMEUNIT_MONTHS:
            {
                secondCnt = unitCnt * SECONDS_IN_MONTH;
                break;
            }
            case TimeUnitType::TIMEUNIT_YEARS:
            {
                secondCnt = unitCnt * SECONDS_IN_YEAR;
                break;
            }
        }

        yearCnt = secondCnt / SECONDS_IN_YEAR;
        secondCnt -= (yearCnt * SECONDS_IN_YEAR);

        monthCnt = secondCnt / SECONDS_IN_MONTH;
        secondCnt -= (monthCnt * SECONDS_IN_MONTH);

        weekCnt = secondCnt / SECONDS_IN_WEEK;
        secondCnt -= (weekCnt * SECONDS_IN_WEEK);

        dayCnt = secondCnt / SECONDS_IN_DAY;
        secondCnt -= (dayCnt * SECONDS_IN_DAY);

        hourCnt = secondCnt / SECONDS_IN_HOUR;
        secondCnt -= (hourCnt * SECONDS_IN_HOUR);

        minuteCnt = secondCnt / SECONDS_IN_MINUTE;
        secondCnt -= (minuteCnt * SECONDS_IN_MINUTE);

        int maxElementIdx = -1;

        std::string timeElement[7];

		if (yearCnt > 0)
		{
            timeElement[++maxElementIdx] = std::to_string(yearCnt) + " " + (yearCnt == 1 ? _T("year") : _T("years"));
		}

		if (monthCnt > 0)
		{
            timeElement[++maxElementIdx] = std::to_string(monthCnt) + " " + (monthCnt == 1 ? _T("month") : _T("months"));
		}

		if (weekCnt > 0)
		{
            timeElement[++maxElementIdx] = std::to_string(weekCnt) + " " + (weekCnt == 1 ? _T("week") : _T("weeks"));
		}

		if (dayCnt > 0)
		{
            timeElement[++maxElementIdx] = std::to_string(dayCnt) + " " + (dayCnt == 1 ? _T("day") : _T("days"));
		}

		if (hourCnt > 0)
		{
            timeElement[++maxElementIdx] = std::to_string(hourCnt) + " " + (hourCnt == 1 ? _T("hour") : _T("hours"));
		}

		if (minuteCnt > 0)
		{
            timeElement[++maxElementIdx] = std::to_string(minuteCnt) + " " + (minuteCnt == 1 ? _T("minute") : _T("minutes"));
		}

		if (secondCnt > 0)
		{
            timeElement[++maxElementIdx] = std::to_string(secondCnt) + " " + (secondCnt == 1 ? _T("second") : _T("seconds"));
		}

        switch (maxElementIdx)
        {
            case 0:
            {
                Output = timeElement[0];
                break;
            }
            case 1:
            {
                Output = timeElement[0] + " " + _T("and") + " " + timeElement[1];
                break;
            }
            case 2:
            {
                Output = timeElement[0] + ", " + timeElement[1] + " " + _T("and") + " " +  timeElement[2];
                break;
            }
            case 3:
            {
                Output = timeElement[0] + ", " + timeElement[1] + ", " + timeElement[2] + " " + _T("and") + " " +  timeElement[3];
                break;
            }
            case 4:
            {
                Output = timeElement[0] + ", " + timeElement[1] + ", " + timeElement[2] + ", " + timeElement[3] + " " + _T("and") + " " +  timeElement[4];
                break;
            }
            case 5:
            {
                Output = timeElement[0] + ", " + timeElement[1] + ", " + timeElement[2] + ", " + timeElement[3] + ", " + timeElement[4] + " " + _T("and") + " " +  timeElement[5];
                break;
            }
            case 6:
            {
                Output = timeElement[0] + ", " + timeElement[1] + ", " + timeElement[2] + ", " + timeElement[3] + ", " + timeElement[4] + ", " + timeElement[5] + " " + _T("and") + " " + timeElement[6];
                break;
            }
        }
    }
    else
    {
        Output = _T("Permanently");
    }

    return Output;
}
