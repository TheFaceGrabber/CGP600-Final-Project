#include "GUI.h"
#include "Direct3D.h"

GUI* GUI::m_pInstance = nullptr;

GUI* GUI::GetInstance()
{
	if (m_pInstance == nullptr)
		m_pInstance = new GUI();

	return m_pInstance;
}

GUI::GUI()
{
	m_pText = new Text2D("Assets/Textures/font1.tif", Direct3D::GetInstance()->GetDevice(), Direct3D::GetInstance()->GetContext());
}

GUI::~GUI()
{
	delete m_pText;
}

void GUI::UpdateGfx()
{
	m_pText->RenderText();
}

void GUI::DrawGUIText(string text, float xPos, float yPos, float scale)
{
	m_pText->AddText(text, xPos, yPos, scale);
}

void GUI::Release()
{
	if(m_pInstance)
		delete m_pInstance;
}
