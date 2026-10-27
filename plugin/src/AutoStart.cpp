/*
 * Hunting in Skyrim 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "AutoStart.h"
#include "Settings.h"

#include "SKSEMenuFramework.h"

namespace AutoStart
{
	namespace
	{
		constexpr const char* kPlugin = "Hunting in Skyrim.esp";
		constexpr unsigned    kStatsQuest = 0x0356BD;  // HISStatsQuest1: its OnQuestStart sets the skills up and starts the Ranger quest and the rest

		float hold = 12.0f;  // seconds to wait after a load
		float timer = 4.0f;
		bool  tried = false;

		RE::TESQuest* StatsQuest()
		{
			auto* dh = RE::TESDataHandler::GetSingleton();
			return dh ? dh->LookupForm<RE::TESQuest>(kStatsQuest, kPlugin) : nullptr;
		}

		// Papyrus Quest.Start(), called the way the start-systems fragment calls it.
		void CallStart(RE::TESQuest* a_quest)
		{
			auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
			if (!vm) {
				return;
			}
			const auto policy = vm->GetObjectHandlePolicy();
			if (!policy) {
				return;
			}
			const auto handle = policy->GetHandleForObject(static_cast<RE::VMTypeID>(a_quest->GetFormType()), a_quest);
			RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> none;
			vm->DispatchMethodCall2(handle, "Quest", "Start", RE::MakeFunctionArguments(), none);
		}

		// The player is in the world and in control: not in a menu, loading, a cutscene, dialogue, or sitting on the opening cart.
		bool PlayerFree()
		{
			auto* player = RE::PlayerCharacter::GetSingleton();
			auto* ui = RE::UI::GetSingleton();
			if (!player || !player->Is3DLoaded() || !ui || ui->GameIsPaused() || !ui->IsShowingMenus()) {
				return false;
			}
			if (ui->IsMenuOpen(RE::MainMenu::MENU_NAME) || ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME) || ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME) ||
				ui->IsMenuOpen(RE::CursorMenu::MENU_NAME) || SKSEMenuFramework::IsAnyBlockingWindowOpened()) {
				return false;
			}
			return player->AsActorState()->GetSitSleepState() == RE::SIT_SLEEP_STATE::kNormal && !player->IsInCombat();
		}
	}

	bool Installed()
	{
		auto* dh = RE::TESDataHandler::GetSingleton();
		return dh && dh->LookupModByName(kPlugin);
	}

	bool Started()
	{
		auto* quest = StatsQuest();
		return quest && quest->IsRunning();
	}

	bool StartNow()
	{
		auto* quest = StatsQuest();
		if (!quest) {
			return false;
		}
		if (quest->IsRunning()) {
			return true;
		}
		SKSE::GetTaskInterface()->AddTask([quest]() {
			CallStart(quest);
			SKSE::log::info("Started the Hunting in Skyrim stats quest ({:08X})", quest->GetFormID());
		});
		return true;
	}

	void OnGameLoaded()
	{
		hold = 12.0f;
		timer = 4.0f;
		tried = false;
	}

	void Tick(float a_dt)
	{
		hold -= a_dt;
		timer -= a_dt;
		if (hold > 0.0f || timer > 0.0f || tried) {
			return;
		}
		timer = 2.0f;
		if (!Settings::Get().autoStart || !Installed()) {
			return;
		}
		if (Started()) {
			tried = true;  // already running (an old save, or started by hand): nothing to do
			return;
		}
		if (!PlayerFree()) {
			return;
		}
		SKSE::log::info("Auto start: Hunting in Skyrim has not been started in this game, starting it");
		tried = StartNow();
	}
}
