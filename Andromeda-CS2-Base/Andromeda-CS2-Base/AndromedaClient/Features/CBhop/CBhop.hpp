#pragma once
#include <Common/Common.hpp>

class CCSGOInput;
class CUserCmd;

class CBhop
{
public:
	auto OnCreateMove( CCSGOInput* pInput , CUserCmd* pUserCmd ) -> void;
};

auto GetBhop() -> CBhop*;
