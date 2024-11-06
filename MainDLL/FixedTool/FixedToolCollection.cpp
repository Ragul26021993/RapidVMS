#include "StdAfx.h"
#include "FixedToolCollection.h"
#include "FixedLine.h"
#include "FixedCircle.h"
#include "FixedText.h"
#include "..\Engine\RCadApp.h"

//Default Constructor..
FixedToolCollection::FixedToolCollection()
{
	this->_mutex = CreateMutex( NULL, false, NULL);
	this->CurrentFixedShape = NULL;
	this->IsValid = true;
} 

//Destructor....//
FixedToolCollection::~FixedToolCollection()
{
           
}

//Add Fixed shape According to type..
void FixedToolCollection::AddFixedShapes(int _fixedMode, int Sid, bool dontClear)
{
	try
	{
		if(_fixedMode == 0)
		{
			CurrentFixedShape = new FixedLine();
			CurrentFixedShape->DontClear(dontClear);
		}
		else if(_fixedMode == 1)
		{
			CurrentFixedShape = new FixedCircle();
			CurrentFixedShape->DontClear(dontClear);
		}
		else if(_fixedMode == 2)
			CurrentFixedShape = new FixedText();
		CurrentFixedShape->setId(Sid);
		FixedShapeCollection[Sid] = CurrentFixedShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0001", __FILE__, __FUNCSIG__); }
}

//Set the fixed shape parameters...//
void FixedToolCollection::setToolParameter(double _first,double _second, double _third, void* anyThing)
{
	try
	{
		if(CurrentFixedShape == NULL) return;
		this->CurrentFixedShape->setToolParameter(_first, _second, _third, anyThing);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0002", __FILE__, __FUNCSIG__); }
}

//Set the fixed shape Parameters..
void FixedToolCollection::setFixedToolParameter(double x, double y, double z, void *AnyThing)
{
	try
	{
		//convert the points
		MAINDllOBJECT->getWindow(0).mouseMove(int(x), int(y));
		double cx = MAINDllOBJECT->getWindow(0).getPointer().x - MAINDllOBJECT->getWindow(0).getCenter().x;
		double cy = MAINDllOBJECT->getWindow(0).getPointer().y - MAINDllOBJECT->getWindow(0).getCenter().y;
		if(CurrentFixedShape != NULL) CurrentFixedShape->setPosition(x, y);
		this->setToolParameter(cx, cy, z, AnyThing);	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0003", __FILE__, __FUNCSIG__); }	
}

//Cahnge Font size..
void FixedToolCollection::ChangeFixedTextSize(bool increase)
{
	try
	{
		if(CurrentFixedShape == NULL) return;
		CurrentFixedShape->setShapeWidth(increase);
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0004", __FILE__, __FUNCSIG__); }
}

//Toggle hatched property of the selected fixed shape
void FixedToolCollection::ToggleFixedShpHatched()
{
	try
	{
		if(CurrentFixedShape == NULL) return;
		CurrentFixedShape->HatcedShp.Toggle();
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0004a", __FILE__, __FUNCSIG__); }
}

//Set the shape color...//
void FixedToolCollection::setFixedToolColor(double r, double g, double b)
{
	if(CurrentFixedShape == NULL) return;
	this->CurrentFixedShape->setColor(r, g, b);
}

//Set the camera(video) center...
void FixedToolCollection::SetWindowCenter(double x, double y)
{
	try
	{
		for(FG_ITER Fitem = FixedShapeCollection.begin(); Fitem != FixedShapeCollection.end(); Fitem++)
			(*Fitem).second->setCamCenter(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0005", __FILE__, __FUNCSIG__); }
}

//Select a shape according to the Id from the front end..
void FixedToolCollection::selectFixedTool(int id)
{
	try{ this->CurrentFixedShape = FixedShapeCollection[id]; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0006", __FILE__, __FUNCSIG__); }
}

//Draw all fixed shape......
void FixedToolCollection::drawAll()
{
	try
	{
		if(!IsValid) return;
		WaitForSingleObject(_mutex, INFINITE);
		for(FG_ITER Fitem = FixedShapeCollection.begin(); Fitem != FixedShapeCollection.end(); Fitem++)
			(*Fitem).second->drawFixedGraphics(4);
		ReleaseMutex(_mutex);
	}
	catch(...){ ReleaseMutex(_mutex); MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0007", __FILE__, __FUNCSIG__); }
}

//Delete the current selected fixed shape...//
void FixedToolCollection::RemoveSelected()
{
	try
	{
		if(CurrentFixedShape == NULL) return;
		IsValid = false;
		FixedShapeCollection.erase(CurrentFixedShape->getId());
		delete CurrentFixedShape;
		IsValid = true;
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0008", __FILE__, __FUNCSIG__); }
}

//Clear all fixed shapes...//
void FixedToolCollection::clearFixedTool(bool clearAll)
{
	try
	{
		IsValid = false;
		WaitForSingleObject( _mutex, INFINITE);
		if(clearAll)
		{
			if(FixedShapeCollection.size() != 0) 
			{
				for(map<int, FixedShape*>::iterator It = FixedShapeCollection.begin(); It != FixedShapeCollection.end(); )
				{
					if(It->second->DontClear())
					{
						It++;
					}
					else
					{
						FixedShape* fs = It->second;
						FixedShapeCollection.erase(It++);
						delete fs;
					}
				}
			}
		}
		else
		{
	    	while(FixedShapeCollection.size() != 0)
			{
				FG_ITER i = FixedShapeCollection.begin();
				FixedShape* fs = (*i).second;
				FixedShapeCollection.erase(fs->getId());
				delete fs;
			}
		}
		ReleaseMutex(_mutex);
		if(FixedShapeCollection.size() == 0)
		   this->CurrentFixedShape = NULL;
		else
		   this->CurrentFixedShape = FixedShapeCollection.begin()->second;
		this->IsValid = true;
		//this->HatcedFixedGraphics(false);
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ ReleaseMutex(_mutex); MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0009", __FILE__, __FUNCSIG__); }
}

//Current Selected shape..
int FixedToolCollection::CurrentSelectedFixedshape()
{
	if(CurrentFixedShape == NULL) return 0;
	return (int)CurrentFixedShape->FShapeType;
}

//Hatched fixed shape..
bool FixedToolCollection::FixedHatched()
{
	try
	{
		if(CurrentFixedShape == NULL) return false;
		return CurrentFixedShape->HatcedShp();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL009a", __FILE__, __FUNCSIG__); return 0; }
}

bool FixedToolCollection::FixedNonClearable()
{
	try
	{
		if(CurrentFixedShape == NULL) return false;
		return CurrentFixedShape->DontClear();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL009a", __FILE__, __FUNCSIG__); return 0; }
}

//Current circle type..
bool FixedToolCollection::FixedCircleType()
{
	try
	{
		if(CurrentFixedShape == NULL) return false;
		if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FCIRCLE)
			return ((FixedCircle*)CurrentFixedShape)->DiameterType;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0010", __FILE__, __FUNCSIG__); return 0; }
}

//Fixed circle radius...//
double FixedToolCollection::FixedRadius()
{
	try
	{
		if(CurrentFixedShape == NULL) return 0;
		if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FLINE)
			return (((FixedLine*)CurrentFixedShape)->GetLineType());
		else if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FCIRCLE)
		{
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				return (((FixedCircle*)CurrentFixedShape)->Radius()/25.4);
			else
				return (((FixedCircle*)CurrentFixedShape)->Radius());
		}
		else if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FTEXT)
			return (((FixedText*)CurrentFixedShape)->getAngle());
		return 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0011", __FILE__, __FUNCSIG__); return 0; }
}

//Fixed string..//
char* FixedToolCollection::FixedString()
{
	try
	{
		if(CurrentFixedShape == NULL) return "";
		if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FTEXT)
			return ((FixedText*)CurrentFixedShape)->getText();
		return "";
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0012", __FILE__, __FUNCSIG__); return ""; }
}

//Position X of the selected shapes..//
double FixedToolCollection::FixedgetX()
{
	try
	{
		if(CurrentFixedShape == NULL) return 0;
		if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FLINE)
			return (((FixedLine*)CurrentFixedShape)->Angle());
		else if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FCIRCLE)
		{
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				return (((FixedCircle*)CurrentFixedShape)->getCenterX()/25.4);
			else
				return (((FixedCircle*)CurrentFixedShape)->getCenterX());
		}
		else if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FTEXT)
		{
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				return (((FixedText*)CurrentFixedShape)->getPointX()/25.4);
			else
				return (((FixedText*)CurrentFixedShape)->getPointX());
		}
		return 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0013", __FILE__, __FUNCSIG__); return 0; }
}

//Position Y of selected shape
double FixedToolCollection::FixedgetY()
{
	try
	{
		if(CurrentFixedShape == NULL) return 0;
		if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FLINE)
		{
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				return (((FixedLine*)CurrentFixedShape)->getOffset()/25.4);
			else
				return (((FixedLine*)CurrentFixedShape)->getOffset());
		}
		else if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FCIRCLE)
		{
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				return (((FixedCircle*)CurrentFixedShape)->getCenterY()/25.4);
			else
				return (((FixedCircle*)CurrentFixedShape)->getCenterY());
		}
		else if(CurrentFixedShape->FShapeType == RapidEnums::FIXEDSHAPETYPE::FTEXT)
		{
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				return (((FixedText*)CurrentFixedShape)->getPointY()/25.4);
			else
				return (((FixedText*)CurrentFixedShape)->getPointY());
		}
		return 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCOLL0014", __FILE__, __FUNCSIG__); return 0; }
}