/*
 * Hunting in Skyrim 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

// HuntingInSkyrimNative.dll settings, kept in Data/SKSE/Plugins/HuntingInSkyrimNative/settings.ini and edited from the SKSE Menu Framework page.
namespace Settings
{
	struct Values
	{
		bool fishingOff = true;  // switch Hunting in Skyrim's own fishing off (its fishing quest is stopped on every load): Fishing 2026 does the fishing
		bool skinningOff = true;  // switch Hunting in Skyrim's own (realistic) skinning off: Hunterborn / Simple Hunting Overhaul do the skinning
		bool autoStart = true;  // start Hunting in Skyrim's systems by itself once the opening is over, without Path of the Woodsman and its messages
	};

	Values&     Get();
	void        Load();
	void        Save();
	void        MarkDirty();       // the file is written a moment after the last change
	void        Tick(float a_dt);  // call every frame
	const char* FilePath();
}
