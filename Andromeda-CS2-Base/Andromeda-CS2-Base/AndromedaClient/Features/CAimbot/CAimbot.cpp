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

// ─────────────────────────── shared ───────────────────────────

auto CAimbot::IsValidTarget( CCSPlayerController* pController , C_CSPlayerPawn* pPawn , bool bRequireVisible ) -> bool
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

	if ( bRequireVisible && !GetCL_VisibleCheck()->IsPlayerControllerVisible( pController ) )
		return false;

	return true;
}

auto CAimbot::ApplyRCS( QAngle& vAngle , C_CSPlayerPawn* pLocalPawn ) -> void
{
	if ( pLocalPawn->m_iShotsFired() <= 1 )
		return;

	const auto& aimPunchCache = pLocalPawn->m_aimPunchCache();
	if ( aimPunchCache.Count() <= 0 )
		return;

	const auto& punch = aimPunchCache[ aimPunchCache.Count() - 1 ];
	vAngle.m_x -= punch.m_x * 2.0f;
	vAngle.m_y -= punch.m_y * 2.0f;
}

// ─────────────────────────── LEGIT ───────────────────────────

auto CAimbot::GetBestLegitTarget() -> C_CSPlayerPawn*
{
	auto* pLocalPawn = GetCL_Players()->GetLocalPlayerPawn();
	if ( !pLocalPawn )
		return nullptr;

	const auto vLocalEye    = GetCL_Players()->GetLocalEyeOrigin();
	const auto vLocalAngles = pLocalPawn->m_angEyeAngles();

	C_CSPlayerPawn* pBest   = nullptr;
	float           flBestFov = Settings::Aimbot::FOV;

	const auto iHighest = SDK::Interfaces::GameEntitySystem()->GetHighestEntityIndex();
	for ( auto i = 1; i <= iHighest; i++ )
	{
		auto* pCtrl = SDK::Interfaces::GameEntitySystem()->GetBaseEntity<CCSPlayerController>( i );
		if ( !pCtrl )
			continue;

		auto* pPawn = SDK::Interfaces::GameEntitySystem()->GetBaseEntity<C_CSPlayerPawn>( pCtrl->m_hPawn().GetEntryIndex() );
		if ( !IsValidTarget( pCtrl , pPawn , Settings::Aimbot::OnlyVisible ) )
			continue;

		const auto vBone = GetCL_Bones()->GetBonePositionByName( pPawn , Settings::Aimbot::BoneTarget );
		if ( vBone.IsZero() )
			continue;

		const auto flFov = vLocalAngles.Distance( Math::CalcAngle( vLocalEye , vBone ) );
		if ( flFov < flBestFov )
		{
			flBestFov = flFov;
			pBest     = pPawn;
		}
	}

	return pBest;
}

auto CAimbot::RunLegit( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void
{
	if ( !Settings::Aimbot::Active )
		return;

	const auto iKey = Settings::Aimbot::Key;
	if ( iKey != 0 && !( GetAsyncKeyState( iKey ) & 0x8000 ) )
		return;

	auto* pLocalPawn = GetCL_Players()->GetLocalPlayerPawn();
	if ( !pLocalPawn )
		return;

	auto* pTarget = GetBestLegitTarget();
	if ( !pTarget )
		return;

	const auto vLocalEye = GetCL_Players()->GetLocalEyeOrigin();
	const auto vBone     = GetCL_Bones()->GetBonePositionByName( pTarget , Settings::Aimbot::BoneTarget );
	if ( vBone.IsZero() )
		return;

	auto vAimAngle = Math::CalcAngle( vLocalEye , vBone );

	if ( Settings::Aimbot::RCS )
		ApplyRCS( vAimAngle , pLocalPawn );

	Math::ClampAngles( vAimAngle );

	// smooth — plus Smoothing est bas, plus c'est snap
	const auto vCurrentAngles = pLocalPawn->m_angEyeAngles();
	QAngle vOut{};
	Math::SmoothAngles( vCurrentAngles , vAimAngle , vOut , Settings::Aimbot::Smoothing );
	Math::ClampAngles( vOut );

	GetCL_Bypass()->SetViewAngles( &vOut , pInput , pUserCmd );
}

// ─────────────────────────── RAGE ───────────────────────────

// Essaie les bones dans Settings::Aimbot::RageBones, retourne le premier non-zero
auto CAimbot::GetBestRageBone( C_CSPlayerPawn* pPawn , const Vector3& vLocalEye ) -> Vector3
{
	for ( int i = 0; Settings::Aimbot::RageBones[ i ] != nullptr; i++ )
	{
		const auto vBone = GetCL_Bones()->GetBonePositionByName( pPawn , Settings::Aimbot::RageBones[ i ] );
		if ( !vBone.IsZero() )
			return vBone;
	}
	return Vector3{};
}

auto CAimbot::GetBestRageTarget() -> C_CSPlayerPawn*
{
	auto* pLocalPawn = GetCL_Players()->GetLocalPlayerPawn();
	if ( !pLocalPawn )
		return nullptr;

	const auto vLocalEye    = GetCL_Players()->GetLocalEyeOrigin();
	const auto vLocalAngles = pLocalPawn->m_angEyeAngles();

	C_CSPlayerPawn* pBest    = nullptr;
	float           flBestFov = 360.f; // rage = pas de limite FOV

	const auto iHighest = SDK::Interfaces::GameEntitySystem()->GetHighestEntityIndex();
	for ( auto i = 1; i <= iHighest; i++ )
	{
		auto* pCtrl = SDK::Interfaces::GameEntitySystem()->GetBaseEntity<CCSPlayerController>( i );
		if ( !pCtrl )
			continue;

		auto* pPawn = SDK::Interfaces::GameEntitySystem()->GetBaseEntity<C_CSPlayerPawn>( pCtrl->m_hPawn().GetEntryIndex() );
		if ( !IsValidTarget( pCtrl , pPawn , Settings::Aimbot::RageOnlyVisible ) )
			continue;

		const auto vBone = GetBestRageBone( pPawn , vLocalEye );
		if ( vBone.IsZero() )
			continue;

		const auto flFov = vLocalAngles.Distance( Math::CalcAngle( vLocalEye , vBone ) );
		if ( flFov < flBestFov )
		{
			flBestFov = flFov;
			pBest     = pPawn;
		}
	}

	return pBest;
}

auto CAimbot::RunRage( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void
{
	if ( !Settings::Aimbot::RageActive )
		return;

	const auto iKey = Settings::Aimbot::RageKey;
	if ( iKey != 0 && !( GetAsyncKeyState( iKey ) & 0x8000 ) )
		return;

	auto* pLocalPawn = GetCL_Players()->GetLocalPlayerPawn();
	if ( !pLocalPawn )
		return;

	auto* pTarget = GetBestRageTarget();
	if ( !pTarget )
		return;

	const auto vLocalEye = GetCL_Players()->GetLocalEyeOrigin();
	const auto vBone     = GetBestRageBone( pTarget , vLocalEye );
	if ( vBone.IsZero() )
		return;

	auto vAimAngle = Math::CalcAngle( vLocalEye , vBone );

	if ( Settings::Aimbot::RageRCS )
		ApplyRCS( vAimAngle , pLocalPawn );

	Math::ClampAngles( vAimAngle );

	// snap instantané, pas de smooth
	GetCL_Bypass()->SetViewAngles( &vAimAngle , pInput , pUserCmd );

	// autofire : tire dès que l'angle est écrit
	if ( Settings::Aimbot::RageAutoFire )
		GetCL_Bypass()->SetAttack( pUserCmd , true ); // SubTick = true pour rage
}

// ─────────────────────────── entry ───────────────────────────

auto CAimbot::OnCreateMove( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void
{
	if ( !SDK::Interfaces::EngineToClient()->IsInGame() )
		return;

	// rage a priorité sur legit si les deux sont actifs
	if ( Settings::Aimbot::RageActive )
		RunRage( pInput , pUserCmd );
	else
		RunLegit( pInput , pUserCmd );
}

auto GetAimbot() -> CAimbot*
{
	return &g_Aimbot;
}
