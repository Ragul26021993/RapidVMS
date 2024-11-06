#pragma once

namespace RWrapper 
{
public ref class RW_Probe_Entity
{
public:
	System::String^ PtType;
	System::Double Val1, Val2, Val3;
};
}
namespace RWrapper 
{
public ref class RW_Probe_Path_Entity
{
public:
	System::Double Pt_X, Pt_Y, Pt_Z;
	System::Double Feed_X, Feed_Y, Feed_Z;
	System::Double Revert_Dist, Revert_Speed;
	bool PathPt;
};
}