#pragma once

#include <Common/Common.hpp>

class CCSGOInput;
class CUserCmd;
class C_CSPlayerPawn;
class CCSPlayerController;

class CAimbot
{
public:
	auto OnCreateMove( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void;

private:
	auto GetBestTarget() -> C_CSPlayerPawn*;
	auto IsValidTarget( CCSPlayerController* pController , C_CSPlayerPawn* pPawn ) -> bool;
};

auto GetAimbot() -> CAimbot*;
