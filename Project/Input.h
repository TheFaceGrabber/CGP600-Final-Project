#pragma once
#include "dinput.h"
#include <xnamath.h>

class Input
{
private:
	static Input* m_pInstance;

	IDirectInput8* m_pDirectInput;
	IDirectInputDevice8* m_pKbdDevice;
	IDirectInputDevice8* m_pMseDevice;

	unsigned char m_KbdKeysState[256];
	DIMOUSESTATE m_MseState;

public:
	static Input* GetInstance();
	static void Release()
	{
		delete m_pInstance;
	}

	void Update();
	bool IsKeyPressed(unsigned char keyCode);
	XMFLOAT2 GetMouseDelta();
	bool IsMouseButtonPressed(short button);

	Input();
	~Input();
};

