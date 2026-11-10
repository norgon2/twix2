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
	// shared
	auto IsValidTarget( CCSPlayerController* pController , C_CSPlayerPawn* pPawn , bool bRequireVisible ) -> bool;
	auto ApplyRCS( QAngle& vAngle , C_CSPlayerPawn* pLocalPawn ) -> void;

	// legit
	auto GetBestLegitTarget() -> C_CSPlayerPawn*;
	auto RunLegit( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void;

	// rage
	auto GetBestRageTarget() -> C_CSPlayerPawn*;
	auto GetBestRageBone( C_CSPlayerPawn* pPawn , const Vector3& vLocalEye ) -> Vector3;
	auto RunRage( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void;
};

auto GetAimbot() -> CAimbot*;
