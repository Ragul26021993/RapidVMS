//Class to draw the text....//
#pragma once
#include "MouseHandler.h"
#include "..\Shapes\RText.h"
#include "..\Shapes\Vector.h"

class RTextHandler :
	public MouseHandler
{
//Variable declaration..//
private:
	Vector location;
	std::string CurrentText;
	RText *RTextShape;
	double Textfsize;
	bool runningPartprogramValid, Validflag;
public:
	RTextHandler();
	~RTextHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);
	
	void SetCurrentText(char* Ctext);
	//Virtual function to handle the partprogram data...//
	virtual void PartProgramData();
};