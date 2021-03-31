#pragma once
#include <cstdint>
#include "Coin.h"

class Miner
{
private:
	uint8_t Reputation;

public:
	Miner() : Reputation(100)
	{}

	void processPacket(Coin c)
	{}
};