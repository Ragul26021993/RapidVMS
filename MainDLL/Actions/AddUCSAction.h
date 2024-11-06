//Class to add the UCS action..//
#pragma once
#include "RAction.h"
#include "..\Engine\UCS.h"

class AddUCSAction:
	public RAction
{
private:
	BaseItem* ucs;		//added UCS
	BaseItem* p_ucs;	//previous UCS
	int NucsId;
public:
	//Constructor and destructor..//
	AddUCSAction();
	virtual ~AddUCSAction();

	void setUCS(UCS* nucs);
	UCS* getUCS();
	UCS* getPUCS();

	//Set and get the UCS id..//
	void setNewId(int id);
	int getNewId();

	//Add/ Remove the UCS to/from the List...
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Add current UCS action...//
	static void addUCS(UCS* nucs);

	//Partprogram read/write/.//
	friend wostream& operator<<(wostream&, AddUCSAction&);
	friend wistream& operator>>(wistream&, AddUCSAction&);
	friend void ReadOldPP(wistream& is, AddUCSAction& action);
};