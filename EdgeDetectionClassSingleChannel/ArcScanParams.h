#pragma once


class ArcScanParams
{
public:
	ArcScanParams();
	ArcScanParams(int ScanDirection);
	~ArcScanParams();

	int ScanDir;
	double stAngle;
	double endAngle;
private:

};

ArcScanParams::ArcScanParams(int ScanDirection)
{
	ScanDir = ScanDirection;
}

ArcScanParams::ArcScanParams()
{

}

ArcScanParams::~ArcScanParams()
{
}
