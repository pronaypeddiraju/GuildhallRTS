//------------------------------------------------------------------------------------------------------------------------------
#include "Game/GameHandle.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"

//------------------------------------------------------------------------------------------------------------------------------
GameHandle::GameHandle(uint cyclicID, uint index)
{
	ASSERT_RECOVERABLE(index <= 0x0000ffff, "");
	ASSERT_RECOVERABLE(cyclicID <= 0x0000ffff, "");

	// So I want cyclicID to be in high-word (high 16 bits)
	// I want index in the lo-word (low 16 bits); 
	uint hiword = cyclicID << 16;
	m_data = hiword | index;
}

//------------------------------------------------------------------------------------------------------------------------------
GameHandle::GameHandle()
{

}

//------------------------------------------------------------------------------------------------------------------------------
GameHandle::~GameHandle()
{

}

//------------------------------------------------------------------------------------------------------------------------------
bool GameHandle::operator!=(GameHandle const &other) const
{
	if (m_data != other.m_data)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool GameHandle::operator==(GameHandle const &other) const
{
	if (m_data == other.m_data)
	{
		return true;
	}
	else
	{
		return false;
	}
}

