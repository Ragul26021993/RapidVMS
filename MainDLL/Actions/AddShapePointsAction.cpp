#include "StdAfx.h"
#include "AddShapePointsAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"

AddShapePointsAction::AddShapePointsAction(void):RAction(_T("AddedShapePoints"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::ADDSHAPEPOINT_ACTION;
}


AddShapePointsAction::~AddShapePointsAction(void)
{
}

void AddShapePointsAction::setShape(Shape* shape)
{
	this->shape = shape;
	((ShapeWithList*)this->shape)->PointAtionList.push_back(this);
}

bool AddShapePointsAction::execute()
{
	try
	{
		ActionStatus(true);		
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0003", __FILE__, __FUNCSIG__); return false; }
}

Shape* AddShapePointsAction::getShape()
{
	return shape;
}


bool AddShapePointsAction::redo()
{
	try
	{
		ActionStatus(true);
		((ShapeWithList*)this->shape)->PointAtionList.push_back(this);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0004", __FILE__, __FUNCSIG__); return false; }
}

void AddShapePointsAction::undo()
{
	try
	{
		ActionStatus(false);
		((ShapeWithList*)this->shape)->PointAtionList.remove(this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0007", __FILE__, __FUNCSIG__); }
}


wostream& operator<<(wostream& os, AddShapePointsAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "AddShapePointsAction" << endl;

		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsId:" << action.shape->UcsId() << endl;
		os << "ActionShapeId:" << action.shape->getId() << endl;
		for each(Shape* Shp in action.AddedShapesId)
		{
			if(Shp != NULL)
				os << "AddedShapesId:" << Shp->getId() << endl;
		}
		os << "EndAddShapePointsAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, AddShapePointsAction& action)
{
	try
	{		
		int Ucsid = 0;
		UCS* ucs = MAINDllOBJECT->getUCS(Ucsid);;	
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"AddShapePointsAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDSHAPEPOINT_ACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndAddShapePointsAction")
			{	
				std::wstring Linestr;				
				is >> Linestr;
				if(is.eof())
				{
					MAINDllOBJECT->PPLoadSuccessful(false);	
					break;
				}
				int ColonIndx = Linestr.find(':');
				if(ColonIndx==-1)
				{
					Tagname=Linestr;
				}
				else
				{
					std::wstring Tag = Linestr.substr(0, ColonIndx);
					std::wstring TagVal = Linestr.substr(ColonIndx + 1, Linestr.length() - ColonIndx - 1);
					std::string Val(TagVal.begin(), TagVal.end() );
					Tagname=Tag;
					if(Tagname==L"Id")
					{
						action.setId(atoi((const char*)(Val).c_str()));
					}		
					else if(Tagname==L"OriginalName")
					{
						action.setOriginalName(TagVal);
					}
					else if(Tagname==L"UcsId")
					{
						Ucsid=atoi((const char*)(Val).c_str());
						ucs = MAINDllOBJECT->getUCS(Ucsid);
					}
					else if(Tagname==L"ActionShapeId")
					{
							int n = atoi((const char*)(Val).c_str()); 
							if(ucs->getShapes().ItemExists(n))
							{
								action.shape = (Shape*)(ucs->getShapes().getList()[n]);
								action.shape->UcsId(Ucsid);
								if(PPCALCOBJECT->PartprogramisLoadingEditMode())
									((ShapeWithList*)action.shape)->PointAtionList.push_back(&action);
							}
					}
					else if(Tagname==L"AddedShapesId")
					{
						int n = atoi((const char*)(Val).c_str());
						if(ucs->getShapes().ItemExists(n))
						{
					    	action.AddedShapesId.push_back((Shape*)(ucs->getShapes().getList()[n]));
						}
					}					
				}
			}
		}
		else
		{
			MAINDllOBJECT->PPLoadSuccessful(false);				
		}		
	return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0008", __FILE__, __FUNCSIG__); return is; }
}