/*
 * Hunting in Skyrim 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "AutoStart.h"
#include "Menu.h"
#include "Settings.h"

#include "SKSEMenuFramework.h"

namespace Menu
{
	namespace
	{
		using namespace ImGuiMCP;

		void Tip(const char* a_text)
		{
			if (a_text && a_text[0] && IsItemHovered()) {
				BeginTooltip();
				PushTextWrapPos(420.0f);
				TextUnformatted(a_text);
				PopTextWrapPos();
				EndTooltip();
			}
		}

		void __stdcall RenderOverview()
		{
			if (!AutoStart::Installed()) {
				TextColored(ImVec4(1.0f, 0.45f, 0.4f, 1.0f), "Hunting in Skyrim.esp is not in the load order.");
				return;
			}
			AutoStart::Refresh();
			const bool started = AutoStart::Started();
			TextColored(started ? ImVec4(0.65f, 0.9f, 0.5f, 1.0f) : ImVec4(0.95f, 0.8f, 0.4f, 1.0f),
				started ? "Hunting in Skyrim is running." : "Hunting in Skyrim has not been started in this game.");
			Spacing();
			TextWrapped("Hunting in Skyrim begins when you cast Path of the Woodsman and answer \"Start systems\". These pages start it for you, and are the home of its settings.");
			Spacing();
			SeparatorText("Start");
			auto& s = Settings::Get();
			if (Checkbox("Start Hunting in Skyrim automatically", &s.autoStart)) {
				Settings::MarkDirty();
			}
			Tip("Once the opening is over and you are in control, the systems start by themselves: skills, trapping, tracking, the stat journal. No spell to cast, "
				"no message boxes. A game that has already started it is left alone.");
			if (!started) {
				if (Button("Start it now")) {
					AutoStart::StartNow();
				}
				SameLine();
				TextDisabled("Takes effect when you close this menu.");
			}
			auto* dh = RE::TESDataHandler::GetSingleton();
			if (dh && dh->LookupModByName("Hunterborn.esp")) {
				Spacing();
				SeparatorText("Hunterborn");
				TextColored(ImVec4(0.65f, 0.9f, 0.5f, 1.0f), "Hunterborn is installed.");
				TextWrapped("Hunterborn does the skinning. With its Hunting in Skyrim compatibility on (Hunterborn > Compatibility), every animal it skins also earns "
							"Hunting in Skyrim skinning experience and the double-skin perk, and the carcass shows Hunting in Skyrim's skinned model. Hunting in Skyrim's own "
							"realistic skinning is switched off for you so the two do not both act on the same animal.");
			}
			Spacing();
			TextDisabled("Hunting in Skyrim by Richard Webster. Settings pages by CageTV.");
		}

		// Drawn every frame whether or not the menu is open.
		void __stdcall Frame()
		{
			const float dt = std::clamp(GetIO()->DeltaTime, 0.0f, 0.1f);
			Settings::Tick(dt);
			AutoStart::Tick(dt);
		}
	}

	void Register()
	{
		if (!SKSEMenuFramework::IsInstalled()) {
			SKSE::log::warn("SKSE Menu Framework is not installed: the Hunting in Skyrim pages are not available, and the automatic start does not run");
			return;
		}
		SKSEMenuFramework::SetSection("Hunting in Skyrim");
		SKSEMenuFramework::AddSectionItem("Overview", RenderOverview);
		SKSEMenuFramework::AddHudElement(Frame);
		SKSE::log::info("Registered with SKSE Menu Framework {}", SKSEMenuFramework::GetMenuFrameworkVersion());
	}
}
