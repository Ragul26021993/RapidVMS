#include "stdafx.h"
#include "CircleAtConeSphereIntersection.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Sphere.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddShapeAction.h"

//Constructor..//
CircleAtConeSphereIntersection::CircleAtConeSphereIntersection()
{
	try
	{
		init();
		setMaxClicks(4);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
CircleAtConeSphereIntersection::~CircleAtConeSphereIntersection()
{
	try
	{

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0002", __FILE__, __FUNCSIG__); }
}


//Initialise the settings..
void CircleAtConeSphereIntersection::init()
{
	try
	{
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mosuemove..
void CircleAtConeSphereIntersection::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down... Select the shapes..
void CircleAtConeSphereIntersection::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(getClicksDone() == 1 && Csh->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			shapeArray[0] = Csh;
		}
		else if(getClicksDone() == 2 && Csh->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
		{
			shapeArray[1] = Csh;
		}
		else if(getClicksDone() == 3 && (Csh->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || Csh->ShapeType == RapidEnums::SHAPETYPE::CIRCLE))
		{
			shapeArray[2] = Csh;
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0006", __FILE__, __FUNCSIG__); }
}

void CircleAtConeSphereIntersection::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0012", __FILE__, __FUNCSIG__); }
}

//Handle the escape button press..
void CircleAtConeSphereIntersection::EscapebuttonPress()
{
	try
	{
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0013", __FILE__, __FUNCSIG__); }
}

//Handle the max click...2 in this case
void CircleAtConeSphereIntersection::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL) //set clicks done back to prev stage
		{
			setClicksDone(1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if ((Csh->ShapeType !=  RapidEnums::SHAPETYPE::CIRCLE3D) && (Csh->ShapeType !=  RapidEnums::SHAPETYPE::CIRCLE3D))
		{
			setClicksDone(1);
			return;
		}
		shapeArray[3] = Csh;
		Vector *cen = ((Sphere*)shapeArray[1])->getCenter();
		double sC[3] = {cen->getX(), cen->getY(), cen->getZ()}, sR = ((Sphere*)shapeArray[1])->Radius();
		cen = ((Circle*)shapeArray[2])->getCenter();
		double c1[3] = {cen->getX(), cen->getY(), cen->getZ()}, r1 = ((Circle*)shapeArray[2])->Radius();
		cen = ((Circle*)shapeArray[3])->getCenter();
		double c2[3] = {cen->getX(), cen->getY(), cen->getZ()}, r2 = ((Circle*)shapeArray[3])->Radius();
		double ratio = (r1 - r2) / RMATH3DOBJECT->Distance_Point_Point(c1, c2); 
		double H = RMATH3DOBJECT->Distance_Point_Point(sC, c2) - sR;
		double parameter[3] = {0}, ans[2] = {0};
		parameter[0] = 1 + pow(ratio,2);
		parameter[1] = (2 * H * pow(ratio,2)) + (2 * r1 * ratio) - 2 *sR;
		parameter[2] = pow(r1,2) - (2 * r1 * H * ratio) + pow(H,2) * pow(ratio,2);
		int anscount = RMATH3DOBJECT->Solve_Quadratic_Equation(parameter, ans);
		if(anscount != 0)
		{
			Circle *newShape = new Circle(_T("C3D"), RapidEnums::SHAPETYPE::CIRCLE3D);
			newShape->setCenter(Vector(sC[0], sC[1], sC[2] - sR + ans[0]));
			double rad = sqrt(pow(sR,2) - pow(sR - ans[0], 2));
			newShape->Radius(rad);
			newShape->dir_l(((Circle*)shapeArray[2])->dir_l());
			newShape->dir_m(((Circle*)shapeArray[2])->dir_m());
			newShape->dir_n(((Circle*)shapeArray[2])->dir_n());
			AddShapeAction::addShape(newShape);
			if(anscount == 2)
			{
				Circle *newShape = new Circle(_T("C3D"), RapidEnums::SHAPETYPE::CIRCLE3D);
				newShape->setCenter(Vector(sC[0], sC[1], sC[2] - sR + ans[0]));
				double rad = sqrt(pow(sR,2) - pow(sR - ans[1], 2));
				newShape->Radius(rad);
				newShape->dir_l(((Circle*)shapeArray[2])->dir_l());
				newShape->dir_m(((Circle*)shapeArray[2])->dir_m());
				newShape->dir_n(((Circle*)shapeArray[2])->dir_n());
				AddShapeAction::addShape(newShape);
			}
		}
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0015", __FILE__, __FUNCSIG__); }
}