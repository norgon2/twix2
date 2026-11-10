#include "CAimbot.hpp"

#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/Interface/CGameEntitySystem.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Bones.hpp>
#include <GameClient/CL_VisibleCheck.hpp>
#include <GameClient/CL_Bypass.hpp>

#include <AndromedaClient/Settings/Settings.hpp>

static CAimbot g_Aimbot{};

auto CAimbot::IsValidTarget( CCSPlayerController* pController , C_CSPlayerPawn* pPawn ) -> bool
{
	if ( !pController || !pPawn )
		return false;

	if ( !pController->m_bPawnIsAlive() )
		return false;

	if ( pPawn->m_iHealth() <= 0 )
		return false;

	auto* pLocalPawn = GetCL_Players()->GetLocalPlayerPawn();
	if ( !pLocalPawn )
		return false;

	if ( pPawn->m_iTeamNum() == pLocalPawn->m_iTeamNum() )
		return false;

	if ( Settings::Aimbot::OnlyVisible && !GetCL_VisibleCheck()->IsPlayerControllerVisible( pController ) )
		return false;

	return true;
}

auto CAimbot::GetBestTarget() -> C_CSPlayerPawn*
{
	auto* pLocalPawn = GetCL_Players()->GetLocalPlayerPawn();
	if ( !pLocalPawn )
		return nullptr;

	const auto vLocalEye = GetCL_Players()->GetLocalEyeOrigin();
	const auto vLocalAngles = pLocalPawn->m_angEyeAngles();

	C_CSPlayerPawn* pBestTarget = nullptr;
	float flBestFov = Settings::Aimbot::FOV;

	const auto iHighest = SDK::Interfaces::GameEntitySystem()->GetHighestEntityIndex();
	for ( auto i = 1; i <= iHighest; i++ )
	{
		auto* pController = SDK::Interfaces::GameEntitySystem()->GetBaseEntity<CCSPlayerController>( i );
		if ( !pController )
			continue;

		auto* pPawn = SDK::Interfaces::GameEntitySystem()->GetBaseEntity<C_CSPlayerPawn>( pController->m_hPawn().GetEntryIndex() );
		if ( !IsValidTarget( pController , pPawn ) )
			continue;

		const auto vBone = GetCL_Bones()->GetBonePositionByName( pPawn , Settings::Aimbot::BoneTarget );
		if ( vBone.IsZero() )
			continue;

		const auto vAimAngle = Math::CalcAngle( vLocalEye , vBone );
		const auto flFov = vLocalAngles.Distance( vAimAngle );

		if ( flFov < flBestFov )
		{
			flBestFov = flFov;
			pBestTarget = pPawn;
		}
	}

	return pBestTarget;
}

auto CAimbot::OnCreateMove( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void
{
	if ( !Settings::Aimbot::Active )
		return;

	if ( !SDK::Interfaces::EngineToClient()->IsInGame() )
		return;

	auto* pLocalPawn = GetCL_Players()->GetLocalPlayerPawn();
	if ( !pLocalPawn )
		return;

	const auto iKey = Settings::Aimbot::Key;
	if ( iKey != 0 && !( GetAsyncKeyState( iKey ) & 0x8000 ) )
		return;

	auto* pTarget = GetBestTarget();
	if ( !pTarget )
		return;

	const auto vLocalEye = GetCL_Players()->GetLocalEyeOrigin();
	const auto vBone = GetCL_Bones()->GetBonePositionByName( pTarget , Settings::Aimbot::BoneTarget );
	if ( vBone.IsZero() )
		return;

	auto vAimAngle = Math::CalcAngle( vLocalEye , vBone );

	// recoil compensation
	if ( Settings::Aimbot::RCS && pLocalPawn->m_iShotsFired() > 1 )
	{
		const auto& aimPunchCache = pLocalPawn->m_aimPunchCache();
		if ( aimPunchCache.Count() > 0 )
		{
			const auto& punch = aimPunchCache[ aimPunchCache.Count() - 1 ];
			vAimAngle.m_x -= punch.m_x * 2.0f;
			vAimAngle.m_y -= punch.m_y * 2.0f;
		}
	}

	Math::ClampAngles( vAimAngle );

	const auto vCurrentAngles = pLocalPawn->m_angEyeAngles();
	QAngle vSmoothAngles{};
	Math::SmoothAngles( vCurrentAngles , vAimAngle , vSmoothAngles , Settings::Aimbot::Smoothing );
	Math::ClampAngles( vSmoothAngles );

	GetCL_Bypass()->SetViewAngles( &vSmoothAngles , pInput , pUserCmd );
}

auto GetAimbot() -> CAimbot*
{
	return &g_Aimbot;
}
