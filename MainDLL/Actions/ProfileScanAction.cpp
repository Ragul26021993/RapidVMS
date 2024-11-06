#include "StdAfx.h"
#include "ProfileScanAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"

ProfileScanAction::ProfileScanAction():RAction(_T("ProfileScan"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION;
	PausableAction = false; SurfaceON = false;
	CompleteProfile = false;
	SkipCount = 10; MaskFactor = 5; Binary_LowerFactor = 40; Binary_UpperFactor = 200; PixelThreshold = 40; MaskingLevel = 1;
	EndPt = NULL;
	CamProperty = "58FF0080010003"; LightProperty = "0000000000000000";
}

ProfileScanAction::~ProfileScanAction()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PScan001", __FILE__, __FUNCSIG__); }
}

bool ProfileScanAction::execute()
{
	ActionStatus(true);
	return true;
}

bool ProfileScanAction::redo()
{
	((ShapeWithList*)Cshape)->PointAtionList.push_back(this);
	ActionStatus(true);
	return true;
}

void ProfileScanAction::undo()
{
	((ShapeWithList*)Cshape)->PointAtionList.remove(this);
	ActionStatus(false);
}

void ProfileScanAction::AddProfileScanAction(Shape* Shp, Vector Pt1, Vector Pt2, Vector* lastPt, Vector currentPosition, bool complete_profile, int PixelSkip, std::string lightsetting, std::string CamSetting, int* SurfaceAlgoProp, bool SurfaceLight)
{
	try
	{
		MAINDllOBJECT->ImageAction_Count++;
		ProfileScanAction *action = new ProfileScanAction();
		action->SkipCount = PixelSkip;
		action->Cshape = Shp;
		((ShapeWithList*)action->Cshape)->PointAtionList.push_back(action);
		action->FirstPt = Pt1;
		action->SecondPt = Pt2;
		action->EndPt = lastPt;
		action->CurrentPos = currentPosition;
		action->CompleteProfile = complete_profile;
		action->LightProperty = lightsetting;
		action->CamProperty = CamSetting;
		action->MaskFactor = SurfaceAlgoProp[0];
		action->Binary_LowerFactor  = SurfaceAlgoProp[1];
		action->Binary_UpperFactor  = SurfaceAlgoProp[2];
		action->PixelThreshold  = SurfaceAlgoProp[3];
		action->MaskingLevel  = SurfaceAlgoProp[4];
		action->SurfaceON = SurfaceLight;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PScan002", __FILE__, __FUNCSIG__); }
}

bool ProfileScanAction::GetSurfaceLifghtInfo()
{
	return this->SurfaceON;
}

bool ProfileScanAction::GetSurfaceAlgoParam(int *Param)
{
	try
	{
		Param[0] = MaskFactor; Param[1] = Binary_LowerFactor; Param[2] = Binary_UpperFactor; Param[3] = PixelThreshold; Param[4] = MaskingLevel;
		return this->SurfaceON;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PScan002a", __FILE__, __FUNCSIG__); }
}

Vector* ProfileScanAction::GetFirstPt()
{
	return &(this->FirstPt);
}

Vector* ProfileScanAction::GetSecondPt()
{
	return &(this->SecondPt);
}

Vector* ProfileScanAction::GetEndPt()
{
	return this->EndPt;
}

Vector* ProfileScanAction::GetCurrentPosition()
{
	return &(this->CurrentPos);
}

int ProfileScanAction::GetPixelCount()
{
	return this->SkipCount;
}

void ProfileScanAction::SetFirstPt(Vector& pt)
{
	this->FirstPt = pt;
}

void ProfileScanAction::SetSecondPt(Vector& pt)
{
	this->SecondPt = pt;
}

void ProfileScanAction::SetCurrentPos(Vector& pt)
{
	this->CurrentPos = pt;
}

Shape* ProfileScanAction::getShape()
{
	return this->Cshape;
}

bool ProfileScanAction::GetProfileInfo()
{
	return this->CompleteProfile;
}

string ProfileScanAction::GetCam_Property()
{
	return this->CamProperty;
}

char* ProfileScanAction::GetLight_Property()
{
	return (char*)LightProperty.c_str();
}

void ProfileScanAction::Translate(Vector pt1)
{
	this->FirstPt += pt1;
	this->SecondPt += pt1;
	this->CurrentPos += pt1;
}
	
void ProfileScanAction::Transform(double* transform)
{
	try
	{
		Vector Temp;
		Temp = MAINDllOBJECT->TransformMultiply(transform, FirstPt.getX(), FirstPt.getY(), FirstPt.getZ());
		FirstPt.set(Temp.getX(), Temp.getY(), Temp.getZ());
		Temp = MAINDllOBJECT->TransformMultiply(transform, SecondPt.getX(), SecondPt.getY(), SecondPt.getZ());
		SecondPt.set(Temp.getX(), Temp.getY(), Temp.getZ());
		Temp = MAINDllOBJECT->TransformMultiply(transform, CurrentPos.getX(), CurrentPos.getY(), CurrentPos.getZ());
		CurrentPos.set(Temp.getX(), Temp.getY(), Temp.getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PScan005", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, ProfileScanAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "ProfileScanAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "PausableAction:" << action.PausableAction << endl; 		
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "CamProperties:" << (char*)action.GetCam_Property().c_str() << endl;
		os << "LightIntensity:" << action.GetLight_Property() << endl;
		os << "UcsId:" << action.Cshape->UcsId() << endl;
		os << "ActionShapeId:" << action.Cshape->getId() << endl;
		os << "NumberOfPixel:" << action.SkipCount << endl;
		os << "MaskFactor:" << action.MaskFactor << endl;
		os << "Binary_LowerFactor:" << action.Binary_LowerFactor << endl;
		os << "Binary_UpperFactor:" << action.Binary_UpperFactor << endl;
		os << "PixelThreshold:" << action.PixelThreshold << endl;
		os << "MaskingLevel:" << action.MaskingLevel << endl;
		os << "CompleteProfile:" << action.CompleteProfile << endl;
		os << "SurfaceON:" << action.SurfaceON << endl;
		os << "FirstPt:values" << endl << (*static_cast<Vector*>(action.GetFirstPt())) << endl;
		os << "SecondPt:values" << endl << (*static_cast<Vector*>(action.GetSecondPt())) << endl;
		if(!action.CompleteProfile)
			os << "EndPt:values" << endl << (static_cast<Vector>(*action.EndPt)) << endl;
		os << "CurrentPos:values" << endl << (*static_cast<Vector*>(action.GetCurrentPosition())) << endl;
		os << "EndProfileScanAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PScan003", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>(wistream& is, ProfileScanAction& action)
{
	try
	{
		int Ucsid;
		UCS* ucs;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"ProfileScanAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndProfileScanAction")
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
					if(Tagname==L"CurrentActionType")
					{						
					}
					else if(Tagname==L"Id")
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
					else if(Tagname==L"CamProperties")
					{
						action.CamProperty = RMATH2DOBJECT->WStringToString(TagVal);
					}
					else if(Tagname==L"LightIntensity")
					{
						action.LightProperty = RMATH2DOBJECT->WStringToString(TagVal);
					}
					else if(Tagname==L"UcsId")
					{
						Ucsid = atoi((const char*)(Val).c_str());
						ucs = MAINDllOBJECT->getUCS(Ucsid);
					}
					else if(Tagname==L"MaskFactor")
					{
						action.MaskFactor = atoi((const char*)(Val).c_str());
					}
					else if(Tagname==L"Binary_LowerFactor")
					{
						action.Binary_LowerFactor = atoi((const char*)(Val).c_str());
					}
					else if(Tagname==L"Binary_UpperFactor")
					{
						action.Binary_UpperFactor = atoi((const char*)(Val).c_str());
					}
					else if(Tagname==L"PixelThreshold")
					{
						action.PixelThreshold = atoi((const char*)(Val).c_str());
					}
					else if(Tagname==L"MaskingLevel")
					{
						action.MaskingLevel = atoi((const char*)(Val).c_str());
					}
					else if(Tagname==L"CompleteProfile")
					{
						if(Val=="0")
							action.CompleteProfile = false;
						else
							action.CompleteProfile = true;
					}
					else if(Tagname==L"SurfaceON")
					{
						if(Val=="0")
							action.SurfaceON = false;
						else
							action.SurfaceON = true;
					}
					else if(Tagname==L"NumberOfPixel")
					{
						action.SkipCount = atoi((const char*)(Val).c_str()); 
					}
					else if(Tagname==L"ActionShapeId")
					{
						int n = atoi((const char*)(Val).c_str()); 
						action.Cshape = (Shape*)(ucs->getShapes().getList()[n]);
						action.Cshape->UcsId(Ucsid);
						if(PPCALCOBJECT->PartprogramisLoadingEditMode())
								((ShapeWithList*)action.Cshape)->PointAtionList.push_back(&action);
					}
					else if(Tagname==L"FirstPt")
					{	
						is >> (*static_cast<Vector*>(&action.FirstPt));						
					}
					else if(Tagname==L"SecondPt")
					{	
						is >> (*static_cast<Vector*>(&action.SecondPt));						
					}
					else if(Tagname==L"EndPt")
					{	
						Vector Temp;
						is >>(*static_cast<Vector*>(&Temp));
						UCS* ucs = MAINDllOBJECT->getUCS(Ucsid);
						action.EndPt = MAINDllOBJECT->getVectorPointer_UCS(&Temp, ucs, true);						
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("IM004", __FILE__, __FUNCSIG__); return is; }
}