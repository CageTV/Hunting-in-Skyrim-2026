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
		bool  fishingDone = false;
		float skinClock = 0.0f;
		constexpr unsigned kRealisticSkinning = 0x0FD2C1;  // HISRealisticSkinningEnabled
		constexpr unsigned kSkinningAnimation = 0x100E36;  // HISSkinningAnimationEnabled

		// Keeps Hunting in Skyrim's own skinning (the realistic skinning on a corpse, with its time, fade and animation) switched off. Its scripts set the
		// global back when a menu option is changed, so this looks again every few seconds.
		void KeepSkinningOff()
		{
			auto* dh = RE::TESDataHandler::GetSingleton();
			if (!dh) {
				return;
			}
			for (const unsigned id : { kRealisticSkinning, kSkinningAnimation }) {
				if (auto* g = dh->LookupForm<RE::TESGlobal>(id, "Hunting in Skyrim.esp"); g && g->value != 0.0f) {
					g->value = 0.0f;
					SKSE::log::info("Hunting in Skyrim's own skinning switched off ({:06X})", id);
				}
			}
		}
		constexpr unsigned kFishingQuest = 0x45D27A;  // HISFishingQuest: its own rods, floats and catches

		// Whether the stats quest is running, as Papyrus's Quest.IsRunning() reports it: -1 not asked yet, 0 no, 1 yes. The quest object's own C++
		// IsRunning() says yes for this quest before it was ever started (seen in game: the menu page claimed it was running), so the question is put to the
		// VM, which answers a moment later.
		std::atomic<int> runState{ -1 };

		class RunningFunctor : public RE::BSScript::IStackCallbackFunctor
		{
		public:
			void operator()(RE::BSScript::Variable a_result) override { runState = a_result.IsBool() ? (a_result.GetBool() ? 1 : 0) : -1; }
			void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&) override {}
		};

		RE::TESQuest* StatsQuest()
		{
			auto* dh = RE::TESDataHandler::GetSingleton();
			return dh ? dh->LookupForm<RE::TESQuest>(kStatsQuest, kPlugin) : nullptr;
		}

		void AskRunning(RE::TESQuest* a_quest)
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
			RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback(new RunningFunctor);
			vm->DispatchMethodCall2(handle, "Quest", "IsRunning", RE::MakeFunctionArguments(), callback);
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

		// Stops HISFishingQuest (Quest.Stop; its fragment shuts the fishing scripts down). Fishing 2026 is the one fishing system; a quest that is not
		// running ignores the call.
		void StopFishing()
		{
			auto* dh = RE::TESDataHandler::GetSingleton();
			auto* quest = dh ? dh->LookupForm<RE::TESQuest>(kFishingQuest, kPlugin) : nullptr;
			if (!quest) {
				return;
			}
			SKSE::GetTaskInterface()->AddTask([quest]() {
				auto*      vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
				const auto policy = vm ? vm->GetObjectHandlePolicy() : nullptr;
				if (!policy) {
					return;
				}
				const auto handle = policy->GetHandleForObject(static_cast<RE::VMTypeID>(quest->GetFormType()), quest);
				RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> none;
				vm->DispatchMethodCall2(handle, "Quest", "Stop", RE::MakeFunctionArguments(), none);
				SKSE::log::info("Stopped Hunting in Skyrim's fishing quest (Fishing 2026 does the fishing)");
			});
		}

		// The player is in the world and in control: not in a menu, loading, a cutscene, dialogue, or sitting on the opening cart.
		// a_why gets the first thing that says no.
		bool PlayerFree(const char*& a_why)
		{
			auto* player = RE::PlayerCharacter::GetSingleton();
			auto* ui = RE::UI::GetSingleton();
			if (!player || !player->Is3DLoaded()) {
				a_why = "the player is not loaded";
				return false;
			}
			if (!ui || ui->GameIsPaused()) {
				a_why = "the game is paused";
				return false;
			}
			if (ui->IsMenuOpen(RE::MainMenu::MENU_NAME) || ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME)) {
				a_why = "a loading or main menu is open";
				return false;
			}
			if (ui->IsMenuOpen(RE::DialogueMenu::MENU_NAME)) {
				a_why = "dialogue";
				return false;
			}
			if (SKSEMenuFramework::IsAnyBlockingWindowOpened()) {
				a_why = "the SKSE Menu Framework window is open";
				return false;
			}
			if (player->AsActorState()->GetSitSleepState() != RE::SIT_SLEEP_STATE::kNormal) {
				a_why = "the player is sitting or sleeping";
				return false;
			}
			if (player->IsInCombat()) {
				a_why = "the player is in combat";
				return false;
			}
			return true;
		}
	}

	bool Installed()
	{
		auto* dh = RE::TESDataHandler::GetSingleton();
		return dh && dh->LookupModByName(kPlugin);
	}

	bool Started() { return runState == 1; }

	bool StartNow()
	{
		auto* quest = StatsQuest();
		if (!quest) {
			return false;
		}
		SKSE::GetTaskInterface()->AddTask([quest]() {
			CallStart(quest);  // a no-op when the quest is already running
			AskRunning(quest);
			SKSE::log::info("Started the Hunting in Skyrim stats quest ({:08X})", quest->GetFormID());
		});
		return true;
	}

	void Refresh()
	{
		static auto last = std::chrono::steady_clock::now() - std::chrono::seconds(10);
		const auto  now = std::chrono::steady_clock::now();
		if (now - last < std::chrono::seconds(5)) {
			return;
		}
		last = now;
		if (auto* quest = StatsQuest()) {
			SKSE::GetTaskInterface()->AddTask([quest]() { AskRunning(quest); });
		}
	}

	void OnGameLoaded()
	{
		fishingDone = false;
		hold = 12.0f;
		timer = 4.0f;
		tried = false;
		runState = -1;
	}

	void Tick(float a_dt)
	{
		hold -= a_dt;
		timer -= a_dt;
		if (hold <= 0.0f && !fishingDone && Settings::Get().fishingOff && Installed()) {
			fishingDone = true;
			StopFishing();
		}
		skinClock += a_dt;
		if (hold <= 0.0f && skinClock >= 5.0f && Settings::Get().skinningOff && Installed()) {
			skinClock = 0.0f;
			KeepSkinningOff();
		}
		if (hold > 0.0f || timer > 0.0f || tried) {
			return;
		}
		timer = 2.0f;
		static float lastLog = -100.0f;
		static float clock = 0.0f;
		clock += 2.0f;
		auto say = [&](const char* a_why) {
			if (clock - lastLog >= 20.0f) {
				lastLog = clock;
				SKSE::log::info("Auto start is waiting: {}", a_why);
			}
		};
		if (!Settings::Get().autoStart) {
			return;
		}
		if (!Installed()) {
			say("Hunting in Skyrim.esp is not in the load order");
			return;
		}
		if (!StatsQuest()) {
			say("the stats quest (0356BD) was not found in Hunting in Skyrim.esp");
			return;
		}
		if (auto* quest = StatsQuest(); quest && runState == -1) {
			SKSE::GetTaskInterface()->AddTask([quest]() { AskRunning(quest); });
			say("asking the game whether the stats quest is running");
			return;
		}
		if (Started()) {
			SKSE::log::info("Auto start: Hunting in Skyrim is already running, nothing to do");
			tried = true;  // already running (an old save, or started by hand): nothing to do
			return;
		}
		const char* why = "";
		if (!PlayerFree(why)) {
			say(why);
			return;
		}
		SKSE::log::info("Auto start: Hunting in Skyrim has not been started in this game, starting it");
		tried = StartNow();
	}
}
