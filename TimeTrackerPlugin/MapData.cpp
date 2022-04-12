#include "pch.h"
#include "MapData.h"

MapData::MapData()
{
	displayName = "";
	timePlayed = 0;
	AddTimePlayed(0);
}

MapData::MapData(const std::string& name, const long long& time)
{
	displayName = name;
	timePlayed = 0;
	AddTimePlayed(time);
}

void MapData::AddTimePlayed(const long long& newTime)
{
	timePlayed += newTime;
	displayTime = GetTimeString(timePlayed);
}

std::string MapData::GetDisplayTime()
{
	return displayTime;
}
