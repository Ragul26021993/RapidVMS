#include "StdAfx.h"
#include "RotationAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Shape.h"

RotationAction::RotationAction():RAction(_T("ReflectionShape"))
{
	try
	{
		this->CurrentActionType = RapidEnums::ACTIONTYPE::ROTATIONACTION;
		memset(RotateMatrix, 0, sizeof(double) * 9);
		memset(RotationOrigin, 0, sizeof(double) * 3);
		RotationPoint = NULL;
		RotationAxisId = -1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0001", __FILE__, __FUNCSIG__); }
}

RotationAction::~RotationAction()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0002", __FILE__, __FUNCSIG__); }
}

bool RotationAction::execute()
{
	try
	{
	   RCollectionBase& rshapes = MAINDllOBJECT->getCurrentUCS().getShapes();
	   for(std::list<int>::iterator i = RotatedShapesId.begin(); i!= RotatedShapesId.end(); i++)
			RotatedShapes[*i] = (Shape*)rshapes.getList()[*i];
	   ActionStatus(true);
	   return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0003", __FILE__, __FUNCSIG__); return false; }
}

bool RotationAction::redo()
{
	try
	{
		for(map<int, Shape*>::iterator i = RotatedShapes.begin(); i != RotatedShapes.end(); i++)
		{
			ShapeWithList* Current_Shape = (ShapeWithList*)(*i).second;
			if(!Current_Shape->Normalshape()) continue;
			if(!Current_Shape->PointsList->getList().empty())
			   Current_Shape->RotateShape(RotateMatrix, RotationOrigin);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0004", __FILE__, __FUNCSIG__); return false; }
}

void RotationAction::undo()
{
	try
	{
		double inverseM[9] = {0};
		RMATH3DOBJECT->OperateMatrix4X4(RotateMatrix, 3, 1, inverseM);
		for(map<int, Shape*>::iterator i = RotatedShapes.begin(); i != RotatedShapes.end(); i++)
		{
			ShapeWithList* Current_Shape = (ShapeWithList*)(*i).second;
			if(!Current_Shape->Normalshape()) continue;
			if(!Current_Shape->PointsList->getList().empty())
			   Current_Shape->RotateShape(inverseM, RotationOrigin);
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0007", __FILE__, __FUNCSIG__); }
}


void RotationAction:: AddshapesColl(list<int>rotatedShapeId, int rotationAxisId)
{
	try
	{
		RotatedShapesId = rotatedShapeId;
	    int RotationAxisId = rotationAxisId;
		MAINDllOBJECT->addAction(this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0006", __FILE__, __FUNCSIG__); }
}


wostream& operator<<(wostream& os, RotationAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "RotationAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		for(list<int>::iterator It =  action.RotatedShapesId.begin(); It !=  action.RotatedShapesId.end(); It++)
			os << "RotatedShapesId:" << *It << endl;
		os << "RotationAxisId:" << action.RotationAxisId << endl;
		os << "Angle:" << action.Angle << endl;
		if(action.RotationPoint != NULL)
		   os <<"RotationPoint:values"<<endl<<(*static_cast<Vector*>(action.RotationPoint)) << endl;
		os << "EndRotationAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, RotationAction& action)
{
	try
	{
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"RotationAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::ROTATIONACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndRotationAction")
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
					else if(Tagname==L"RotatedShapesId")
					{
						action.RotatedShapesId.push_back(atoi((const char*)(Val).c_str()));
					}	
					else if(Tagname==L"RotationAxisId")
					{
						action.RotationAxisId=atoi((const char*)(Val).c_str());
					}	
					else if(Tagname==L"Angle")
					{
						action.Angle=atof((const char*)(Val).c_str());
					}	
					if(Tagname==L"RotationPoint")
					{
						action.RotationPoint = new Vector(0,0,0);
						is >> (*static_cast<Vector*>(action.RotationPoint));
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
