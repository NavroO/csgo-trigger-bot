#include "memory.h"

#include <thread>
#include <iostream>

namespace offset
{
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDEA964;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DFFEF4;
	constexpr ::std::ptrdiff_t dwForceAttack = 0x322DCFC;
	constexpr ::std::ptrdiff_t m_iHealth = 0x100;
	constexpr ::std::ptrdiff_t m_iCrosshairId = 0x11838;
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;

	constexpr ::std::ptrdiff_t m_fFlags = 0x104;
	constexpr ::std::ptrdiff_t dwForceJump = 0x52BBC7C;
}


int main() 
{
	const auto memory = Memory("csgo.exe");
	const auto client = memory.GetModuleAddress("client.dll");

	std::cout << std::hex << "client.dll -> 0x" << client << std::dec << std::endl;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		const auto& localPlayer = memory.Read < std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& localHealth = memory.Read < std::int32_t>(localPlayer + offset::m_iHealth);

		if (localPlayer)
		{
			const auto onGround = memory.Read<bool>(localPlayer + offset::m_fFlags);

			if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0))
				memory.Write<BYTE>(client + offset::dwForceJump, 6);
		}


		// Trigger bot
		if (!GetAsyncKeyState(VK_SHIFT))
			continue;

		if (!localHealth)
			continue;

		const auto& crosshairId = memory.Read<std::int32_t>(localPlayer + offset::m_iCrosshairId);

		if (!crosshairId || crosshairId > 64)
			continue;

		const auto& player = memory.Read<std::uintptr_t>(client + offset::dwEntityList + (crosshairId - 1) * 0x10);

		if (!memory.Read<std::int32_t>(player + offset::m_iHealth))
			continue;

		if (memory.Read<std::int32_t>(player + offset::m_iTeamNum) ==
			memory.Read<std::int32_t>(localPlayer + offset::m_iTeamNum))
			continue;

		memory.Write<std::uintptr_t>(client + offset::dwForceAttack, 6);
		std::this_thread::sleep_for(std::chrono::microseconds(20));
		memory.Write<std::uintptr_t>(client + offset::dwForceAttack, 4);
		// End trigger bot code
	}

	return 0;
}