/*
 * Hunting in Skyrim 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

// Hunting in Skyrim begins when the player casts Path of the Woodsman and answers "Start systems": that starts the HiS stats quest, which sets the four
// skills up and starts the other quests, then shows three message boxes. This does the same start on its own, with no spell and no messages.
namespace AutoStart
{
	bool Installed();     // Hunting in Skyrim.esp is in the load order
	bool Started();       // the stats quest is running
	bool StartNow();      // start it (the same Quest.Start call the spell's fragment makes); false when it cannot be done now
	void OnGameLoaded();  // a save was loaded or a new game started: wait a little before anything runs
	void Tick(float a_dt);  // every frame
}
