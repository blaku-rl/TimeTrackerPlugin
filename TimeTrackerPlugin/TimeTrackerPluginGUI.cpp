#include "pch.h"
#include "TimeTrackerPlugin.h"
#include "imgui/imgui_internal.h"

std::string TimeTrackerPlugin::GetPluginName() {
	return "TimeTrackerPlugin";
}

void TimeTrackerPlugin::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> TimeTrackerPlugin
void TimeTrackerPlugin::RenderSettings() {
	if (curMap != "") {
		if (shouldUpdateTime) {
			auto markedTime = std::chrono::steady_clock::now();
			imGuiTimeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(markedTime - startTime).count();
			shouldUpdateTime = false;
			gameWrapper->SetTimeout([this](GameWrapper* gw) {
				shouldUpdateTime = true;
				}, 1.0f);
			SortMapTimes();
		}
		
		ImGui::Text("Current Map: %s", maps[curMap].displayName.c_str());
		ImGui::Text("Current Session Length: %s", FormatDisplayTime(imGuiTimeDiff).c_str());
		ImGui::Separator();
	}
	ImGui::Text("Table Sort Method: ");
	ImGui::SameLine();
	ImGui::RadioButton("Time Played", &tableSortStyle, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Map Name", &tableSortStyle, 1);
	ImGui::Separator();
	ImGui::Checkbox("Reverse Order", &isReversed);
	ImGui::Separator();
	
	ImGui::Text("Time Spent In Maps");

	ImGui::Columns(2, "maptimetable");
	ImGui::Separator();

	ImGui::Text("Map Name");
	ImGui::NextColumn();

	ImGui::Text("Time");
	ImGui::NextColumn();

	ImGui::Separator();
	std::vector<std::string>::iterator iter;
	std::vector<std::string>::iterator stop;

	if (tableSortStyle == 1) {
		iter = isReversed ? mapNamesSorted.end() : mapNamesSorted.begin();
		stop = isReversed ? mapNamesSorted.begin() : mapNamesSorted.end();
	}
	else {
		iter = isReversed ? mapTimesSorted.end() : mapTimesSorted.begin();
		stop = isReversed ? mapTimesSorted.begin() : mapTimesSorted.end();
	}

	if (isReversed) --iter;

	while (iter != stop) {
		std::string& rawName = *iter;
		auto& map = maps[rawName];
		ImGui::AlignTextToFramePadding();
		if (ImGui::Selectable(map.displayName.c_str(), rawName == imGuiSelectedMap, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
			if (ImGui::IsMouseDoubleClicked(0)) {
				imGuiSelectedMap = rawName;
				LoadImGuiMapInBuffer(map.displayName);
				ImGui::OpenPopup("Edit Map Name");
			}
		}
		ImGui::NextColumn();

		ImGui::AlignTextToFramePadding();
		if (curMap == rawName)
			ImGui::Text(FormatDisplayTime(imGuiTimeDiff + map.timePlayed).c_str());
		else
			ImGui::Text(map.GetDisplayTime().c_str());

		ImGui::NextColumn();
		isReversed ? --iter : ++iter;
	}

	ImGui::Columns(1);
	ImGui::Separator();

	if (ImGui::BeginPopupModal("Edit Map Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextUnformatted("Name");
		ImGui::SameLine();
		ImGui::InputTextEx("##name", "", nameBuffer, sizeof(nameBuffer), ImVec2(0, 0), ImGuiTextFlags_None);
		ImGui::SetItemDefaultFocus();

		if (ImGui::Button("Save", ImVec2(120, 0))) { 
			UpdateNameFromBuffer();
			imGuiSelectedMap = "";
			SortMapNames();
			gameWrapper->Execute([this](GameWrapper* gw) {
				SaveMapData();
				});
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { 
			imGuiSelectedMap = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void TimeTrackerPlugin::ClearBuffer() {
	for (int i = 0; i < sizeof(nameBuffer); i++) {
		nameBuffer[i] = 0;
	}
}

void TimeTrackerPlugin::LoadImGuiMapInBuffer(std::string mapName) {
	ClearBuffer();

	for (int i = 0; i < sizeof(nameBuffer) || i < mapName.size(); i++) {
		nameBuffer[i] = mapName[i];
	}
}

void TimeTrackerPlugin::UpdateNameFromBuffer() {
	if (maps.find(imGuiSelectedMap) == maps.end()) return;

	maps[imGuiSelectedMap].displayName = std::string(nameBuffer);
}

std::string TimeTrackerPlugin::FormatDisplayTime(long long time) {
	long long days = time / DAY_MULTIPLIER;
	long long remainingTime = time - (days * DAY_MULTIPLIER);

	long long hours = time / HOUR_MULTIPLIER;
	remainingTime = remainingTime - (hours * HOUR_MULTIPLIER);

	long long minutes = remainingTime / MINUTE_MULTIPLIER;
	remainingTime = remainingTime - (minutes * MINUTE_MULTIPLIER);

	long long seconds = remainingTime / SECOND_MULTIPLIER;

	return std::to_string(days) + " days, " + std::to_string(hours) + " hours, " + std::to_string(minutes) + " minutes, " + std::to_string(seconds) + " seconds";
}
