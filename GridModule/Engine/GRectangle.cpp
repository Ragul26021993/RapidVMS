#include "StdAfx.h"
#include "GRectangle.h"

GRectangle::GRectangle()
{
	itemid = Rnumber;
	Rnumber++;
	isvalid = false;
	isselected = false;
	VisitCount = 0;
	ImagePath = "";
}

GRectangle::~GRectangle()
{
}

bool GRectangle::Ishiglighted(double x, double y)
{
	if(x >= topleft[0] && x <= bottomright[0] && y >= bottomright[1] && y <= topleft[1]) return true;
	return false;
}

void GRectangle::CalculateEndpoints()
{
	topright[0] = bottomright[0]; topright[1] = topleft[1]; topright[2] = topleft[2];
	bottomleft[0] = topleft[0]; bottomleft[1] = bottomright[1]; bottomleft[2] = bottomright[2];
	center[0] = (bottomright[0] + topleft[0]) / 2; center[1] = (bottomright[1] + topleft[1]) / 2;
}

void GRectangle::SetLeftTop(double x, double y, double z)
{
	topleft[0] = x; topleft[1] = y; topleft[2] = z;
}

void GRectangle::SetBottomRight(double x, double y, double z)
{
	bottomright[0] = x; bottomright[1] = y; bottomright[2] = z;
}

double* GRectangle::GetLeftTop()
{
	return &topleft[0];
}

double* GRectangle::GetBottomRight()
{
	return &bottomright[0];
}

double* GRectangle::GetRightTop()
{
	return &topright[0];
}

double* GRectangle::GetBottomLeft()
{
	return &bottomleft[0];
}

double* GRectangle::GetCenter()
{
	return &center[0];
}

void GRectangle::reset()
{
	Rnumber = 0;
}

bool GRectangle::valid()
{
	return isvalid;
}

void GRectangle::valid(bool flag)
{
	isvalid = flag;
}

bool GRectangle::selected()
{
	return isselected;
}

void GRectangle::selected(bool flag)
{
	isselected = flag;
}

void GRectangle::IncrementVcount()
{
	VisitCount++;
}

int GRectangle::GetVisitCount()
{
	return VisitCount;
}

int GRectangle::getId()
{
	return itemid;
}

void GRectangle::SetName(std::string st)
{
	Rname = st;
}

std::string GRectangle::GetName()
{
	return Rname;
}

std::string GRectangle::GetImagePath()
{
	return ImagePath;
}

void GRectangle::SetImagePath(std::string Path)
{
	ImagePath = Path;
}
int GRectangle::Rnumber=0;