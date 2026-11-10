#include "CBhop.hpp"

#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Bypass.hpp>

#include <AndromedaClient/Settings/Settings.hpp>

static CBhop g_Bhop{};

auto CBhop::OnCreateMove( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void
{
	if ( !Settings::Bhop::Active )
		return;

	if ( !SDK::Interfaces::EngineToClient()->IsInGame() )
		return;

	auto* pLocalPawn = GetCL_Players()->GetLocalPlayerPawn();
	if ( !pLocalPawn )
		return;

	// MoveType 2 = MOVETYPE_WALK, only bhop when on ground (flags & FL_ONGROUND)
	const auto iFlags = pLocalPawn->m_fFlags();
	const bool bOnGround = ( iFlags & 1 ); // FL_ONGROUND = 1

	// space held
	const bool bSpaceHeld = ( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) != 0;

	if ( !bSpaceHeld )
		return;

	if ( bOnGround )
		GetCL_Bypass()->SetJump( pUserCmd );
	else
		GetCL_Bypass()->SetDontJump( pUserCmd );
}

auto GetBhop() -> CBhop*
{
	return &g_Bhop;
}
