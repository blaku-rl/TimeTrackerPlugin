#pragma once
#include "nlohmann/json.hpp"

struct MapData {
private:
	static const int DAY_MULTIPLIER = 86400000;
	static const int HOUR_MULTIPLIER = 3600000;
	static const int MINUTE_MULTIPLIER = 60000;
	static const int SECOND_MULTIPLIER = 1000;
	std::string displayTime;

public:
	std::string displayName;
	long long timePlayed;

	MapData();
	MapData(std::string name, long long time);

	void AddTimePlayed(long long newTime);
	std::string GetDisplayTime();
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData, displayName, timePlayed)