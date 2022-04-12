#include "pch.h"
#include "TimeTrackerPlugin.h"
#include "imgui/imgui_internal.h"
#include "IMGUI/imgui_stdlib.h"
#include "DayConvert.h"

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
		ImGui::Text("Current Session Length: %s", GetTimeString(imGuiTimeDiff).c_str());
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
				newMapName = maps[rawName].displayName;
				ImGui::OpenPopup("Edit Map Name");
			}
		}
		ImGui::NextColumn();

		ImGui::AlignTextToFramePadding();
		if (curMap == rawName)
			ImGui::Text(GetTimeString(imGuiTimeDiff + map.timePlayed).c_str());
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
		ImGui::InputText("##name", &newMapName);
		ImGui::SetItemDefaultFocus();

		if (ImGui::Button("Save", ImVec2(120, 0))) { 
			UpdateName();
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

void TimeTrackerPlugin::UpdateName() {
	if (maps.find(imGuiSelectedMap) == maps.end()) return;
	maps[imGuiSelectedMap].displayName = newMapName;
}