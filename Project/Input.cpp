#include "Input.h"
#include "Direct3D.h"
#include <xnamath.h>

Input* Input::m_pInstance = nullptr;

Input* Input::GetInstance()
{
	if (m_pInstance == nullptr)
		m_pInstance = new Input();

	return m_pInstance;
}

void Input::Update()
{
	HRESULT hr;

	hr = m_pMseDevice->GetDeviceState(sizeof(m_MseState), (LPVOID)&m_MseState);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			m_pMseDevice->Acquire();
		}
	}

	hr = m_pKbdDevice->GetDeviceState(sizeof(m_KbdKeysState), (LPVOID)&m_KbdKeysState);
	if(FAILED(hr))
	{
		if(hr == DIERR_INPUTLOST || hr== DIERR_NOTACQUIRED)
		{
			m_pKbdDevice->Acquire();
		}
	}


}

bool Input::IsKeyPressed(unsigned char keyCode)
{
	return m_KbdKeysState[keyCode] & 0x80;
}

XMFLOAT2 Input::GetMouseDelta()
{
	return XMFLOAT2{ (float)m_MseState.lX/100, (float)m_MseState.lY/100 };
}

bool Input::IsMouseButtonPressed(short button)
{
	if (button > 2)
		return false;

	return m_MseState.rgbButtons[button];
}

Input::Input()
{
	ZeroMemory(m_KbdKeysState, sizeof(m_KbdKeysState));
	DirectInput8Create(Direct3D::GetInstance()->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);

	m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKbdDevice, NULL);
	m_pKbdDevice->SetDataFormat(&c_dfDIKeyboard);
	m_pKbdDevice->SetCooperativeLevel(Direct3D::GetInstance()->GetHWindow(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pKbdDevice->Acquire();

	m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMseDevice, NULL);
	m_pMseDevice->SetDataFormat(&c_dfDIMouse);
	m_pMseDevice->SetCooperativeLevel(Direct3D::GetInstance()->GetHWindow(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pMseDevice->Acquire();
}

Input::~Input()
{
	if(m_pKbdDevice)
	{
		m_pKbdDevice->Unacquire();
		m_pKbdDevice->Release();
	}

	m_pDirectInput->Release();
}
