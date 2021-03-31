#pragma once
#include <cstdint>

class Coin
{
private:
	const int value;
	const int decryptionKey;

public:
	Coin(int hashedValue, int _decryptionKey) : value(hashedValue), decryptionKey(_decryptionKey)
	{}

	int getValue()
	{
		return (value + decryptionKey); // decrypt Value
	}

	static int hashVal(int value)
	{
		return value; // return hashed value
	}
};