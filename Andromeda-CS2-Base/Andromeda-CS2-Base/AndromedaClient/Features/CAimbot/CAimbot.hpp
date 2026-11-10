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
	auto IsValidTarget( CCSPlayerController* pController , C_CSPlayerPawn* pPawn , bool bRequireVisible ) -> bool;
	auto ApplyRCS( QAngle& vAngle , C_CSPlayerPawn* pLocalPawn ) -> void;
	auto GetBestRageBone( C_CSPlayerPawn* pPawn ) -> Vector3;
	auto GetBestLegitTarget() -> C_CSPlayerPawn*;
	auto GetBestRageTarget() -> C_CSPlayerPawn*;
	auto RunLegit( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void;
	auto RunRage( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void;
};

auto GetAimbot() -> CAimbot*;
