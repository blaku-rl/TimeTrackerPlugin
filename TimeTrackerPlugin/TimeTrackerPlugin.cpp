#include "pch.h"
#include "TimeTrackerPlugin.h"
#include <fstream>
#include <algorithm>

BAKKESMOD_PLUGIN(TimeTrackerPlugin, "Track time played in each map", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void TimeTrackerPlugin::onLoad()
{
	using namespace std::placeholders;
	_globalCvarManager = cvarManager;
	started = false;
	maps = {};
	defaultMapNames = {};
	curMap = "";
	std::filesystem::path timeTrackerFolder = gameWrapper->GetDataFolder() / "TimeTrackerData";
	if (!std::filesystem::exists(timeTrackerFolder))
		std::filesystem::create_directory(timeTrackerFolder);
	timeDataFilePath = timeTrackerFolder / "maptimedata.json";
	defaultNamesFilePath = timeTrackerFolder / "defaultmapnames.json";

	shouldUpdateTime = true;
	imGuiTimeDiff = 0;
	imGuiSelectedMap = "";
	tableSortStyle = 0;
	isReversed = false;
	mapNamesSorted = {};
	mapTimesSorted = {};
	ClearBuffer();

	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.Countdown.BeginState", bind(&TimeTrackerPlugin::MapLoad, this, _1));
	gameWrapper->HookEventPost("Function TAGame.GameEvent_Soccar_TA.Destroyed", bind(&TimeTrackerPlugin::MapUnload, this, _1));
	gameWrapper->HookEventPost("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", bind(&TimeTrackerPlugin::MapUnload, this, _1));
	LoadMapData();
	LoadDefaultMapNames();
}

void TimeTrackerPlugin::onUnload()
{
	SaveMapData();
}

void TimeTrackerPlugin::MapLoad(std::string eventName)
{
	if (started) return;
	startTime = std::chrono::steady_clock::now();
	started = true;
	CheckMapName();
}

void TimeTrackerPlugin::MapUnload(std::string eventName)
{
	if (!started || maps.find(curMap) == maps.end()) return;
	maps[curMap].AddTimePlayed(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count());
	started = false;
	curMap = "";
	imGuiTimeDiff = 0;
	SaveMapData();
	SortMapTimes();
}

void TimeTrackerPlugin::CheckMapName()
{
	if (!started) return;

	//Sometimes the data isn't loaded by the on load function. Not sure why, but just check again to bring it in
	if (maps.size() == 0) {
		LoadMapData();
	}
	if (defaultMapNames.size() == 0) {
		LoadDefaultMapNames();
	}

	std::string mapName = gameWrapper->GetCurrentMap();
	if (mapName.empty()) {
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			CheckMapName();
			}, 0.5f);
		return;
	}

	for (int i = 0; i < mapName.size(); i++) {
		mapName[i] = std::tolower(mapName[i]);
	}

	if(maps.find(mapName) == maps.end()){
		std::string displayName = defaultMapNames.find(mapName) == defaultMapNames.end() ? mapName : defaultMapNames[mapName];
		maps[mapName] = MapData(displayName, 0);
		mapNamesSorted.push_back(mapName);
		mapTimesSorted.push_back(mapName);
		SortMapNames();
		SortMapTimes();
	}

	curMap = mapName;
}

void TimeTrackerPlugin::SaveMapData()
{
	nlohmann::json jsonMapData = maps;
	auto out = std::ofstream(timeDataFilePath);
	if (out.is_open()) {
		out << jsonMapData.dump();
	}
	out.close();
}

void TimeTrackerPlugin::LoadMapData()
{
	maps = {};
	mapNamesSorted = {};
	mapTimesSorted = {};

	auto dataIn = std::ifstream(timeDataFilePath);
	nlohmann::json jsonMapData;
	if (dataIn.is_open()) {
		dataIn >> jsonMapData;
	}
	dataIn.close();
	maps = jsonMapData.get<std::map<std::string, MapData>>();
	mapNamesSorted.reserve(maps.size());
	mapTimesSorted.reserve(maps.size());

	for (auto& [rawName, map] : maps) {
		map.AddTimePlayed(0);
		mapNamesSorted.push_back(rawName);
		mapTimesSorted.push_back(rawName);
	}

	SortMapNames();
	SortMapTimes();
}

void TimeTrackerPlugin::SortMapNames()
{
	std::sort(mapNamesSorted.begin(), mapNamesSorted.end(),
		[this](const std::string& mapName1, const std::string mapName2) {
			return maps[mapName1].displayName < maps[mapName2].displayName;
		});
}

void TimeTrackerPlugin::SortMapTimes()
{
	std::sort(mapTimesSorted.begin(), mapTimesSorted.end(),
		[this](const std::string& mapName1, const std::string mapName2) -> bool {
			long long time1 = maps[mapName1].timePlayed;
			long long time2 = maps[mapName2].timePlayed;

			if (!started) {
				return time1 > time2;
			}

			if (mapName1 == curMap) {
				time1 += imGuiTimeDiff;
			}
			else if (mapName2 == curMap) {
				time2 += imGuiTimeDiff;
			}

			return time1 > time2;
		});
}

void TimeTrackerPlugin::LoadDefaultMapNames()
{
	auto dataIn = std::ifstream(defaultNamesFilePath);
	nlohmann::json jsonMapData;
	if (dataIn.is_open()) {
		dataIn >> jsonMapData;
	}
	dataIn.close();
	defaultMapNames = jsonMapData.get<std::map<std::string, std::string>>();
}
