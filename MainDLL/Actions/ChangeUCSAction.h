//Class held the change ucs action..//
#pragma once
#include "RAction.h"

class ChangeUCSAction:
	public RAction
{
//variable declaration...//
private:
	int previousid, newid;
public:
	//Constructor and destructor..
	ChangeUCSAction();
	virtual ~ChangeUCSAction();

	//Current UCS id...//
	void setNId(int id);

	//Add/ remove the UCSchange to action list
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	
	//Set and get the New UCS id...//
	int getNID();
	void setPID(int);
	int getPID();

	//Change ucs according to the UCS id..!
	static void changeUCS(int id);

	//Read/write the partprogram details..//
	friend wostream& operator<<(wostream&, ChangeUCSAction&);
	friend wistream& operator>>(wistream&, ChangeUCSAction&);
	friend void ReadOldPP(wistream& is, ChangeUCSAction& action);
};