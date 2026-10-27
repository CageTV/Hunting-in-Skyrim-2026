Hunting in Skyrim 2026
A modern companion for Hunting in Skyrim - A Hunting Guild SE (Nexus 10353) by Richard Webster. Part of the 2026 series with Campfire 2026, Frostfall 2026,
Last Seed 2026, Hunterborn 2026, Simple Hunting Overhaul 2026, Immersive Death Cycle 2026 and Stress and Fear 2026.

THIS IS A START, NOT THE WHOLE UPDATE. Hunting in Skyrim itself stays installed and is a requirement: this adds a small SKSE plugin that does what the mod
makes you do by hand. More of the update will follow in later versions.

WHAT IT DOES (1.0.0)
--------------------
Hunting in Skyrim begins when you cast the spell Path of the Woodsman and answer "Start systems". Only then do the skills, trapping, tracking and the stat journal
exist, and you get three message boxes. This plugin starts it for you:

- Once the opening of a new game is over and you are in control (not in a menu, dialogue, combat, or sitting or sleeping), Hunting in Skyrim starts by itself.
  No spell, no message boxes. It is the same start the spell makes (the stats quest), so everything behaves as if you had answered "Start systems".
- A game where Hunting in Skyrim is already running is left alone. If you would rather start it yourself, turn the automatic start off in the settings.
- A "Hunting in Skyrim" page in SKSE Menu Framework shows whether it is running, has the automatic start switch, and a "Start it now" button.
- When Hunterborn 2026 is installed, the page says what the connection between them does (skinning experience, the double-skin perk, the skinned animal models).

TRAPS
-----
Not changed in this version: place a trap by dropping it from your inventory. Equipping it only shows its statistics. Right after placing a trap, wait a couple of
seconds before using it; the mod ignores clicks while it sets the trap up.

COMPATIBILITY
-------------
- Last Seed 2026 1.3.1 and Gourmet - AutoPatch 1.2.1 leave Hunting in Skyrim's 73 trap baits alone. Older versions treat the baits as food: they spoil, or
  Last Seed asks about each one.
- Hunterborn 2026 skips Hunting in Skyrim's experience until Hunting in Skyrim has been started, which this plugin does for you.

REQUIREMENTS
------------
- Hunting in Skyrim - A Hunting Guild SE 2.0 Beta 1.8 (Nexus 10353)
- SKSE Menu Framework 3, SKSE (Address Library for the build you use)

BUILDS
------
The installer asks which HuntingInSkyrimNative.dll: "new" for Skyrim 1.6.1170 and newer, "older" for Skyrim VR and 1.6.1130 and older.

CREDITS AND LICENSE
-------------------
Hunting in Skyrim and Fishing in Skyrim by Richard Webster, who has given permission to update them. This plugin and its settings pages are by CageTV, MIT
licensed (see LICENSE.txt); they contain none of the original mod's plugin, scripts, meshes, textures or sounds.
