#pragma once

#include <Common/Common.hpp>

class CCSGOInput;
class CUserCmd;

class CTriggerbot
{
public:
	auto OnCreateMove( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void;

private:
	auto IsEnemyUnderCrosshair() -> bool;

private:
	bool  m_bShooting = false;
	float m_flNextShot = 0.f;
};

auto GetTriggerbot() -> CTriggerbot*;
