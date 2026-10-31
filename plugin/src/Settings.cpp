/*
 * Hunting in Skyrim 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Settings.h"

namespace Settings
{
	namespace
	{
		constexpr const char* kFile = "Data/SKSE/Plugins/HuntingInSkyrimNative/settings.ini";
		Values values;
		float  timer = -1.0f;

		std::string Trim(const std::string& a_s)
		{
			const auto b = a_s.find_first_not_of(" \t\r\n");
			const auto e = a_s.find_last_not_of(" \t\r\n");
			return b == std::string::npos ? std::string() : a_s.substr(b, e - b + 1);
		}
	}

	Values& Get() { return values; }

	const char* FilePath() { return kFile; }

	void Load()
	{
		std::ifstream in(kFile);
		if (!in) {
			Save();  // first run: write the defaults so the file exists
			return;
		}
		std::string line;
		while (std::getline(in, line)) {
			if (line.empty() || line[0] == ';' || line[0] == '#' || line[0] == '[') {
				continue;
			}
			const auto eq = line.find('=');
			if (eq == std::string::npos) {
				continue;
			}
			const std::string key = Trim(line.substr(0, eq));
			const std::string val = Trim(line.substr(eq + 1));
			if (key == "auto_start") {
				values.autoStart = val == "1" || val == "true";
			} else if (key == "fishing_off") {
				values.fishingOff = val == "1" || val == "true";
			} else if (key == "skinning_off") {
				values.skinningOff = val == "1" || val == "true";
			}
		}
	}

	void Save()
	{
		std::error_code ec;
		std::filesystem::create_directories(std::filesystem::path(kFile).parent_path(), ec);
		std::ofstream out(kFile, std::ios::trunc);
		if (!out) {
			SKSE::log::warn("Cannot write {}", kFile);
			return;
		}
		out << "; Hunting in Skyrim 2026. Written by the in-game page; safe to edit by hand while the game is closed.\n"
			<< "auto_start=" << (values.autoStart ? 1 : 0) << "\n"
			<< "fishing_off=" << (values.fishingOff ? 1 : 0) << "\n"
			<< "skinning_off=" << (values.skinningOff ? 1 : 0) << "\n";
	}

	void MarkDirty() { timer = 0.6f; }

	void Tick(float a_dt)
	{
		if (timer >= 0.0f) {
			timer -= a_dt;
			if (timer < 0.0f) {
				Save();
			}
		}
	}
}
