#pragma once
#include "nlohmann/json.hpp"
#include "DayConvert.h"

struct MapData {
private:
	std::string displayTime;

public:
	std::string displayName;
	long long timePlayed;

	MapData();
	MapData(const std::string& name, const long long& time);

	void AddTimePlayed(const long long& newTime);
	std::string GetDisplayTime();
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapData, displayName, timePlayed)