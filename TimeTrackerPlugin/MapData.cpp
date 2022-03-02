#include "pch.h"
#include "MapData.h"

MapData::MapData()
{
	displayName = "";
	timePlayed = 0;
	
	AddTimePlayed(0);
}

MapData::MapData(std::string name, long long time)
{
	displayName = name;
	timePlayed = 0;

	AddTimePlayed(time);
}

void MapData::AddTimePlayed(long long newTime)
{
	timePlayed += newTime;

	long long days = timePlayed / DAY_MULTIPLIER;
	long long remainingTime = timePlayed - (days * DAY_MULTIPLIER);

	long long hours = timePlayed / HOUR_MULTIPLIER;
	remainingTime = remainingTime - (hours * HOUR_MULTIPLIER);

	long long minutes = remainingTime / MINUTE_MULTIPLIER;
	remainingTime = remainingTime - (minutes * MINUTE_MULTIPLIER);

	long long seconds = remainingTime / SECOND_MULTIPLIER;

	displayTime = std::to_string(days) + " days, " + std::to_string(hours) + " hours, " + std::to_string(minutes) + " minutes, " + std::to_string(seconds) + " seconds";
}

std::string MapData::GetDisplayTime()
{
	return displayTime;
}
