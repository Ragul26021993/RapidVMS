#include "StdAfx.h"
#include "UCS.h"
#include "RCadApp.h"
#include "../Actions/AddUCSAction.h"
#include "../Shapes/Line.h"

//Constructor...
UCS::UCS(TCHAR *myname, bool AddAxis):BaseItem(genName(myname))
{
	try
	{
		init();
		if(AddAxis)
		{
			MAINDllOBJECT->AddAxisShapes(this);
			MAINDllOBJECT->DrawCoordinatePlaneFor3DMode(this);
			MAINDllOBJECT->AddMarkingShape(this);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RUCS0001", __FILE__, __FUNCSIG__); }
}

//Initialise the snap point manager....//
void UCS::init()
{
	try
	{
		UCSangle(0); UCSMode(0); UCSIntercept(0); UCSAngle_B2S = 0;
		UCSPoint.set(0, 0, 0); UCSPt_B2S.set(0, 0, 0);
		KeepGraphicsStraightMode(false);
		currentDroX(0); currentDroY(0); currentDroZ(0); currentDroR(0);
		RMATH2DOBJECT->LoadIdentityMatrix(&transform[0], 3);
		isptmgr = new SnapPtManager(&shapes);
		RelatedUCSChangeAction = new RCollectionBase(false);
		PPshapesCollection_Original = new RCollectionBase(false);
		PPshapesCollection_ForPath = new RCollectionBase(false);
		PPMeasureCollection_Original = new RCollectionBase(false);
		PPMeasureCollection_ForPath = new RCollectionBase(false);
		AxisLine = NULL; CenterPt = NULL;
		AddingFirstTime(true);
		ParentUcsId(0);
		ChildUcsId(0);
		MarkingShape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RUCS0003", __FILE__, __FUNCSIG__); }
}

//Funcntion to generate the UCS name..
TCHAR* UCS::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		if(prefix != _T("MCS"))
			UCSnumber++;
		else
			return name;
		TCHAR shapenumstr[10];
		_itot_s(UCSnumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RUCS0004", __FILE__, __FUNCSIG__); return _T("NA");}
}

//Destructor..//
UCS::~UCS()
{
	try
	{
		delete RelatedUCSChangeAction;
		delete isptmgr;
		AxisLine = NULL; CenterPt = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RUCS0002", __FILE__, __FUNCSIG__); }
}

//Clear all the entities in the current clear..
void UCS::clearAll()
{
	try
	{
		this->getShapes().deleteAll();
		this->getDimensions().deleteAll();
		this->getIPManager().getIsectPts()->deleteAll();
		this->getShapes().getSelected().clearAll();
		this->getDimensions().getSelected().clearAll();
		this->RelatedUCSChangeAction->clearAll();
		this->PPshapesCollection_Original->deleteAll();
		this->PPshapesCollection_ForPath->deleteAll();
		this->PPMeasureCollection_Original->deleteAll();
		this->PPMeasureCollection_ForPath->deleteAll();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RUCS0004", __FILE__, __FUNCSIG__); }
}

UCS* UCS::CreateUcs(int ucsmode, Vector* origin)
{
	try
	{
		if(ucsmode == 0)
		{
			MAINDllOBJECT->AddingUCSModeFlag = true;
			UCS* nucs = new UCS(_T("UCS"));
			UCS& cucs = MAINDllOBJECT->getCurrentUCS();
			nucs->UCSMode(1);
			//nucs->UCSPoint.set(origin->getX(), origin->getY(), origin->getZ());
			nucs->SetUCSProjectionType(MAINDllOBJECT->UCSProjectionType);
			//nucs->getWindow(0).init(cucs.getWindow(0));
			//nucs->getWindow(1).init(cucs.getWindow(1));
			//nucs->getWindow(2).init(cucs.getWindow(2));
			for (int j = 0; j < 3; j++)
			{
				nucs->getWindow(j).init(cucs.getWindow(j));
				nucs->getWindow(j).WindowNo = j;
			}
			nucs->CenterPt = MAINDllOBJECT->highlightedPoint();
			nucs->SetParameters(0, origin);
			AddUCSAction::addUCS(nucs);
			nucs->getWindow(1).homeIt(1);
			//nucs->SetKeepgraphicsStraightflag(MAINDllOBJECT->KeepGraphicsStflag);
			MAINDllOBJECT->centerScreen(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
			//Update graphics...//
			MAINDllOBJECT->updateAll();
			MAINDllOBJECT->AddingUCSModeFlag = false;
			return nucs;
		}
	}
	catch(...)
	{
	   return NULL;
	}
}

//Get the shape collections of the current UCS..!
RCollectionBase& UCS::getShapes()
{
	return shapes;
}

//Returns the dimension collection...
RCollectionBase& UCS::getDimensions()
{
	return dimensions;
}

//Returns the dimension collection...
RCollectionBase& UCS::getDxfCollecion()
{
	return dxfColl;
}

//Returns the intersection point manager...//
SnapPtManager& UCS::getIPManager()
{
	return *(this->isptmgr);
}

//Get the window according to the no..
RWindow& UCS::getWindow(int no)
{
	return windows[no];
}

 //partprogram Shape Collection.
RCollectionBase& UCS::getPPShapes_Original()
{
	return *PPshapesCollection_Original;
}
	
//partprogram Shape Collection for path.
RCollectionBase& UCS::getPPShapes_Path()
{
	return *PPshapesCollection_ForPath;
}

//partprogram measure Collection.
RCollectionBase& UCS::getPPMeasures_Original()
{
	return *PPMeasureCollection_Original;
}

//partprogram measure Collection for path.
RCollectionBase& UCS::getPPMeasures_Path()
{
	return *PPMeasureCollection_ForPath;
}


//Set Current DRO..
void UCS::SetCurrentUCS_DRO(Vector* v)
{
	DROPostn.set(*v);
}

//Returns current DRO
Vector UCS::GetCurrentUCS_DROpostn()
{
	return DROPostn;
}

//UCS number index..
int UCS::UCSnumber = 0;
void UCS::reset()
{
	UCSnumber = 0;
}

void UCS::SetUCSProjectionType(int i)
{
	try
	{
		UCSProjectionType(i);
		if(this->shapes.count() >= 3)
		{
			if(i == 0) //XY
			{
				X_AxisLine->setModifiedName("X Axis");
				Y_AxisLine->setModifiedName("Y Axis");
				Z_AxisLine->setModifiedName("Z Axis");
			}
			else if(i == 1) //YZ
			{
				X_AxisLine->setModifiedName("Y Axis");
				Y_AxisLine->setModifiedName("Z Axis");
				Z_AxisLine->setModifiedName("X Axis");
			}
			else if(i == 2) //XZ
			{
				X_AxisLine->setModifiedName("X Axis");
				Y_AxisLine->setModifiedName("Z Axis");
				Z_AxisLine->setModifiedName("Y Axis");
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RUCS0005", __FILE__, __FUNCSIG__); }
}


//Set The Angle and recalculate the Transformation Matrix in 2D
void UCS::SetParameters(double Angle, Vector* Origin)
{
	int numl = 0;
	try
	{
		//Set the internal variables....
		this->UCSangle(Angle);
		numl++;
		this->UCSPoint.set(Origin->getX(), Origin->getY(), Origin->getZ(), Origin->getR());
		numl++;
		//Calculate sin and cos
		double sinTheta = sin(Angle), cosTheta = cos(Angle);
		//Assign the values for the transformation matrix
		//This is a pure rotation around the Z-axis ONLY!
		numl++;
		this->transform[0] = cosTheta;		this->transform[1] = sinTheta;		this->transform[2] = -1 * (this->UCSPoint.getY() * sinTheta + this->UCSPoint.getX() * cosTheta); //0; //this->transform[2] = 0;	
		this->transform[3] = -sinTheta;		this->transform[4] = cosTheta;		this->transform[5] = this->UCSPoint.getX() * sinTheta - this->UCSPoint.getY() * cosTheta; //0; //this->transform[6] = 0;
		this->transform[6] = 0;				this->transform[7] = 0;				this->transform[8] = 1; //this->transform[10] = 1;
		numl++;
		//this->transform[12] = 0; //this->UCSPoint.getX() * (1 - cosTheta) - this->UCSPoint.getY() * sinTheta;	
		//this->transform[13] = 0; 
		//this->transform[14] = 0;	
		//this->transform[15] = 1;
		MAINDllOBJECT->UCSParamChanged();
	}
	catch (...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RUCS0005a" + std::to_string(numl), __FILE__, __FUNCSIG__);
	}
}

//Partprogram... Write...///
wostream& operator<<(wostream& os, UCS& x)
{
	try
	{
		os << x.getId() << endl;
		os << x.UCSangle() << endl;
		os << x.UCSIntercept() << endl;
		os << "ParentUcsId" << endl;
		os << x.ParentUcsId() << endl;
		os << x.ChildUcsId() << endl;
		os << x.UCSMode() << endl;
		//os << "AxisLine" << endl;
		//os << x.AxisLine->getId() << endl;
		os << x.UCSProjectionType() << endl;
		os << (static_cast<Vector>(x.DROPostn)) << endl;
		os << (static_cast<Vector>(x.UCSPoint)) << endl;
		os << x.currentDroX() << endl;
		os << x.currentDroY() << endl;
		os << x.currentDroZ() << endl;
		os << x.currentDroR() << endl;
		if(x.UCSMode() == 2 && x.UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			os << x.AxisLine->getId() << endl;
		if(x.UCSMode() == 4)
		{
			os << x.ShapesCol.size() << endl;
			for(map<int, Shape*>::iterator It1 = x.ShapesCol.begin(); It1 != x.ShapesCol.end(); It1++)
			{
				os << It1->second->getId() << endl;
			}
			os << x.PointsCol.size() << endl;
			for(map<int, Vector*>::iterator It2 = x.PointsCol.begin(); It2 != x.PointsCol.end(); It2++)
			{
				os << (*static_cast<Vector*>(It2->second)) << endl;
			}
		}
		os << x.getWindow(0).getUppX() << endl;
		os << x.getWindow(0).getUppY() << endl;
		os << x.getWindow(0).getUppf() << endl;
		os << x.getWindow(1).getUppX() << endl;
		os << x.getWindow(1).getUppf() << endl;
		os << x.getWindow(2).getUppX() << endl;
		os << x.getWindow(2).getUppf() << endl;
		RC_ITER item = x.shapes.getList().begin();
		os << (*item).second->getId() << endl;
		item++;
		os << (*item).second->getId() << endl;
		item++;
		os << (*item).second->getId() << endl;
		item++;
		os << (*item).second->getId() << endl;
		item++;
		os << (*item).second->getId() << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RUCS0006", __FILE__, __FUNCSIG__); return os;}
}

//Part program read....///
wistream& operator>> (wistream& is, UCS& x)
{
	try
	{
		int n,k;
		double upp, upp1, uppf;
		is >> n;
		x.setId(n);
		is >> upp; x.UCSangle(upp); x.UCSAngle_B2S = upp;
		is >> upp; x.UCSIntercept(upp);
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname == L"ParentUcsId")
		{
			is >> n; x.ParentUcsId(n);
			is >> n; x.ChildUcsId(n);
			is >> n; x.UCSMode(n);
		}
		else
			 x.UCSMode(atoi((const char*)(Tagname).c_str()));
		//Calculate the transformation matix....
		double CenterPt[2] = {0}, UcsAngle = x.UCSangle();
		RMATH2DOBJECT->TransformM_RotateAboutPoint(&CenterPt[0], -UcsAngle, &x.transform[0]);
		//is >> Tagname;
		//if (Tagname == L"AxisLine")
		//{
		//	is >> n;
		//	if (MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(n))
		//		x.AxisLine = MAINDllOBJECT->getCurrentUCS().getShapes().getList[n];
		//	//Now read Projection type
		//	is >> n;
		//}
		//else
		//	n = atoi((const char*)(Tagname).c_str());
		is >> n;
		x.UCSProjectionType(RapidEnums::RAPIDPROJECTIONTYPE(n));

		is >> (*static_cast<Vector*>(&x.DROPostn));
		is >> (*static_cast<Vector*>(&x.UCSPoint));
		is >> upp;  x.currentDroX(upp);
		is >> upp;  x.currentDroY(upp);
		is >> upp;  x.currentDroZ(upp);
		is >> upp;  x.currentDroR(upp);
		if(x.UCSMode() == 2  && x.UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
		{
			is >> n;
			x.AxisLine = (Shape*)((UCS*)MAINDllOBJECT->getUCSList().getList()[x.ParentUcsId()])->getShapes().getList()[n];
			//x.AxisLine->addListener((ItemListener*)&x);
		}
		if(x.UCSMode() == 4)
		{
			is >> n;
			for(int i = 0; i < n; i++)
			{
				is >> k;
				x.ShapesCol[i] = (Shape*) MAINDllOBJECT->getUCS(0)->getShapes().getList()[k];
			}
			is >> n;
			for(int i = 0; i < n; i++)
			{
				x.PointsCol[i] = new Vector(0,0,0);
				is >> (*static_cast<Vector*>(x.PointsCol[i]));
			}
		}
		if(x.UCSMode() == 1 || x.UCSMode() == 2)
			x.CenterPt = ((UCS*)MAINDllOBJECT->getUCSList().getList()[x.ParentUcsId()])->getIPManager().getNearestPoint(x.UCSPoint.getX(), x.UCSPoint.getY(), 0.01);
		x.OriginalUCSPoint.set(x.UCSPoint);
		x.UCSPt_B2S.set(x.UCSPoint);
		is >> upp;
		is >> upp1;
		is >> uppf;
		x.getWindow(0).init(MAINDllOBJECT->getCurrentUCS().getWindow(0), upp, upp1, uppf);
		is >> upp;
		is >> uppf;
		x.getWindow(1).init(MAINDllOBJECT->getCurrentUCS().getWindow(1), upp, upp, uppf);
		is >> upp;
		is >> uppf;
		x.getWindow(2).init(MAINDllOBJECT->getCurrentUCS().getWindow(2), upp, upp, uppf);
		x.getWindow(1).changeZoom(true);
		x.getWindow(2).changeZoom(true);
		/*if(x.UCSMode() == 0)
			x.getWindow(1).homeIt(1);*/
		RC_ITER item = x.shapes.getList().begin();
		BaseItem* Citem1 = (*item).second;
		item++;
		BaseItem* Citem2 = (*item).second;
		item++;
		BaseItem* Citem3 = (*item).second;
		item++;
		BaseItem* Citem4 = (*item).second;
		item++;
		BaseItem* Citem5 = (*item).second;
		x.shapes.EraseItem(Citem1, false);
		x.shapes.EraseItem(Citem2, false);
		x.shapes.EraseItem(Citem3, false);
		x.shapes.EraseItem(Citem4, false);
		x.shapes.EraseItem(Citem5, false);

		is >> n;  Citem1->setId(n);
		x.shapes.addItem(Citem1, false);
		is >> n;  Citem2->setId(n);
		x.shapes.addItem(Citem2, false);
		is >> n;  Citem3->setId(n);
		x.shapes.addItem(Citem3, false);
		is >> n;  Citem4->setId(n);
		x.shapes.addItem(Citem4, false);
		is >> n;  Citem5->setId(n);
		x.shapes.addItem(Citem5, false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RUCS0007", __FILE__, __FUNCSIG__); return is; }
}