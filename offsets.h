#pragma once
#include <cstdint>

//offsets
namespace Offsets 
{
	static constexpr int BASE_ADDRESS = 0x00400000;
	static constexpr int OF_BASE_ADDRESS = 0x0010F4F4;
	static constexpr int PLAYER_BASE = BASE_ADDRESS + OF_BASE_ADDRESS;

	static constexpr int entity_list = PLAYER_BASE + 0x4;
	static constexpr int players_in_map = PLAYER_BASE + 0xC;

	static constexpr int NAME_OFFSET = 0x255;
	static constexpr int POSx_POS = 0x4;
	static constexpr int POSy_POS = 0x8;
	static constexpr int POSz_POS = 0xC;
	
	static constexpr int ENTITY_SIZE = 0x4;
	static constexpr int POSITION_OFFSET = 0x34;
	static constexpr int HEALTH_OFFSET = 0xF8;
	static constexpr int POS_LEFT_RIGHT_ANG = 0x40;
	static constexpr int POSX_UP_DOWN_ANG = 0x44;
	static constexpr int TEAM_OFFSET = 0x32C;
	static constexpr int VIEWMATRIX = 0x501AE8;

	static constexpr int POSx_NORMAL_POS = 0x34;
	static constexpr int POSy_NORMAL_POS = 0x38;
	static constexpr int POSz_NORMAL_POS = 0x3C;
	
}




