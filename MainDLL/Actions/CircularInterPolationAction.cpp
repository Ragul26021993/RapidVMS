#include "StdAfx.h"
#include "CircularInterPolationAction.h"
#include "..\Engine\RCadApp.h"

CircularInterPolationAction::CircularInterPolationAction():RAction(_T("CirInt"))
{
	try
	{ 
		this->SweepAngle = 0; this->Hopscount = 0; this->PausableAction = false;
		this->shape = NULL; this->ShapeUsed = true; this->AddPtFlag = false;
		this->CurrentActionType = RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CirInt001", __FILE__, __FUNCSIG__); }
}

CircularInterPolationAction::~CircularInterPolationAction()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CirInt002", __FILE__, __FUNCSIG__); }
}

bool CircularInterPolationAction::execute()
{
	ActionStatus(true);
	return true;
}

bool CircularInterPolationAction::redo()
{
	ActionStatus(true);
	((ShapeWithList*)this->shape)->PointAtionList.push_back(this);
	return true;
}

void CircularInterPolationAction::undo()
{
	((ShapeWithList*)this->shape)->PointAtionList.remove(this);
	ActionStatus(false);
}

void CircularInterPolationAction::AddCircularInterPolationAction(Shape* s, double* CircleCent, double* Nvector, double* CurrentPosition, double SweepAng, int HopsCnt, bool AddpointFlag)
{
	try
	{ 
		CircularInterPolationAction *action = new CircularInterPolationAction();
		action->shape = s;
		((ShapeWithList*)s)->PointAtionList.push_back(action);
		action->Center.set(CircleCent[0], CircleCent[1], CircleCent[2]);
		action->CurrentPos.set(CurrentPosition[0], CurrentPosition[1], CurrentPosition[2]);
		action->NormalVector.set(Nvector[0], Nvector[1], Nvector[2]);
		action->SweepAngle = SweepAng;
		action->AddPtFlag = AddpointFlag;
		action->Hopscount = HopsCnt;
		action->ShapeUsed = true;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CirInt003", __FILE__, __FUNCSIG__); }
}

void CircularInterPolationAction::AddCircularInterPolationAction(double* CircleCent, double* Nvector, double* CurrentPosition, double SweepAng, int HopsCnt)
{
	try
	{ 
		CircularInterPolationAction *action = new CircularInterPolationAction();
		action->ShapeUsed = false;		
		action->Center.set(CircleCent[0], CircleCent[1], CircleCent[2]);
		action->CurrentPos.set(CurrentPosition[0], CurrentPosition[1], CurrentPosition[2]);
		action->NormalVector.set(Nvector[0], Nvector[1], Nvector[2]);
		action->SweepAngle = SweepAng;
		action->Hopscount = HopsCnt;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CirInt003", __FILE__, __FUNCSIG__); }
}

void CircularInterPolationAction::Translate(Vector pt1)
{
	try
	{
		//there is no need to translate normal vector, we need to change normal vector in case of transform(rotation)...
		Center += pt1;
		CurrentPos += pt1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CirInt004", __FILE__, __FUNCSIG__); }
}

Vector* CircularInterPolationAction::GetCenter()
{
	return &(this->Center);
}

Vector* CircularInterPolationAction::GetNormalVector()
{
	return &(this->NormalVector);
}

Vector* CircularInterPolationAction::GetCurrentPosition()
{
	return &(this->CurrentPos);
}

double CircularInterPolationAction::GetSweepAngle()
{
	return this->SweepAngle;
}

int CircularInterPolationAction::GetHopsCount()
{
	return this->Hopscount;
}

void CircularInterPolationAction::SetCenter(Vector& pt)
{
	this->Center.set(pt.getX(), pt.getY(), pt.getZ());
}

void CircularInterPolationAction::Transform(double* Tmatrix)
{
}
	
void CircularInterPolationAction::SetCurrentPosition(Vector& pt)
{
	this->CurrentPos.set(pt.getX(), pt.getY(), pt.getZ());
}

void CircularInterPolationAction::SetNormalVector(Vector& pt)
{
	this->NormalVector.set(pt.getX(), pt.getY(), pt.getZ());
}

double CircularInterPolationAction::GetRadius()
{
	try
	{
		double Pt1[3] = {this->Center.getX(), this->Center.getY(), this->Center.getZ()};
		double Pt2[3] = {this->CurrentPos.getX(), this->CurrentPos.getY(), this->CurrentPos.getZ()};
		return RMATH3DOBJECT->Distance_Point_Point(Pt1, Pt2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CirInt005", __FILE__, __FUNCSIG__); return 0;}
}

Shape* CircularInterPolationAction::getShape()
{
	return this->shape;
}

bool CircularInterPolationAction::GetShapeInfo()
{
	return this->ShapeUsed;
}

bool CircularInterPolationAction::GetAddPointInfo()
{
	return this->AddPtFlag;
}

wostream& operator<<(wostream& os, CircularInterPolationAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "CircularInterPolationAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "PausableAction:" << action.PausableAction << endl; 		
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsId:" << action.shape->UcsId() << endl;
		os << "AddPtFlag:" << action.AddPtFlag << endl;
		os << "ShapeUsedFlag:" << action.ShapeUsed << endl;
		if(action.ShapeUsed)
			os << "ActionShapeId:" << action.shape->getId() << endl;
		os << "NumberOfHops:" << action.Hopscount << endl;
		os << "SweepAngle:" << action.SweepAngle << endl;
		os << "Center:values" << endl << (*static_cast<Vector*>(action.GetCenter())) << endl;
		os << "NormalVector:values" << endl << (*static_cast<Vector*>(action.GetNormalVector())) << endl;
		os << "CurrentPos:values" << endl << (*static_cast<Vector*>(action.GetCurrentPosition())) << endl;
		os << "EndCircularInterPolationAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CirInt006", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>(wistream& is, CircularInterPolationAction& action)
{
	try
	{
		int Ucsid;
		UCS* ucs;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"CircularInterPolationAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndCircularInterPolationAction")
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
					else if(Tagname==L"PausableAction")
					{
						if(Val=="0")
							action.PausableAction = false;
						else
							action.PausableAction = true;
					}
					else if(Tagname==L"OriginalName")
					{
						action.setOriginalName(TagVal);
					}
					else if(Tagname==L"UcsId")
					{
						Ucsid = atoi((const char*)(Val).c_str());
						ucs = MAINDllOBJECT->getUCS(Ucsid);
					}
					else if(Tagname==L"AddPtFlag")
					{
						if(Val=="0")
							action.AddPtFlag = false;
						else
							action.AddPtFlag = true;
					}
					else if(Tagname==L"ShapeUsedFlag")
					{
						if(Val=="0")
							action.ShapeUsed = false;
						else
							action.ShapeUsed = true;
					}
					else if(Tagname==L"ActionShapeId")
					{
						int n = atoi((const char*)(Val).c_str()); 
						if(ucs->getShapes().ItemExists(n))
						{
							action.shape = (Shape*)(ucs->getShapes().getList()[n]);
							action.shape->UcsId(Ucsid);
						}
					}
					if(Tagname==L"NumberOfHops")
					{
						action.Hopscount = atoi((const char*)(Val).c_str());
					}
					if(Tagname==L"SweepAngle")
					{
						action.SweepAngle = atof((const char*)(Val).c_str());
					}
					else if(Tagname==L"Center")
					{	
						is >> (*static_cast<Vector*>(&action.Center));						
					}
					else if(Tagname==L"NormalVector")
					{	
						is >> (*static_cast<Vector*>(&action.NormalVector));						
					}
					else if(Tagname==L"CurrentPos")
					{	
						is >> (*static_cast<Vector*>(&action.CurrentPos));						
					}
				}
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("CirInt007", __FILE__, __FUNCSIG__); return is; }
}