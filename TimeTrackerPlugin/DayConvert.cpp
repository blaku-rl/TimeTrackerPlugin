#include "pch.h"
#include "DayConvert.h"

std::string GetTimeString(const long long& time)
{
	std::string timeString = "";

	long long days = time / DAY_MULTIPLIER;
	timeString += std::to_string(days) + " day" + (days == 1 ? "" : "s") + ", ";
	long long remainingTime = time - (days * DAY_MULTIPLIER);

	long long hours = remainingTime / HOUR_MULTIPLIER;
	timeString += std::to_string(hours) + " hour" + (hours == 1 ? "" : "s") + ", ";
	remainingTime = remainingTime - (hours * HOUR_MULTIPLIER);

	long long minutes = remainingTime / MINUTE_MULTIPLIER;
	timeString += std::to_string(minutes) + " minute" + (minutes == 1 ? "" : "s") + ", ";
	remainingTime = remainingTime - (minutes * MINUTE_MULTIPLIER);

	long long seconds = remainingTime / SECOND_MULTIPLIER;
	timeString += std::to_string(seconds) + " second" + (seconds == 1 ? "" : "s") + ", ";

	return timeString;
}
