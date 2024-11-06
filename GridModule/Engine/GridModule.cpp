// GridModule.cpp : Defines the exported functions for the DLL application.
//
#include "StdAfx.h"
#include "GridModule.h"

GridModule::GridModule(std::string Gridname, bool ShowRowColumnHeaders)
{
	try
	{
		MyGridName = Gridname;
		ShowRCHeaders = ShowRowColumnHeaders;
		GridTop = 0; GridLeft = 0; 
		GridBottom = 0; GridRight = 0;
		GridWidth = 0; GridHeight = 0; 
		CellWidth = 0; CellHeight = 0;
		UserOnlyWH = true; UseRowColumnsCount = false; 
		UseTwoCornersWH = false; 
		RowsCount = 0; ColumnsCount = 0;
		SelectedGrect = NULL;
		temp = (char*)malloc(20);
		GridModuleError = NULL;
		GRectangle().reset();
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0001", __FILE__, __FUNCSIG__); }
}

GridModule::~GridModule()
{
}

void GridModule::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	if(GridModuleError != NULL)
		GridModuleError((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
}

void GridModule::SetLeftTop(double x, double y)
{
	GridLeft = x; 
	GridTop = y; 
}

void GridModule::SetBottomRight(double x, double y)
{
	GridRight = x; 
	GridBottom = y;
}

void GridModule::SetWidth(double width)
{
	GridWidth = width;
}

void GridModule::SetHeight(double height)
{
	GridHeight = height;
}

void GridModule::SetCellWidth(double width)
{
	CellWidth = width;
}

void GridModule::SetCellHeight(double height)
{
	CellHeight = height;
}

void GridModule::SetRows(int rows)
{
	RowsCount = rows;
}

void GridModule::SetColumns(int columns)
{
	ColumnsCount = columns;
}

void GridModule::SetTheGridCalcualtionType(bool UsingWidthHeight, bool UsingRowColumnCounts, bool Using2Corners)
{
	UserOnlyWH = UsingWidthHeight; 
	UseRowColumnsCount = UsingRowColumnCounts; 
	UseTwoCornersWH = Using2Corners;
}

void GridModule::CalculateRectangles(bool EffectiveMovementRowWise)
{
	try
	{
		GRectangle* GRect;
		if(UserOnlyWH)
		{
			ColumnsCount = GridWidth/ CellWidth;
			RowsCount = GridHeight/ CellHeight;
		}
		else if(UseRowColumnsCount)
		{
			GridWidth = ColumnsCount * CellWidth;
			GridHeight = RowsCount * CellHeight;
		}
		else if(UseTwoCornersWH)
		{
			GridWidth = abs(GridLeft -  GridRight);
			GridHeight = abs(GridTop -  GridBottom);
			ColumnsCount = GridWidth/ CellWidth;
			RowsCount = GridHeight/ CellHeight;
		}
		for(int i = 0; i < RowsCount; i++)
		{
			for(int j = 0; j < ColumnsCount; j++)
			{
				GRect = new GRectangle();
				GRect->SetLeftTop(GridLeft + j * CellWidth, GridTop - i * CellHeight, 0);
				GRect->SetBottomRight(GridLeft + (j + 1) * CellWidth, GridTop - (i + 1) * CellHeight, 0);
				GRect->CalculateEndpoints();
				GRect->valid(true);
				allitems[GRect->getId()] = GRect;
			}
		}
		if(EffectiveMovementRowWise)
		{
			/*int index=0;
			for(int i = 0; i < RowsCount; i++)
			{
				for(int j = 0; j < ColumnsCount; j++)
				{
					if(i % 2 == 0)
					{
						ItemsAccordingToEffectiveMovementInColumn[index]=allitems[i * ColumnsCount + j];						
					}
					else
					{
						ItemsAccordingToEffectiveMovementInColumn[index]=allitems[(i+1) * ColumnsCount - j - 1];						
					}
					index++;	
				}
			}*/
		}
		if(ShowRCHeaders)
		{
			GRect = new GRectangle();
			GRect->SetLeftTop(GridLeft - CellWidth/2, GridTop + CellHeight/2, 0);
			GRect->SetBottomRight(GridLeft, GridTop, 0);
			GRect->CalculateEndpoints();
			GRect->valid(true);
			GRect->SetName(MyGridName);
			RowColumnitems[GRect->getId()] = GRect;
			int num = 65, num1 = 65;
			int dp,sgn;
			char st[12];
			for(int j = 0; j < ColumnsCount; j++)
			{
				GRect = new GRectangle();
				GRect->SetLeftTop(GridLeft + j * CellWidth, GridTop + CellHeight/2, 0);
				GRect->SetBottomRight(GridLeft + (j + 1) * CellWidth, GridTop, 0);
				GRect->CalculateEndpoints();
				GRect->valid(true);
				if(num > 90)
				{
					st[0] = char(num - 91 + 65);
					st[1] = char(num1);
					st[2] = '\0';
					num1++;
					if(num1 > 90)
					{
						num1 = 65;
						num++;
					}
				}
				else
				{
					st[0] = char(num);
					st[1] = '\0';
					num++;
				}
				GRect->SetName(st);
				RowColumnitems[GRect->getId()] = GRect;
			}
			num = 1;
			for(int j = 0; j < RowsCount; j++)
			{
				GRect = new GRectangle();
				GRect->SetLeftTop(GridLeft - CellWidth/2, GridTop - j * CellHeight, 0);
				GRect->SetBottomRight(GridLeft, GridTop - (j + 1) * CellHeight, 0);
				GRect->CalculateEndpoints();
				GRect->valid(true);
				_fcvt_s(&st[0], 10, num,0,&dp, &sgn);
				num++;
				GRect->SetName(st);
				RowColumnitems[GRect->getId()] = GRect;
			}
		}
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0002", __FILE__, __FUNCSIG__); }
}

map<int,GRectangle*>& GridModule::getItemList()
{
	return allitems;
}

map<int,GRectangle*>& GridModule::getRCItemList()
{
	return RowColumnitems;
}

map<int,GRectangle*>& GridModule::getECItemList()
{
	return ItemsAccordingToEffectiveMovementInColumn;
}

std::list<GRectangle*>& GridModule::getSelectedItemList()
{
	return SelectedItemsList;
}

void GridModule::IncrementaccordingEffectiveMovement()
{
	try
	{
		for(GM_ITER item = ItemsAccordingToEffectiveMovementInColumn.begin(); item != ItemsAccordingToEffectiveMovementInColumn.end(); item++)
			((*item).second)->selected(false);
		GMitem++;
		if(GMitem == ItemsAccordingToEffectiveMovementInColumn.end())
			GMitem = ItemsAccordingToEffectiveMovementInColumn.begin();
		SelectedGrect = ((*GMitem).second);
		SelectedGrect->selected(true);
	}
catch(...){ SetAndRaiseErrorMessage("GRIDM0003", __FILE__, __FUNCSIG__); }
}

void GridModule::IncrementSelection()
{
	try
	{
		for(GM_ITER item = allitems.begin(); item != allitems.end(); item++)
			((*item).second)->selected(false);
		GMitem++;
		if(GMitem == allitems.end())
			GMitem = allitems.begin();
		SelectedGrect = ((*GMitem).second);
		SelectedGrect->selected(true);
	}
catch(...){ SetAndRaiseErrorMessage("GRIDM0003", __FILE__, __FUNCSIG__); }
}

void GridModule::IncrementSelectedSelection()
{
	try
	{
		SelectedGrect = (*GMSSitem);
		GMSSitem++;
		if(GMSSitem == SelectedItemsList.end())
			GMSSitem = SelectedItemsList.begin();

	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0004", __FILE__, __FUNCSIG__); }
}

void GridModule::IncrementaccordingSorted()
{
	try
	{
		SelectedGrect = (*GMSitem);
		GMSitem++;
		if(GMSitem == SortedItemsList.end())
			GMSitem = SortedItemsList.begin();

	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0005", __FILE__, __FUNCSIG__); }
}

void GridModule::ClearSelection()
{
	try
	{
		SelectedItemsList.clear();
		for(GM_ITER item = allitems.begin(); item != allitems.end(); item++)
			((*item).second)->selected(false);
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0006", __FILE__, __FUNCSIG__); }
}

void GridModule::SetCurrentIterator(map<int,GRectangle*>::iterator d)
{
	GMitem = d;
}

void GridModule::SetCurrentIteratorOfEffectiveRCMovement(map<int,GRectangle*>::iterator d)
{
	GMitem = d;
}

void GridModule::SetCurrentIterator(GRectangle* d)
{
	try
	{
		for(GM_ITER item = allitems.begin(); item != allitems.end(); item++)
		{
			if(((*item).second)->getId() == d->getId())
				GMitem = item;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0007", __FILE__, __FUNCSIG__); }
}

void GridModule::SetCurrentIteratorOfEffectiveRCMovement(GRectangle* d)
{
	try
	{
		for(GM_ITER item = ItemsAccordingToEffectiveMovementInColumn.begin(); item != ItemsAccordingToEffectiveMovementInColumn.end(); item++)
		{
			if(((*item).second)->getId() == d->getId())
				GMitem = item;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0007", __FILE__, __FUNCSIG__); }
}

void GridModule::SetSelectedRect(GRectangle* Sel)
{
	SelectedGrect = Sel;
}

GRectangle* GridModule::getSmartCurrentHighlightedCell(double x, double y)
{
	try
	{
		for(GM_ITER item = ItemsAccordingToEffectiveMovementInColumn.begin(); item != ItemsAccordingToEffectiveMovementInColumn.end(); item++)
		{
			if(((*item).second)->Ishiglighted(x, y))
				return ((*item).second);
		}
		return NULL;
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0008", __FILE__, __FUNCSIG__); return NULL; }
}

GRectangle* GridModule::getCurrentHighlightedCell(double x, double y)
{
	try
	{
		for(GM_ITER item = allitems.begin(); item != allitems.end(); item++)
		{
			if(((*item).second)->Ishiglighted(x, y))
				return ((*item).second);
		}
		return NULL;
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM00028", __FILE__, __FUNCSIG__); return NULL; }
}

GRectangle* GridModule::getCurrentSelecteCell()
{
	return SelectedGrect;
}

bool GridModule::ShowColumnRowHeaders()
{
	return ShowRCHeaders;
}

void GridModule::AddRemoveToselectedItems(GRectangle* Sel, double x, double y, bool Add)
{
	try
	{
		if(SelectedItemsList.size() == 0)
		{
			ClearSelection();
			Sel->selected(true);
			SelectedGrect = NULL;
		}
		if(Add)
			SelectedItemsList.push_back(Sel);
		else
			SelectedItemsList.remove(Sel);
		GMSSitem = SelectedItemsList.begin();
		SortItemList(SelectedItemsList, x, y);
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0010", __FILE__, __FUNCSIG__); }
}

void GridModule::SortItemList(std::list<GRectangle*> ItemList, double x, double y)
{
	try
	{
		SortedItemsList.clear();
		GRectangle *Grect, *Grect1;
		double dis = 0, min = 0;
		double point2[2] = {x, y};
		bool firstflag = true;
		if(ItemList.size() < 3)
		{
			for each(GRectangle* gr in ItemList)
				SortedItemsList.push_back(gr);
		}
		else
		{
			for each(GRectangle* gr in ItemList)
			{
				for(int i = 0; i < 2; i ++)
					dis += pow((*(gr->GetCenter() + i) - point2[i]),2);  
				if(firstflag)
				{
					min = dis;
					firstflag = false;
					Grect1 = gr;
				}
				else if(dis < min)
				{
					min = dis;
					Grect1 = gr;
				}
				dis = 0;
			}
			SortedItemsList.push_back(Grect1);
			ItemList.remove(Grect1);
			while(ItemList.size() != 0)
			{
				dis = 0, min = 0;
				firstflag = true;
				for each(GRectangle* gr in ItemList)
				{
					for(int i = 0; i < 2; i ++)
						dis += pow((*(Grect1->GetCenter() + i) - *(gr->GetCenter() + i)),2);  
					if(firstflag)
					{
						min = dis;
						firstflag = false;
						Grect = gr;
					}
					else if(dis < min)
					{
						min = dis;
						Grect = gr;
					}
					dis = 0;
				}
				Grect1 = Grect;
				SortedItemsList.push_back(Grect1);
				ItemList.remove(Grect1);
			}
		}
		GMSitem = SortedItemsList.begin();
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0011", __FILE__, __FUNCSIG__); }
}

void GridModule::WritetoDXF(char* filename)
{
	try
	{
		this->filename = filename;
		GRectangle* Gsh;
		for(GM_ITER item = allitems.begin(); item != allitems.end(); item++)
		{
			Gsh = ((*item).second);
			AddRectangleParameter(Gsh->GetLeftTop(),Gsh->GetRightTop(), Gsh->GetBottomRight(), Gsh->GetBottomLeft());

		}
		for(GM_ITER item = RowColumnitems.begin(); item != RowColumnitems.end(); item++)
		{
			Gsh = ((*item).second);
			AddRectangleParameter(Gsh->GetLeftTop(),Gsh->GetRightTop(), Gsh->GetBottomRight(), Gsh->GetBottomLeft());
		}
		this->generateDXFFile(filename);
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0012", __FILE__, __FUNCSIG__); }
}

void GridModule::entryPoint()
{
	try
	{
		DXFDataWriter<<"0"<<endl;
		DXFDataWriter<<"SECTION"<<endl;
		DXFDataWriter<<"2"<<endl;
		DXFDataWriter<<"ENTITIES"<<endl;
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0013", __FILE__, __FUNCSIG__); }
}

void GridModule::setCommonParameter(char* str,double x1, double x2, double x3)
{
	try
	{
		DXFList.push_back("0");
		DXFList.push_back(str);
		DXFList.push_back("8");
		DXFList.push_back("0");
		DXFList.push_back("10");
		DXFList.push_back(postMinus(x1));
		DXFList.push_back("20");
		DXFList.push_back(postMinus(x2));
		DXFList.push_back("30");
		DXFList.push_back(postMinus(x3));
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0014", __FILE__, __FUNCSIG__); }
}

char* GridModule::postMinus(double x)
{
	try
	{
		if(abs(x) < 1)
		{
			_fcvt_s(&cd[0], 35, x, 10, &d, &sgn);
			_fcvt_s(&cd[abs(d) + 1], 35, x, 10, &d, &sgn);
			int i = abs(d) + 1;
			for(int j = 0; j < (abs(d) + 1); j++)
			{i--; cd[i] = '0';  }
			temp = cd;
			temp.insert(1,"."); 
		}
		else
		{
			//convert to string, string wont have the dot but the dp will have the dot position
			_fcvt_s(&cd[0], 35, x, 10, &d, &sgn);
			//copy the string from the dot position, to the next byte
			temp = cd;
			temp.insert(d,".");
		}
		if(x < 0)
		{
			temp.insert(0,"-");
		}
		strcpy(cd,(const char*)temp.c_str());
		return cd;
		
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0015", __FILE__, __FUNCSIG__); return ""; }
}

bool GridModule::generateDXFFile(char* filename)
{
	try
	{
		 filename = filename;
		 DXFDataWriter.open(filename);
		 if(!DXFDataWriter)
		 {
			 return false;
 		 }
		 entryPoint();
		 for(DXFListIterator=DXFList.begin();DXFListIterator!=DXFList.end();DXFListIterator++)
		 {
			 DXFDataWriter<<*DXFListIterator<<endl;
		 }
		 exitPoint();
		 DXFDataWriter.close();
   		 return true;
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0017", __FILE__, __FUNCSIG__); return false; }
}

void GridModule::addtextparameter( double ax,double bx,double cx,double width,double height,char* text )
{
	try
	{
		setCommonParameter("MTEXT",ax, bx, cx);
		DXFList.push_back("40");
		DXFList.push_back(postMinus(height));
		DXFList.push_back("41");
		DXFList.push_back(postMinus(width));
		DXFList.push_back("1");
		DXFList.push_back(text);
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0018", __FILE__, __FUNCSIG__); }
}

void GridModule::AddRectangleParameter(double *point1, double *point2, double *point3, double *point4)
{
	try
	{
		DXFList.push_back("0");
		DXFList.push_back("POLYLINE");
		DXFList.push_back("8");
		DXFList.push_back("0");
		DXFList.push_back("66");
		DXFList.push_back("1");
		DXFList.push_back("10");
		DXFList.push_back("0");
		DXFList.push_back("20");
		DXFList.push_back("0");
		DXFList.push_back("30");
		DXFList.push_back("0");
		setCommonParameter("VERTEX", point1[0], point1[1], point1[2]);
		setCommonParameter("VERTEX", point2[0], point2[1], point2[2]);
		setCommonParameter("VERTEX", point3[0], point3[1], point3[2]);
		setCommonParameter("VERTEX", point4[0], point4[1], point4[2]);
		setCommonParameter("VERTEX", point1[0], point1[1], point1[2]);
		DXFList.push_back("0");
		DXFList.push_back("SEQEND");
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0019", __FILE__, __FUNCSIG__); }
}

void GridModule::exitPoint()
{
	try
	{
		DXFDataWriter<<"0"<<endl;
		DXFDataWriter<<"ENDSEC"<<endl;
		DXFDataWriter<<"0"<<endl;
		DXFDataWriter<<"EOF"<<endl;
	}
	catch(...){ SetAndRaiseErrorMessage("GRIDM0020", __FILE__, __FUNCSIG__); }
}