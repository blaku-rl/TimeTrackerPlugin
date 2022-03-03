#pragma once
#include "nlohmann/json.hpp"
#include "Constants.h"

struct MapData {
private:
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