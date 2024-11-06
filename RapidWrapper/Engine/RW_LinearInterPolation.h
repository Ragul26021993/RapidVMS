//Class Used for Linear Interpolation ...
#pragma once
namespace RWrapper 
{
public ref class RW_LinearInterPolation
{
	static RW_LinearInterPolation^ LinearInterPolationInstance; //hold the instance..
	cli::array<double>^ TargetPos;
	bool TargetFlag;
public:
	RW_LinearInterPolation(); //Default constructor..
	~RW_LinearInterPolation(); //Destructor.. Clear the memory..
	static RW_LinearInterPolation^ MYINSTANCE(); //Expose the class instance..
	void SetTargetPosition();
	void StartLinearInterpolation();
	void ClearAll();
	void TakePointForReference();
};
}