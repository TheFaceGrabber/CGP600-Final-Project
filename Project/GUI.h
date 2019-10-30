#pragma once
#include "text2D.h"

class GUI
{
private:
	static GUI* m_pInstance;
	Text2D* m_pText;
public:
	static GUI* GetInstance();
	static void Release();

	GUI();
	~GUI();

	void UpdateGfx();
	void DrawGUIText(string text, float xPos, float yPos, float scale);
};

