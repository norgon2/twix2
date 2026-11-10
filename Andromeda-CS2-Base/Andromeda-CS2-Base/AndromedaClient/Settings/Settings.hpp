#pragma once

#include <Common/Common.hpp>
#include <ImGui/imgui.h>

namespace Settings
{
	namespace Visual
	{
		inline auto Active = true;
		inline auto Team = true;
		inline auto Enemy = true;
		inline auto OnlyVisible = false;
		inline auto PlayerBox = true;
		inline auto BoneESP = true;
		inline auto BoneESPTeam = true;
		inline auto BoneESPEnemy = true;
		inline auto Glow = true;
		inline auto GlowTeam = true;
		inline auto GlowEnemy = true;

		inline auto PlayerBoxType = 3;
	}
	namespace Menu
	{
		inline auto MenuStyle = 0;
		inline auto MenuDPI = 1.f;
	}
	namespace Aimbot
	{
		// --- Legit ---
		inline auto Active      = false;
		inline auto OnlyVisible = true;
		inline auto FOV         = 5.f;      // degrees, legit mode
		inline auto Smoothing   = 8.f;      // lower = snappier (1 = instant, 20 = slow)
		inline auto RCS         = true;
		inline auto Key         = VK_LBUTTON; // 0 = always on
		inline const char* BoneTarget = "head_0";

		// --- Ragebot ---
		inline auto RageActive      = false;
		inline auto RageKey         = VK_LBUTTON; // hold to rage
		inline auto RageAutoFire    = true;        // fire automatically on lock
		inline auto RageOnlyVisible = false;       // through smokes etc.
		inline auto RageRCS         = true;
		// bones essayés dans l'ordre jusqu'au premier non-zero
		inline const char* RageBones[] = { "head_0", "spine_1", "pelvis", nullptr };
	}
	namespace Triggerbot
	{
		inline auto Active  = false;
		inline auto Key     = 0; // 0 = always on when active
		inline auto DelayMs = 50; // ms before first shot
	}
	namespace Bhop
	{
		inline auto Active = false;
	}
	namespace Colors
	{
		namespace Visual
		{
			inline ImVec4 PlayerBoxTT = { 255.f / 255.f , 75.f / 255.f , 75.f / 255.f  , 1.f };
			inline ImVec4 PlayerBoxTT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 PlayerBoxCT = { 65.f / 255.f , 200 / 255.f , 255.f / 255.f , 1.f };
			inline ImVec4 PlayerBoxCT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 BoneESPTT = { 255.f / 255.f , 75.f / 255.f , 75.f / 255.f  , 1.f };
			inline ImVec4 BoneESPTT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 BoneESPCT = { 65.f / 255.f , 200 / 255.f , 255.f / 255.f , 1.f };
			inline ImVec4 BoneESPCT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 GlowTT = { 255.f / 255.f , 75.f / 255.f , 75.f / 255.f  , 1.f };
			inline ImVec4 GlowTT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 GlowCT = { 65.f / 255.f , 200 / 255.f , 255.f / 255.f , 1.f };
			inline ImVec4 GlowCT_Visible = { 0 , 1.f , 0.f , 1.f };
		}
	}
}
