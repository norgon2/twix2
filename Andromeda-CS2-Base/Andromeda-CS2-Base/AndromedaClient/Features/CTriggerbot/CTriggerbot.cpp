#include "CTriggerbot.hpp"

#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/Interface/CGameEntitySystem.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Trace.hpp>
#include <GameClient/CL_Bypass.hpp>

#include <AndromedaClient/Settings/Settings.hpp>

#include <chrono>

static CTriggerbot g_Triggerbot{};

static auto GetTimeMs() -> float
{
	using namespace std::chrono;
	return static_cast<float>( duration_cast<milliseconds>( steady_clock::now().time_since_epoch() ).count() );
}

auto CTriggerbot::IsEnemyUnderCrosshair() -> bool
{
	auto* pLocalPawn = GetCL_Players()->GetLocalPlayerPawn();
	if ( !pLocalPawn )
		return false;

	const auto vEye = GetCL_Players()->GetLocalEyeOrigin();
	const auto vAngles = pLocalPawn->m_angEyeAngles();

	Vector3 vForward{};
	Math::AngleVectors( vAngles , vForward );

	const auto vEnd = vEye + vForward * 8192.0f;
	auto* pHit = GetCL_Trace()->TraceToEntityEndPos( &vEnd );
	if ( !pHit )
		return false;

	auto* pHitPawn = reinterpret_cast<C_CSPlayerPawn*>( pHit );

	if ( pHitPawn->m_iHealth() <= 0 )
		return false;

	if ( pHitPawn->m_iTeamNum() == pLocalPawn->m_iTeamNum() )
		return false;

	return true;
}

auto CTriggerbot::OnCreateMove( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void
{
	if ( !Settings::Triggerbot::Active )
		return;

	if ( !SDK::Interfaces::EngineToClient()->IsInGame() )
		return;

	const auto iKey = Settings::Triggerbot::Key;
	if ( iKey != 0 && !( GetAsyncKeyState( iKey ) & 0x8000 ) )
		return;

	const bool bEnemyUnder = IsEnemyUnderCrosshair();
	const float flNow = GetTimeMs();

	if ( bEnemyUnder )
	{
		if ( !m_bShooting )
		{
			m_flNextShot = flNow + static_cast<float>( Settings::Triggerbot::DelayMs );
			m_bShooting = true;
		}

		if ( flNow >= m_flNextShot )
			GetCL_Bypass()->SetAttack( pUserCmd , false );
	}
	else
	{
		m_bShooting = false;
		GetCL_Bypass()->SetDontAttack( pUserCmd , false );
	}
}

auto GetTriggerbot() -> CTriggerbot*
{
	return &g_Triggerbot;
}
