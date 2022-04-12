#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include <chrono>
#include "MapData.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class TimeTrackerPlugin: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow
{
	virtual void onLoad();
	virtual void onUnload();

	void MapLoad(std::string eventName);
	void MapUnload(std::string eventName);
	void CheckMapName();
	void SaveMapData();
	void LoadMapData();
	void SortMapNames();
	void SortMapTimes();
	void LoadDefaultMapNames();

	std::chrono::time_point<std::chrono::steady_clock> startTime;
	bool started;
	long long count;
	std::map<std::string, MapData> maps;
	std::map<std::string, std::string> defaultMapNames;
	std::string curMap;
	std::filesystem::path timeDataFilePath;
	std::filesystem::path defaultNamesFilePath;

	bool shouldUpdateTime;
	long long imGuiTimeDiff;
	std::string imGuiSelectedMap;
	std::string newMapName;
	int tableSortStyle;
	bool isReversed;
	std::vector<std::string> mapNamesSorted;
	std::vector<std::string> mapTimesSorted;

	// Inherited via PluginSettingsWindow
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;
	void UpdateName();
};

